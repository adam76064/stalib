#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "image_utils.h"
#include "page_split/PageLayoutEstimator.h"
#include "page_split/PageLayoutAdapter.h"
#include "page_split/LayoutType.h"
#include "ImageTransformation.h"
#include "ImageMetadata.h"
#include "ProjectPages.h"
#include "imageproc/BinaryThreshold.h"

namespace py = pybind11;

struct PageSplitParams {
    std::string layout_type = "auto";
    int bw_threshold = 128;
};

struct PageSplitResult {
    std::string type;
    int num_sub_pages;
    py::list cutter_lines; // ((x1,y1),(x2,y2))
    py::list inscribed_cutter_lines;
    py::list uncut_outline;
    py::list left_page_outline;
    py::list right_page_outline;
    py::list single_page_outline;
};

using namespace page_split;

PageSplitResult estimate_layout(const py::array_t<uint8_t>& image, int dpi_x, int dpi_y, const PageSplitParams& params) {
    QImage qimg = numpyToQImage(image);
    
    // No need to convert to Grayscale8; image_utils gives us RGB32 which BinaryImage natively supports.
    
    ImageMetadata metadata(qimg.size(), Dpi(dpi_x, dpi_y));
    ImageTransformation xform(QRectF(0, 0, qimg.width(), qimg.height()), Dpi(dpi_x, dpi_y));

    LayoutType ltype = AUTO_LAYOUT_TYPE;
    if (params.layout_type == "single_uncut") ltype = SINGLE_PAGE_UNCUT;
    else if (params.layout_type == "page_plus_offcut") ltype = PAGE_PLUS_OFFCUT;
    else if (params.layout_type == "two_pages") ltype = TWO_PAGES;

    PageLayout layout = PageLayoutEstimator::estimatePageLayout(ltype, qimg, xform, imageproc::BinaryThreshold(params.bw_threshold), nullptr);

    PageSplitResult res;
    res.type = "unknown";
    if (layout.type() == PageLayout::SINGLE_PAGE_UNCUT) res.type = "single_uncut";
    else if (layout.type() == PageLayout::SINGLE_PAGE_CUT) res.type = "single_cut"; // Wait, LayoutType and PageLayout::Type are different!
    else if (layout.type() == PageLayout::TWO_PAGES) res.type = "two_pages";
    
    res.num_sub_pages = ProjectPages::adviseNumberOfLogicalPages(metadata, OrthogonalRotation());

    for (int i = 0; i < layout.numCutters(); ++i) {
        res.cutter_lines.append(lineToTuple(layout.cutterLine(i)));
    }
    for (int i = 0; i < layout.numCutters(); ++i) {
        res.inscribed_cutter_lines.append(lineToTuple(layout.inscribedCutterLine(i)));
    }
    
    res.uncut_outline = polygonToList(layout.uncutOutline());
    res.left_page_outline = polygonToList(layout.leftPageOutline());
    res.right_page_outline = polygonToList(layout.rightPageOutline());
    res.single_page_outline = polygonToList(layout.singlePageOutline());
    
    return res;
}

void bind_page_split(py::module_& m) {
    auto sub = m.def_submodule("page_split", "Page Split filter stage");
    
    py::class_<PageSplitParams>(sub, "PageSplitParams")
        .def(py::init<>())
        .def_readwrite("layout_type", &PageSplitParams::layout_type)
        .def_readwrite("bw_threshold", &PageSplitParams::bw_threshold);
        
    py::class_<PageSplitResult>(sub, "PageSplitResult")
        .def_readwrite("type", &PageSplitResult::type)
        .def_readwrite("num_sub_pages", &PageSplitResult::num_sub_pages)
        .def_readwrite("cutter_lines", &PageSplitResult::cutter_lines)
        .def_readwrite("inscribed_cutter_lines", &PageSplitResult::inscribed_cutter_lines)
        .def_readwrite("uncut_outline", &PageSplitResult::uncut_outline)
        .def_readwrite("left_page_outline", &PageSplitResult::left_page_outline)
        .def_readwrite("right_page_outline", &PageSplitResult::right_page_outline)
        .def_readwrite("single_page_outline", &PageSplitResult::single_page_outline);
        
    sub.def("estimate_layout", &estimate_layout,
            py::arg("image"), py::arg("dpi_x") = 300, py::arg("dpi_y") = 300, py::arg("params") = PageSplitParams(),
            "Estimate the page layout (split lines) for the given image.");
}
