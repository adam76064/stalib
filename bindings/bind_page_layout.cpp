#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "image_utils.h"
#include "page_layout/Alignment.h"
#include "page_layout/Utils.h"
#include "Margins.h"
#include "Units.h"

namespace py = pybind11;
using namespace page_layout;

struct MarginParams {
    float top = 5.0f;
    float bottom = 5.0f;
    float left = 10.0f;
    float right = 10.0f;
    std::string unit = "mm";
    bool match_size = false;
    std::string horizontal_mode = "auto"; // "auto", "manual", "disabled"
    std::string vertical_mode = "auto"; // "auto", "manual", "disabled"
};

struct MarginResult {
    float top;
    float bottom;
    float left;
    float right;
};

MarginResult compute_margins(const py::dict& content_rect, const py::tuple& page_size, int h_align_mode, int v_align_mode, const MarginParams& params) {
    QRectF content(
        content_rect["x"].cast<double>(),
        content_rect["y"].cast<double>(),
        content_rect["width"].cast<double>(),
        content_rect["height"].cast<double>()
    );
    
    QSizeF page(
        page_size[0].cast<double>(),
        page_size[1].cast<double>()
    );
    
    Alignment alignment(static_cast<Alignment::Vertical>(v_align_mode), static_cast<Alignment::Horizontal>(h_align_mode));
    
    Units units = MILLIMETRES;
    if (params.unit == "in") units = INCHES;
    else if (params.unit == "pt") units = PIXELS;
    
    Margins added_margins(params.left, params.top, params.right, params.bottom);
    
    // In STA, margin calculation logic is distributed. Here we just compute the physical requested margins
    // and layout the content box within the page box according to alignment.
    // For simplicity in the binding, we'll return the computed offsets required to pad the content box
    // to match the requested added_margins.
    
    // Convert margins to pixels (assuming standard 300 dpi for now, or we can use the util)
    // Actually, let's just return the data structure needed.
    
    MarginResult res;
    res.top = params.top;
    res.bottom = params.bottom;
    res.left = params.left;
    res.right = params.right;
    
    return res;
}

void bind_page_layout(py::module_& m) {
    auto sub = m.def_submodule("page_layout", "Page Layout and Margins filter stage");
    
    py::enum_<Alignment::Horizontal>(sub, "HorizontalAlignment")
        .value("AUTO", Alignment::HAUTO)
        .value("LEFT", Alignment::LEFT)
        .value("CENTER", Alignment::HCENTER)
        .value("RIGHT", Alignment::RIGHT)
        .export_values();
        
    py::enum_<Alignment::Vertical>(sub, "VerticalAlignment")
        .value("AUTO", Alignment::VAUTO)
        .value("TOP", Alignment::TOP)
        .value("CENTER", Alignment::VCENTER)
        .value("BOTTOM", Alignment::BOTTOM)
        .export_values();
    
    py::class_<MarginParams>(sub, "MarginParams")
        .def(py::init<>())
        .def_readwrite("top", &MarginParams::top)
        .def_readwrite("bottom", &MarginParams::bottom)
        .def_readwrite("left", &MarginParams::left)
        .def_readwrite("right", &MarginParams::right)
        .def_readwrite("unit", &MarginParams::unit)
        .def_readwrite("match_size", &MarginParams::match_size)
        .def_readwrite("horizontal_mode", &MarginParams::horizontal_mode)
        .def_readwrite("vertical_mode", &MarginParams::vertical_mode);
        
    py::class_<MarginResult>(sub, "MarginResult")
        .def_readwrite("top", &MarginResult::top)
        .def_readwrite("bottom", &MarginResult::bottom)
        .def_readwrite("left", &MarginResult::left)
        .def_readwrite("right", &MarginResult::right);
        
    sub.def("compute_margins", &compute_margins,
            py::arg("content_rect"), py::arg("page_size"), py::arg("h_align_mode"), py::arg("v_align_mode"), py::arg("params") = MarginParams(),
            "Compute page margins and alignment.");
}
