#ifndef RAY_OBJECT_HPP
#define RAY_OBJECT_HPP

#include "bitmap.hpp"

#include <cmath>

namespace ray {

class Scene;

class Vector {
  double _i = 0.0;
  double _j = 0.0;
  double _k = 0.0;

public:
  Vector() {}
  explicit Vector(double i, double j, double k) : _i(i), _j(j), _k(k) {}

  double i() const { return _i; }
  double j() const { return _j; }
  double k() const { return _k; }

  Vector operator+(const Vector &other) const {
    return Vector(i() + other.i(), j() + other.j(), k() + other.k());
  }

  Vector operator-(const Vector &other) const {
    return Vector(i() - other.i(), j() - other.j(), k() - other.k());
  }

  Vector operator-() const {
    return Vector(- i(), - j(), - k());
  }

  double operator*(const Vector &other) const {
    return i() * other.i() + j() * other.j() + k() * other.k();
  }

  double mag() const {
    return std::sqrt((*this) * (*this));
  }
};


// Represents the ray offset() + k * direction(), propagating towards increasing
// k.
class Ray {
  Vector _direction;
  Vector _offset;

public:
  explicit Ray(Vector d, Vector o) : _direction(d), _offset(o) {}

  const Vector &direction() const { return _direction; }
  const Vector &offset() const { return _offset; }
};

class Object {
public:
  virtual bool incident(const Scene &scene, const Ray &r,
                        double &out_incidence_k, Color &out_pixel) = 0;
};

}

#endif
