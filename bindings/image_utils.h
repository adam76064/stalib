// image_utils.h — numpy ↔ QImage conversion utilities shared by all binding files
// All conversions happen at the pybind11 boundary; Qt types never cross into Python.

#pragma once

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <QImage>
#include <stdexcept>
#include <string>

namespace py = pybind11;

/**
 * Convert a numpy uint8 array (H×W, H×W×1, H×W×3, H×W×4) to QImage.
 * The returned QImage owns a copy of the data so the numpy array can be
 * garbage-collected after this call.
 */
inline QImage numpyToQImage(
    const py::array_t<uint8_t, py::array::c_style | py::array::forcecast>& arr) {

  auto buf = arr.request();
  if (buf.ndim < 2 || buf.ndim > 3)
    throw std::runtime_error("Image must be 2-D (grayscale) or 3-D (H×W×C)");

  const int h  = static_cast<int>(buf.shape[0]);
  const int w  = static_cast<int>(buf.shape[1]);
  const int ch = (buf.ndim == 3) ? static_cast<int>(buf.shape[2]) : 1;

  QImage::Format fmt;
  switch (ch) {
    case 1: fmt = QImage::Format_Grayscale8; break;
    case 3: fmt = QImage::Format_RGB888;     break;
    case 4: fmt = QImage::Format_RGBA8888;   break;
    default:
      throw std::runtime_error("Unsupported channel count: " + std::to_string(ch));
  }

  // QImage(data, w, h, bytesPerLine, format) — make a deep copy so numpy array
  // lifetime does not need to outlive the QImage.
  const auto* src = static_cast<const uchar*>(buf.ptr);
  const int stride = w * ch;
  QImage img(src, w, h, stride, fmt);
  
  // Convert to Format_RGB32 because ScanTailor's BinaryImage and other classes
  // strictly expect Format_RGB32, Format_ARGB32, or Format_Indexed8.
  return img.convertToFormat(QImage::Format_RGB32);
}

/**
 * Convert a QImage to a numpy uint8 array (H×W×C or H×W for grayscale).
 */
inline py::array_t<uint8_t> qimageToNumpy(const QImage& img) {
  QImage converted;
  int channels;

  switch (img.format()) {
    case QImage::Format_Grayscale8:
      converted = img;
      channels  = 1;
      break;
    case QImage::Format_RGB888:
      converted = img;
      channels  = 3;
      break;
    case QImage::Format_RGBA8888:
      converted = img;
      channels  = 4;
      break;
    default:
      // Normalize everything else to RGB888
      converted = img.convertToFormat(QImage::Format_RGB888);
      channels  = 3;
      break;
  }

  const int h = converted.height();
  const int w = converted.width();

  py::array_t<uint8_t> result;
  if (channels == 1) {
    result = py::array_t<uint8_t>({h, w});
  } else {
    result = py::array_t<uint8_t>({h, w, channels});
  }

  auto buf = result.request();
  auto* dst = static_cast<uint8_t*>(buf.ptr);

  for (int y = 0; y < h; ++y) {
    std::memcpy(dst + y * w * channels, converted.constScanLine(y), w * channels);
  }
  return result;
}

/**
 * Convert a QPolygonF to a Python list of (x, y) tuples.
 */
inline py::list polygonToList(const QPolygonF& poly) {
  py::list result;
  for (const QPointF& pt : poly)
    result.append(py::make_tuple(pt.x(), pt.y()));
  return result;
}

/**
 * Convert a QLineF to a Python tuple ((x1,y1), (x2,y2)).
 */
inline py::tuple lineToTuple(const QLineF& line) {
  return py::make_tuple(
      py::make_tuple(line.x1(), line.y1()),
      py::make_tuple(line.x2(), line.y2()));
}

/**
 * Convert a QRectF to a Python dict {x, y, width, height}.
 */
inline py::dict rectToDict(const QRectF& r) {
  py::dict d;
  d["x"]      = r.x();
  d["y"]      = r.y();
  d["width"]  = r.width();
  d["height"] = r.height();
  return d;
}
