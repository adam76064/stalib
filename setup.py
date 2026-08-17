import os
import sys
import glob
from setuptools import setup, find_packages
from pybind11.setup_helpers import Pybind11Extension, build_ext

# ---------------------------------------------------------------------------
# Dynamic Dependency Resolution (Qt5 & Boost)
# ---------------------------------------------------------------------------

def find_qt5_paths():
    """Find Qt5 include and library directories dynamically."""
    qt_candidates = [
        os.environ.get("QT_DIR"),
        os.environ.get("Qt5_DIR"),
        os.environ.get("QT5_DIR"),
        os.environ.get("QT_ROOT"),
        r"C:\Qt\5.15.2\msvc2019_64",
        r"C:\Qt\5.15.0\msvc2019_64",
        "/usr/include/qt5",
        "/usr/include/x86_64-linux-gnu/qt5",
        "/usr/local/opt/qt@5",
    ]
    
    qt_include = None
    qt_lib = None
    
    for candidate in qt_candidates:
        if candidate and os.path.exists(candidate):
            # Check for include directory
            inc = os.path.join(candidate, "include") if os.path.exists(os.path.join(candidate, "include")) else candidate
            lib = os.path.join(candidate, "lib") if os.path.exists(os.path.join(candidate, "lib")) else candidate
            if os.path.exists(inc):
                qt_include = inc
                qt_lib = lib
                break
                
    if not qt_include:
        print("[WARNING] Qt5 directory not found via environment variables (QT_DIR/Qt5_DIR) or standard paths.")
        qt_include = ""
        qt_lib = ""
        
    return qt_include, qt_lib

def find_boost_path():
    """Find Boost include directory dynamically."""
    boost_candidates = [
        os.environ.get("BOOST_ROOT"),
        os.environ.get("BOOST_INCLUDEDIR"),
        os.environ.get("BOOST_DIR"),
        r"C:\local\boost_1_82_0",
        r"C:\local\boost",
        r"C:\boost",
        "/usr/include",
        "/usr/local/include",
    ]
    
    boost_include = None
    for candidate in boost_candidates:
        if candidate and os.path.exists(candidate):
            # Check for boost header directory
            if os.path.exists(os.path.join(candidate, "boost")):
                boost_include = candidate
                break
            elif os.path.exists(os.path.join(candidate, "include", "boost")):
                boost_include = os.path.join(candidate, "include")
                break
            else:
                boost_include = candidate
                break
                
    if not boost_include:
        print("[WARNING] Boost directory not found via environment variables (BOOST_ROOT/BOOST_DIR) or standard paths.")
        boost_include = ""
        
    return boost_include

qt_include, qt_lib = find_qt5_paths()
boost_include = find_boost_path()

# ---------------------------------------------------------------------------
# C++ Source Collection
# ---------------------------------------------------------------------------

src_files = [
    "bindings/image_utils.cpp",
    "bindings/bind_fix_orientation.cpp",
    "bindings/bind_page_split.cpp",
    "bindings/bind_deskew.cpp",
    "bindings/bind_select_content.cpp",
    "bindings/bind_page_layout.cpp",
    "bindings/bind_output.cpp",
    "bindings/module.cpp",
]

dirs_to_build = [
    "lib/core",
    "lib/core/filters",
    "lib/core/filters/page_layout",
    "lib/core/filters/page_split",
    "lib/core/filters/select_content",
    "lib/core/math",
    "lib/dewarping",
    "lib/imageproc",
    "lib/math",
    "lib/math/adiff",
    "lib/math/spfit",
    "lib/foundation",
]

for d in dirs_to_build:
    for f in glob.glob(f"{d}/*.cpp"):
        if not f.endswith("moc.cpp") and not f.endswith("ZoneEditorBase.cpp"):
            f = f.replace("\\", "/")
            if "core/filters" in f and f.endswith("Utils.cpp"):
                continue
            if "core/filters" in f and f.endswith("Settings.cpp"):
                continue
            src_files.append(f)

# Include search directories
include_dirs = [
    "lib",
    "lib/core",
    "lib/core/filters",
    "lib/core/math",
    "lib/dewarping",
    "lib/imageproc",
    "lib/math",
    "lib/foundation",
]

if qt_include:
    include_dirs.extend([
        qt_include,
        os.path.join(qt_include, "QtCore"),
        os.path.join(qt_include, "QtGui"),
        os.path.join(qt_include, "QtWidgets"),
        os.path.join(qt_include, "QtXml"),
    ])

if boost_include:
    include_dirs.append(boost_include)

library_dirs = [qt_lib] if qt_lib else []
libraries = ["Qt5Core", "Qt5Gui", "Qt5Widgets", "Qt5Xml"] if sys.platform == "win32" else ["Qt5Core", "Qt5Gui", "Qt5Widgets", "Qt5Xml"]

# Platform-specific compiler & linker options
if sys.platform == "win32":
    extra_compile_args = ["/Zc:__cplusplus", "/EHsc", "/wd4819", "/wd4267", "/wd4244", "/wd4018", "/bigobj"]
    extra_link_args = []
else:
    extra_compile_args = ["-std=c++17", "-fvisibility=hidden"]
    extra_link_args = []

ext_modules = [
    Pybind11Extension(
        "stalib_cpp",
        src_files,
        include_dirs=include_dirs,
        library_dirs=library_dirs,
        libraries=libraries,
        cxx_std=17,
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
        define_macros=[("QT_CORE_LIB", None), ("QT_GUI_LIB", None), ("QT_WIDGETS_LIB", None), ("QT_XML_LIB", None)]
    ),
]

# Read long description from README.md
long_description = ""
if os.path.exists("README.md"):
    with open("README.md", "r", encoding="utf-8") as f:
        long_description = f.read()

setup(
    name="scantailor-advanced",
    version="1.0.0",
    description="ScanTailor Advanced Core Image Processing Library for Python",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="Joseph Artsimovich, 4lex4, STALib Contributors",
    url="https://github.com/adam76064/stalib",
    license="GPLv3",
    packages=["stalib"],
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    install_requires=[
        "numpy>=1.21",
        "Pillow>=9.0",
    ],
    python_requires=">=3.8",
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: Python :: 3.13",
        "Programming Language :: Python :: 3.14",
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Image Processing",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux",
        "Operating System :: MacOS",
    ],
    keywords="scantailor image-processing document-scanner deskew binarization ocr layout-analysis",
)
