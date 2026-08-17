// DebugImagesImpl.h — no-op stub for library builds (dbg=nullptr in all API calls)
#ifndef SCANTAILOR_CORE_DEBUGIMAGESIMPL_H_
#define SCANTAILOR_CORE_DEBUGIMAGESIMPL_H_
#include "imageproc/DebugImages.h"
#include <functional>
class DebugImagesImpl : public DebugImages {
 public:
  void add(const QImage&, const QString&, const std::function<QWidget*(const QImage&)>& = {}) override {}
  void add(const imageproc::BinaryImage&, const QString&, const std::function<QWidget*(const QImage&)>& = {}) override {}
  bool empty() const override { return true; }
  void retrieveNext(QString* = nullptr, std::function<QWidget*(const QImage&)>* = nullptr) override {}
};
#endif
