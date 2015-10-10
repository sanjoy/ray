#ifndef RAY_EUCLID_HPP
#define RAY_EUCLID_HPP

#include <array>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <utility>

namespace ray {

struct Ruler {
  template <unsigned op_count = 1> static bool is_zero(double d) {
    return fabs(d) < (op_count * std::numeric_limits<double>::round_error());
  }

  template <unsigned op_count = 1> static bool is_negative(double d) {
    return d < 0.0;
  }

  template <unsigned op_count = 1> static bool is_equal(double d0, double d1) {
    return is_zero<op_count>(d0 - d1);
  }

  static double epsilon() {
    return std::numeric_limits<double>::round_error() * 750;
  }
};

class Vector {
  double _i = 0.0;
  double _j = 0.0;
  double _k = 0.0;

public:
  Vector() {}
  explicit Vector(double i, double j, double k) : _i(i), _j(j), _k(k) {}

  static Vector get_i() { return Vector(1.0, 0.0, 0.0); }
  static Vector get_j() { return Vector(0.0, 1.0, 0.0); }
  static Vector get_k() { return Vector(0.0, 0.0, 1.0); }

  double i() const { return _i; }
  double j() const { return _j; }
  double k() const { return _k; }

  bool is_zero() const {
    return Ruler::is_zero(i()) && Ruler::is_zero(j()) && Ruler::is_zero(k());
  }

  bool operator==(const Vector &o) const {
    return Ruler::is_equal(i(), o.i()) && Ruler::is_equal(j(), o.j()) &&
           Ruler::is_equal(k(), o.k());
  }

  void print(std::ostream &out) const {
    out << "[ " << i() << " " << j() << " " << k() << " ]";
  }

  Vector operator+(const Vector &other) const {
    return Vector(i() + other.i(), j() + other.j(), k() + other.k());
  }

  Vector operator-(const Vector &other) const {
    return Vector(i() - other.i(), j() - other.j(), k() - other.k());
  }

  Vector operator-() const { return Vector(-i(), -j(), -k()); }

  double operator*(const Vector &other) const {
    return i() * other.i() + j() * other.j() + k() * other.k();
  }

  Vector operator*(double v) const { return Vector(v * i(), v * j(), v * k()); }

  Vector cross_product(const Vector &v) const {
    return Vector(j() * v.k() - k() * v.j(), k() * v.i() - i() * v.k(),
                  i() * v.j() - j() * v.i());
  }

  bool get_scale(const Vector &v, double &result) const {
    bool found = false;
    for (auto &p : {std::make_pair(i(), v.i()), std::make_pair(j(), v.j()),
                    std::make_pair(k(), v.k())}) {
      if (found) {
        if (!Ruler::is_equal(p.second * result, p.first))
          return false;
        continue;
      }

      if (Ruler::is_zero(p.second)) {
        if (!Ruler::is_zero(p.first))
          return false;
        continue;
      }

      found = true;
      result = p.first / p.second;
    }
    return true;
  }

  double horizontal_gradient() const {
    return k() / std::sqrt(i() * i() + j() * j());
  }

  double mag() const { return std::sqrt(i() * i() + j() * j() + k() * k()); }

  double dist(const Vector &other) const { return (*this - other).mag(); }

  Vector normalize() const {
    assert(!Ruler::is_zero(mag()) && "Cannot normalize a zero vector!");
    return (*this) * (1.0 / mag());
  }

  Vector rotate(double radian, const Vector &orth) {
    assert(Ruler::is_zero(*this * orth) && "precondition!");

    Vector orthonormal = orth.normalize();
    Vector normal_in_rotation_plane = this->cross_product(orthonormal);
    return ((*this) * std::cos(radian) +
            normal_in_rotation_plane * std::sin(radian));
  }
};

inline std::ostream &operator<<(std::ostream &out, const Vector &v) {
  v.print(out);
  return out;
}

inline Vector operator*(double d, const Vector &v) { return v * d; }

// Represents the ray offset() + k * direction(), propagating towards increasing
// k.
class Ray {
  Vector _direction;
  Vector _offset;

  explicit Ray(Vector o, Vector d) : _direction(d), _offset(o) {}

public:
  static Ray from_two_points(Vector from, Vector to) {
    return Ray(from, to - from);
  }

  static Ray from_offset_and_direction(Vector offset, Vector direction) {
    return Ray(offset, direction);
  }

  void print(std::ostream &out) const {
    out << "O: " << offset() << " D: " << direction();
  }

  const Vector &direction() const { return _direction; }
  const Vector &offset() const { return _offset; }

  Vector at(double k) const { return offset() + k * direction(); }

  bool intersect(const Ray &r, double &k_self, double &k_other) const {
    //     O + k * D  == O' + k' * D'
    // =>  k * D      == (O' - O) + k' * D'
    // =>  k * D X D' == (O' - O) X D'
    // =>  k          == ((O' - O) X D') / (D X D')

    Vector numerator = (r.offset() - offset()).cross_product(r.direction());
    Vector denominator = direction().cross_product(r.direction());

    if (!numerator.get_scale(denominator, k_self))
      return false;

    numerator = offset() + k_self * direction() - r.offset();
    return numerator.get_scale(r.direction(), k_other);
  }

