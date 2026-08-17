#include <pybind11/pybind11.h>

namespace py = pybind11;

// Forward declarations for submodule binders
void bind_fix_orientation(py::module_& m);
void bind_page_split(py::module_& m);
void bind_deskew(py::module_& m);
void bind_select_content(py::module_& m);
void bind_page_layout(py::module_& m);
void bind_output(py::module_& m);

PYBIND11_MODULE(stalib_cpp, m) {
    m.doc() = "ScanTailor Advanced C++ algorithms — Python bindings";
    
    bind_fix_orientation(m);
    bind_page_split(m);
    bind_deskew(m);
    bind_select_content(m);
    bind_page_layout(m);
    bind_output(m);
}
