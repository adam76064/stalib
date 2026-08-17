#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "image_utils.h"
#include "select_content/ContentBoxFinder.h"
#include "select_content/PageFinder.h"
#include "ImageTransformation.h"
#include "ImageMetadata.h"
#include "imageproc/Dpi.h"
#include "FilterData.h"
#include "foundation/TaskStatus.h"

struct DummyTaskStatus : public TaskStatus {
    void cancel() override {}
    bool isCancelled() const override { return false; }
    void throwIfCancelled() const override {}
};

namespace py = pybind11;
using namespace select_content;

struct ContentParams {
    bool enable_page_detection = true;
    bool enable_fine_tuning = true;
    int dpi_x = 300;
    int dpi_y = 300;
};

struct ContentResult {
    py::dict content_rect;
    py::dict page_rect;
    bool has_content;
};

ContentResult find_content(const py::array_t<uint8_t>& image, const ContentParams& params) {
    QImage qimg = numpyToQImage(image);
    
    // No need to convert to Grayscale8; image_utils gives us RGB32 which BinaryImage natively supports.
    
    ImageTransformation xform(QRectF(0, 0, qimg.width(), qimg.height()), Dpi(params.dpi_x, params.dpi_y));
    FilterData filter_data(qimg);
    FilterData data(filter_data, xform);
    DummyTaskStatus status;
    
    QRectF page_rect;
    if (params.enable_page_detection) {
        page_rect = PageFinder::findPageBox(status, data, params.enable_fine_tuning, qimg.size(), 0.0, nullptr);
    } else {
        page_rect = QRectF(0, 0, qimg.width(), qimg.height());
    }
    
    QRectF content_rect = ContentBoxFinder::findContentBox(status, data, page_rect, nullptr);
    
    ContentResult out;
    out.has_content = !content_rect.isEmpty();
    if (out.has_content) {
        out.content_rect = rectToDict(content_rect);
    } else {
        out.content_rect = rectToDict(QRectF());
    }
    out.page_rect = rectToDict(page_rect);
    
    return out;
}

void bind_select_content(py::module_& m) {
    auto sub = m.def_submodule("select_content", "Select Content filter stage");
    
    py::class_<ContentParams>(sub, "ContentParams")
        .def(py::init<>())
        .def_readwrite("enable_page_detection", &ContentParams::enable_page_detection)
        .def_readwrite("enable_fine_tuning", &ContentParams::enable_fine_tuning)
        .def_readwrite("dpi_x", &ContentParams::dpi_x)
        .def_readwrite("dpi_y", &ContentParams::dpi_y);
        
    py::class_<ContentResult>(sub, "ContentResult")
        .def_readwrite("content_rect", &ContentResult::content_rect)
        .def_readwrite("page_rect", &ContentResult::page_rect)
        .def_readwrite("has_content", &ContentResult::has_content);
        
    sub.def("find_content", &find_content,
            py::arg("image"), py::arg("params") = ContentParams(),
            "Find the content bounding box and page outline in the given image.");
}