  bool contains(const Vector &v, double &out_k) const {
    if (direction().is_zero())
      return false;
    return (v - offset()).get_scale(direction(), out_k);
  }
};

inline std::ostream &operator<<(std::ostream &out, const Ray &r) {
  r.print(out);
  return out;
}

// Infinite plane in 3D space: all p such that (p - point()) * normal() == 0.
class Plane {
  Vector _normal;
  Vector _point;

public:
  Plane() {}
  explicit Plane(const Vector &norm, const Vector &p)
      : _normal(norm), _point(p) {}

  explicit Plane(const std::array<Vector, 3> &pts) {
    _normal = (pts[1] - pts[0]).cross_product(pts[2] - pts[0]).normalize();
    _point = pts[0];
  }

  const Vector &normal() const { return _normal; }
  const Vector &point() const { return _point; }

  // Returns true if r intersects the plane at exactly one point.
  bool intersect(const Ray &r, double &out) const {
    double denom = normal() * r.direction();

    if (Ruler::is_zero(denom))
      return false;

    out = ((point() - r.offset()) * normal()) / denom;
    return true;
  }

  bool contains(const Vector &p) const {
    return Ruler::is_zero((p - point()) * normal());
  }
};

// Finite convex segment of a plane
// Note: this current implemention of this is buggy.
class ConvexPlaneSegment {
  Plane _container;
  std::vector<Vector> _points;

public:
  ConvexPlaneSegment() {}
  explicit ConvexPlaneSegment(const Plane &ctr, const std::vector<Vector> &&pts)
      : _container(ctr), _points(pts) {
#ifndef NDEBUG
    for (const Vector &v : points())
      assert(container().contains(v));
#endif
  }

  const Plane &container() const { return _container; }
  const std::vector<Vector> &points() const { return _points; }

  bool intersect(const Ray &r, double &out) const {
    if (!container().intersect(r, out))
      return false;

    unsigned intersect_count = 0;

    Vector intersection_point = r.at(out);
    if (intersection_point == container().point())
      return true;

    Ray outgoing_ray =
        Ray::from_two_points(intersection_point, container().point());

    for (unsigned i = 0, e = points().size(); i != e; ++i) {
      const Vector &p_from = points()[i];
      const Vector &p_to = points()[i == (e - 1) ? 0 : (i + 1)];

      double k_boundary, k_ray;
      Ray r = Ray::from_two_points(p_from, p_to);

      if (r.intersect(outgoing_ray, k_boundary, k_ray) && k_boundary >= 0.0 &&
          k_boundary < 1.0 && k_ray >= 0.0)
        intersect_count++;
    }

    return intersect_count % 2 == 1;
  }
};

// Finite convex segment of a plane
class RectanglePlaneSegment {
  Plane _container;
  std::array<Vector, 3> _pts;
  Vector _orth_0, _orth_1;
  double _orth_0_begin, _orth_0_end;
  double _orth_1_begin, _orth_1_end;

public:
  RectanglePlaneSegment() {}
  explicit RectanglePlaneSegment(const std::array<Vector, 3> &pts)
      : _container(pts), _pts(pts) {
#ifndef NDEBUG
    for (const Vector &v : points())
      assert(container().contains(v));
#endif

    _orth_0 = (points()[0] - points()[1]).normalize();
    _orth_1 = (points()[2] - points()[1]).normalize();

    _orth_0_begin = points()[1] * _orth_0;
    _orth_0_end = points()[0] * _orth_0;

    _orth_1_begin = points()[1] * _orth_1;
    _orth_1_end = points()[2] * _orth_1;

    assert(Ruler::is_zero(_orth_0 * _orth_1));
  }

  const Plane &container() const { return _container; }
  const std::array<Vector, 3> &points() const { return _pts; }

  bool intersect(const Ray &r, double &out) const {
    if (!container().intersect(r, out))
      return false;

    Vector isection = r.at(out);

    double orth_0_component = isection * _orth_0;
    if (orth_0_component > _orth_0_end || orth_0_component < _orth_0_begin)
      return false;

    double orth_1_component = isection * _orth_1;
    return (orth_1_component <= _orth_1_end &&
            orth_1_component >= _orth_1_begin);
  }
};

class Sphere {
  Vector _center;
  double _radius;
  Vector _center_normal;
  double _rhs;

public:
  Sphere() {}

  Sphere(const Vector &center, double radius)
      : _center(center), _radius(radius), _center_normal(_center.normalize()) {
    _rhs = center * center - radius * radius;
  }

  const Vector &center() const { return _center; }
  double radius() const { return _radius; }

  bool intersect(const Ray &r, double &out) {
    // k^2 * a + k * b + c = 0

    double a = r.direction() * r.direction();
    double b =
        2 * (r.direction() * r.offset()) - 2 * (r.direction() * center());
    double c = r.offset() * r.offset() - 2 * r.offset() * center() + _rhs;

    assert(!Ruler::is_zero(a) && "Direction vector has unit length!");

    double disc_sqr = b * b - 4 * a * c;
    if (Ruler::is_negative(disc_sqr))
      return false;

    double disc = std::sqrt(disc_sqr);
    double k1 = (-b + disc) / (2 * a);
    double k2 = (-b - disc) / (2 * a);

    out = k1 < k2 ? k1 : k2;
    return true;
  }
};
}

#endif
