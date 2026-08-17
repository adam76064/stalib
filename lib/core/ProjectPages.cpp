#include "ProjectPages.h"
int ProjectPages::adviseNumberOfLogicalPages(const ImageMetadata& metadata, OrthogonalRotation rotation) {
  const QSize size(rotation.rotate(metadata.size()));
  const QSize dpi(rotation.rotate(metadata.dpi().toSize()));
  return (size.width() * dpi.height() > size.height() * dpi.width()) ? 2 : 1;
}
