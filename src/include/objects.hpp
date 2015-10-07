#ifndef RAY_OBJECTS_HPP
#define RAY_OBJECTS_HPP

#include "euclid.hpp"
#include "object.hpp"

#include <memory>

namespace ray {

class BoxObject : public Object {
public:
  static constexpr unsigned FACE_COUNT = 6;
  BoxObject(const Vector &center, const Vector &normal_a,
            const Vector &normal_b, double side);
  virtual bool incident(const Scene &, const Ray &, double,
                        double &, Color &) override;

private:
  RectanglePlaneSegment _faces[FACE_COUNT];
  Color _colors[FACE_COUNT];
};

class SphericalMirror : public Object {
  Sphere _sphere;

public:
  SphericalMirror(const Vector &center, double radius) :
    _sphere(center, radius) {}
  virtual bool incident(const Scene &, const Ray &, double,
                        double &, Color &) override;
};

class SkyObject : public Object {
  bool _uniform = false;

public:
  SkyObject(bool uniform = false) : _uniform(uniform) {}

  virtual bool incident(const Scene &, const Ray &, double,
                        double &, Color &) override;
};

}

#endif
