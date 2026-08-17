#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "image_utils.h"
#include <QTransform>

namespace py = pybind11;

/**
 * Apply 0, 90, 180, or 270 degree rotation to the image.
 */
py::array_t<uint8_t> apply_fix_orientation(const py::array_t<uint8_t>& image, int degrees) {
    if (degrees % 90 != 0) {
        throw std::invalid_argument("Degrees must be a multiple of 90.");
    }
    
    QImage qimg = numpyToQImage(image);
    
    // Normalize to [0, 360)
    degrees = degrees % 360;
    if (degrees < 0) degrees += 360;

    if (degrees == 0) {
        return image; // no rotation needed
    }

    QTransform xform;
    xform.rotate(degrees);
    QImage rotated = qimg.transformed(xform);
    return qimageToNumpy(rotated);
}

void bind_fix_orientation(py::module_& m) {
    auto sub = m.def_submodule("fix_orientation", "Fix Orientation filter stage");
    
    sub.def("fix_orientation", &apply_fix_orientation, 
            py::arg("image"), py::arg("degrees"),
            "Apply 0, 90, 180, or 270 degree rotation to the given image.");
}
