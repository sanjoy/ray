#ifndef RAY_OBJECTS_HPP
#define RAY_OBJECTS_HPP

#include "euclid.hpp"
#include "object.hpp"

#include <memory>

namespace ray {

class BoxObj : public Object {
  Cube _cube;
  std::array<Color, Cube::kFaceCount> _colors;

public:
  BoxObj(const Scene &scene, const Vector &center, const Vector &normal_a,
         const Vector &normal_b, double side);
  virtual bool incident(ThreadContext &, const Ray &, double, double &,
                        Color &) const override;
};

class SphericalMirrorObj : public Object {
  Sphere _sphere;
  static constexpr unsigned _max_nesting = 10;
  static unsigned max_nesting() { return SphericalMirrorObj::_max_nesting; }

public:
  SphericalMirrorObj(const Scene &scene, const Vector &center, double radius)
      : Object(scene), _sphere(center, radius) {}
  virtual bool incident(ThreadContext &, const Ray &, double, double &,
                        Color &) const override;
};

class SkyObj : public Object {
  bool _uniform = false;

public:
  SkyObj(const Scene &scene, bool uniform = false)
      : Object(scene), _uniform(uniform) {}

  virtual bool incident(ThreadContext &, const Ray &, double, double &,
                        Color &) const override;
};

class InfinitePlane : public Object {
  Plane _plane;
  double _check_size;
  Vector _axis_0, _axis_1;

public:
  InfinitePlane(const Scene &scene, Plane plane, Vector axis_0,
                double check_size)
      : Object(scene), _plane(plane), _check_size(check_size), _axis_0(axis_0),
        _axis_1(axis_0.cross_product(plane.normal())) {

    _axis_0 = _axis_0.normalize();
    _axis_1 = _axis_1.normalize();
  }

  virtual bool incident(ThreadContext &, const Ray &, double, double &,
                        Color &) const override;
};

class RefractiveBoxObj : public Object {
  static constexpr unsigned FACE_COUNT = 6;
  static constexpr double _relative_refractive_index = 1.3;

  static constexpr unsigned _max_nesting = 10;
  static unsigned max_nesting() { return RefractiveBoxObj::_max_nesting; }
  Cube _cube;

public:
  RefractiveBoxObj(const Scene &scene, const Vector &center,
                   const Vector &normal_a, const Vector &normal_b, double side);
  virtual bool incident(ThreadContext &, const Ray &, double, double &,
                        Color &) const override;
};
}

#endif
