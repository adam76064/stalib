#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "image_utils.h"
#include "imageproc/SkewFinder.h"
#include "imageproc/BinaryThreshold.h"
#include "imageproc/BinaryImage.h"
#include "imageproc/Grayscale.h"
#include "imageproc/Transform.h"
#include "imageproc/Morphology.h"
#include "imageproc/ReduceThreshold.h"
#include "imageproc/UpscaleIntegerTimes.h"
#include "imageproc/SeedFill.h"
#include "imageproc/RasterOp.h"
#include "imageproc/Dpi.h"
#include "ImageTransformation.h"

namespace py = pybind11;
using namespace imageproc;

struct SkewResult {
    float angle;
    float confidence;
};

// This mirrors exactly Task::cleanup() from the original ScanTailor Advanced.
// Removes horizontal shadow bands from the binarized image before skew detection.
static void cleanup_for_skew(BinaryImage& image, const Dpi& dpi) {
    Dpi reducedDpi(dpi);
    BinaryImage reducedImage;

    {
        ReduceThreshold reductor(image);
        while (reducedDpi.horizontal() >= 200 && reducedDpi.vertical() >= 200) {
            reductor.reduce(2);
            reducedDpi = Dpi(reducedDpi.horizontal() / 2, reducedDpi.vertical() / 2);
        }
        reducedImage = reductor.image();
    }

    // Compute brick size from 150dpi reference: 200x14 at 150dpi
    auto from150dpi = [](int size, int targetDpi) -> int {
        int newSize = (size * targetDpi + 75) / 150;
        return std::max(newSize, 1);
    };

    int brickW = from150dpi(200, reducedDpi.horizontal());
    int brickH = from150dpi(14, reducedDpi.vertical());
    QSize brick(brickW, brickH);

    BinaryImage opened(openBrick(reducedImage, brick, BLACK));
    reducedImage.release();

    BinaryImage seed(upscaleIntegerTimes(opened, image.size(), WHITE));
    opened.release();

    BinaryImage garbage(seedFill(seed, image, CONN8));
    seed.release();

    rasterOp<RopSubtract<RopDst, RopSrc>>(image, garbage);
}

SkewResult find_skew(const py::array_t<uint8_t>& image, int dpi_x = 300, int dpi_y = 300) {
    QImage qimg = numpyToQImage(image);

    // Convert to true grayscale for better Otsu threshold accuracy.
    QImage gray = toGrayscale(qimg);

    // Use Otsu's method for binarization, exactly like STA.
    BinaryThreshold thresh = BinaryThreshold::otsuThreshold(gray);
    BinaryImage bin_img(gray, thresh);

    // Run the cleanup step to remove horizontal shadow bands.
    Dpi dpi(dpi_x, dpi_y);
    cleanup_for_skew(bin_img, dpi);

    // Set the resolution ratio on the SkewFinder, exactly like STA does.
    SkewFinder skew_finder;
    skew_finder.setResolutionRatio((double)dpi_x / dpi_y);

    Skew res = skew_finder.findSkew(bin_img);

    float effective_angle = 0.0f;
    // STA only accepts angles above GOOD_CONFIDENCE threshold.
    // The angle is negated because we want to counter-rotate.
    if (res.confidence() >= Skew::GOOD_CONFIDENCE) {
        effective_angle = static_cast<float>(-res.angle());
    }

    return { effective_angle, static_cast<float>(res.confidence()) };
}

py::array_t<uint8_t> apply_deskew(const py::array_t<uint8_t>& image, float angle_deg, int dpi_x = 300, int dpi_y = 300) {
    if (angle_deg == 0.0f) {
        return image;
    }
    QImage qimg = numpyToQImage(image);

    // Replicate the exact ImageTransformation pipeline from STA.
    ImageTransformation xform(QRectF(0, 0, qimg.width(), qimg.height()), Dpi(dpi_x, dpi_y));
    xform.setPostRotation(angle_deg);

    QImage deskewed = imageproc::transform(
        qimg,
        xform.transform(),
        xform.resultingRect().toAlignedRect(),
        imageproc::OutsidePixels::assumeColor(QColor(255, 255, 255))
    );
    return qimageToNumpy(deskewed);
}

void bind_deskew(py::module_& m) {
    auto sub = m.def_submodule("deskew", "Deskew filter stage");

    py::class_<SkewResult>(sub, "SkewResult")
        .def_readwrite("angle", &SkewResult::angle)
        .def_readwrite("confidence", &SkewResult::confidence);

    sub.def("find_skew", &find_skew,
            py::arg("image"), py::arg("dpi_x") = 300, py::arg("dpi_y") = 300,
            "Find the skew angle of the image (replicating original STA pipeline). "
            "Returns angle in degrees to be passed to apply_deskew. "
            "Returns 0 if confidence is below threshold.");

    sub.def("apply_deskew", &apply_deskew,
            py::arg("image"), py::arg("angle_deg"), py::arg("dpi_x") = 300, py::arg("dpi_y") = 300,
            "Apply the given deskew angle (in degrees) to the image.");
}
