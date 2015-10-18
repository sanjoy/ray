#ifndef RAY_OBJECTS_HPP
#define RAY_OBJECTS_HPP

#include "euclid.hpp"
#include "object.hpp"

#include <memory>

namespace ray {

class BoxObj : public Object {
public:
  static constexpr unsigned FACE_COUNT = 6;
  BoxObj(const Vector &center, const Vector &normal_a, const Vector &normal_b,
         double side);
  virtual bool incident(const Scene &, const Ray &, double, double &,
                        Color &) override;

private:
  std::array<RectanglePlaneSegment, FACE_COUNT> _faces;
  std::array<Color, FACE_COUNT> _colors;
};

class SphericalMirrorObj : public Object {
  Sphere _sphere;
  unsigned _max_nesting = 10;
  unsigned _current_nesting = 0;

public:
  SphericalMirrorObj(const Vector &center, double radius)
      : _sphere(center, radius) {}
  virtual bool incident(const Scene &, const Ray &, double, double &,
                        Color &) override;
};

class SkyObj : public Object {
  bool _uniform = false;

public:
  SkyObj(bool uniform = false) : _uniform(uniform) {}

  virtual bool incident(const Scene &, const Ray &, double, double &,
                        Color &) override;
};
}

#endif
