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
  static constexpr unsigned _max_nesting = 10;
  unsigned _current_nesting = 0;

  static unsigned max_nesting() { return SphericalMirrorObj::_max_nesting; }

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

class RefractiveBoxObj : public Object {
  static constexpr unsigned FACE_COUNT = 6;
  std::array<RectanglePlaneSegment, FACE_COUNT> _faces;
  static constexpr double _relative_refractive_index = 1.5;

  static constexpr unsigned _max_nesting = 10;
  unsigned _current_nesting = 0;

  static unsigned max_nesting() { return RefractiveBoxObj::_max_nesting; }

public:
  RefractiveBoxObj(const Vector &center, const Vector &normal_a,
                   const Vector &normal_b, double side);
  virtual bool incident(const Scene &, const Ray &, double, double &,
                        Color &) override;

  std::array<RectanglePlaneSegment, FACE_COUNT> &faces() { return _faces; }
};
}

#endif
