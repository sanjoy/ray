#ifndef RAY_OBJECTS_HPP
#define RAY_OBJECTS_HPP

#include "euclid.hpp"
#include "object.hpp"

#include <memory>

namespace ray {

class BoxObject : public Object {
  ConvexPlaneSegment _plane;

public:
  BoxObject();
  virtual bool incident(const Scene &, const Ray &, double &, Color &) override;
};

class SkyObject : public Object {
public:
  virtual bool incident(const Scene &, const Ray &, double &, Color &) override;
};

}

#endif
