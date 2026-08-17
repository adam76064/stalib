// ProjectPages.h — minimal stub: only adviseNumberOfLogicalPages() is used by algorithms
#ifndef SCANTAILOR_CORE_PROJECTPAGES_H_
#define SCANTAILOR_CORE_PROJECTPAGES_H_
#include "ImageMetadata.h"
#include "OrthogonalRotation.h"
class ProjectPages {
 public:
  static int adviseNumberOfLogicalPages(const ImageMetadata& metadata, OrthogonalRotation rotation);
};
#endif
