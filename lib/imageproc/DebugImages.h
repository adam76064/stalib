// DebugImages.h — adapted for STALib: boost::function replaced with std::function
#ifndef SCANTAILOR_IMAGEPROC_DEBUGIMAGES_H_
#define SCANTAILOR_IMAGEPROC_DEBUGIMAGES_H_
#include <QString>
#include <deque>
#include <functional>
class QImage;
class QWidget;
namespace imageproc { class BinaryImage; }
class DebugImages {
 public:
  virtual ~DebugImages() = default;
  virtual void add(const QImage& img, const QString& label,
                   const std::function<QWidget*(const QImage&)>& factory = {}) = 0;
  virtual void add(const imageproc::BinaryImage& img, const QString& label,
                   const std::function<QWidget*(const QImage&)>& factory = {}) = 0;
  virtual bool empty() const = 0;
  virtual void retrieveNext(QString* label = nullptr,
                            std::function<QWidget*(const QImage&)>* factory = nullptr) = 0;
};
#endif
