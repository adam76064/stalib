# STALib (ScanTailor Advanced Library)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Python: >=3.8](https://img.shields.io/badge/python-3.8+-blue.svg)](https://www.python.org/)

**STALib** is a high-performance Python image processing library that encapsulates the battle-tested core algorithms of [ScanTailor Advanced](https://github.com/4lex4/scantailor-advanced) (originally authored by Joseph Artsimovich and 4lex4). 

All image processing algorithms are compiled down to optimized C++ binaries with native `pybind11` bindings, operating directly on standard NumPy `uint8` arrays and PIL Images.

---

## Key Features

- 🔄 **Fix Orientation**: Lossless orthogonal rotations (90°, 180°, 270°).
- 📖 **Page Split**: Intelligent layout classification (`single_page`, `two_pages`, `page_plus_offcut`) with automatic gutter/cutter line detection.
- 📐 **Deskew**: High-precision line-based skew estimation with horizontal shadow removal, confidence scoring, and mathematical rotation.
- 🎯 **Select Content**: Robust heuristic content bounding box and page boundary detection.
- 📏 **Page Layout**: Physical margin calculation, page dimension standardization, and multi-page size matching for batch jobs.
- 🖨️ **Output Processing**: Production-grade binarization (**Otsu**, **Sauvola**, **Wolf**), despeckling, morphological/Savitzky-Golay smoothing, and dewarping.

---

## Installation

### From PyPI (Precompiled Wheels)
```bash
pip install stalib
```

### Building from Source

**Requirements:**
- Python >= 3.8
- C++17 compliant compiler (MSVC 2019+ on Windows, GCC 8+ or Clang on Linux/macOS)
- Qt 5 development headers and libraries (`Qt5Core`, `Qt5Gui`, `Qt5Xml`)
- Boost C++ Libraries (header-only math and geometry)

```bash
# Clone the repository
git clone https://github.com/adam76064/stalib.git
cd stalib

# Install dependencies
pip install pybind11 numpy Pillow setuptools wheel

# Build and install locally
pip install .
```

*Note on Windows / Custom Paths:* You can point to custom Qt or Boost installations using environment variables before building:
```powershell
$env:QT_DIR="C:\Qt\5.15.2\msvc2019_64"
$env:BOOST_ROOT="C:\local\boost_1_82_0"
pip install .
```

---

## Quickstart & API Reference

All images in STALib are passed as standard NumPy `uint8` arrays (e.g. from `np.array(Image.open(...))` or OpenCV).

### 1. Complete End-to-End Pipeline
```python
import numpy as np
from PIL import Image
from stalib import Pipeline

# Load image
img = Image.open("document_scan.jpg").convert("RGB")
img_np = np.array(img)

# Initialize pipeline (default 300 DPI)
pipeline = Pipeline(dpi=300)

# Process all 6 stages automatically
result_np = pipeline.process(img_np)

# Save output
Image.fromarray(result_np).save("document_clean.png")
```

---

### 2. Stage-by-Stage Processing

#### Stage 1: Fix Orientation
```python
import stalib_cpp

# Rotate by 0, 90, 180, or 270 degrees
rotated_image = stalib_cpp.fix_orientation.fix_orientation(image_np, 90)
```

#### Stage 2: Page Split
```python
import stalib_cpp

params = stalib_cpp.page_split.PageSplitParams()
params.layout_type = "auto"  # "auto", "single_uncut", "two_pages", "page_plus_offcut"

# Detect layout and cutter lines
split_result = stalib_cpp.page_split.estimate_layout(image_np, dpi_x=300, dpi_y=300, params=params)
print("Detected layout:", split_result.type)
print("Cutter line endpoints:", split_result.cutter_lines)
```

#### Stage 3: Deskew
```python
import stalib_cpp

# 1. Detect skew angle with confidence rating
skew_info = stalib_cpp.deskew.find_skew(image_np, dpi_x=300, dpi_y=300)
print(f"Angle: {skew_info.angle:.2f}°, Confidence: {skew_info.confidence:.2f}")

# 2. Apply deskew (angle is in degrees)
deskewed_np = stalib_cpp.deskew.apply_deskew(image_np, skew_info.angle, dpi_x=300, dpi_y=300)
```

#### Stage 4: Select Content
```python
import stalib_cpp

params = stalib_cpp.select_content.ContentParams()
params.enable_page_detection = True
params.enable_fine_tuning = True

content_info = stalib_cpp.select_content.find_content(image_np, params)
if content_info.has_content:
    print("Content Bounding Box:", content_info.content_rect)  # dict with x, y, width, height
    print("Page Outline Box:", content_info.page_rect)
```

#### Stage 5: Page Layout & Margins
```python
import stalib_cpp

params = stalib_cpp.page_layout.MarginParams()
params.top = 10.0     # in millimeters
params.bottom = 10.0
params.left = 15.0
params.right = 15.0
params.unit = "mm"
params.match_size = True

h, w = image_np.shape[:2]
content_rect = {"x": w * 0.1, "y": h * 0.1, "width": w * 0.8, "height": h * 0.8}

# Compute final layout dimensions and alignment
layout = stalib_cpp.page_layout.compute_margins(
    content_rect=content_rect,
    page_size=(w, h),
    alignment_h=stalib_cpp.page_layout.HorizontalAlignment.CENTER,
    alignment_v=stalib_cpp.page_layout.VerticalAlignment.CENTER,
    params=params
)
print("Calculated Margins (px):", layout.top, layout.bottom, layout.left, layout.right)
```

#### Stage 6: Output & Binarization
```python
import stalib_cpp

params = stalib_cpp.output.OutputParams()
params.mode = "bw"                      # "bw", "color", "mixed"
params.binarization = "otsu"            # "otsu", "sauvola", "wolf"
params.threshold_adjustment = 0         # Thinner (-50) to Thicker (+50)
params.despeckle = 1.0                  # Despeckle intensity (0.0 to 3.0)
params.fill_margins = True
params.fill_offcut = False
params.normalize_illumination = True
params.morphological_smoothing = False
params.savitzky_golay_smoothing = False

content_rect = {"x": 0, "y": 0, "width": w, "height": h}
page_rect = {"x": 0, "y": 0, "width": w, "height": h}

result = stalib_cpp.output.generate_output(image_np, content_rect, page_rect, params)
if result.has_primary:
    final_output_np = result.primary
```

---

## License & Credits

- **License**: GNU General Public License v3.0 ([GPLv3](LICENSE))
- **Original Authors**: Joseph Artsimovich, 4lex4 ([ScanTailor Advanced](https://github.com/4lex4/scantailor-advanced))
- **Python Bindings & Modular Engine**: STALib Open Source Community
