#ifndef RAY_EUCLID_HPP
#define RAY_EUCLID_HPP

/// euclid.hpp: geometry utilities.

#include <array>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <utility>

namespace ray {

struct Ruler {
  static bool is_zero(double d) {
    return fabs(d) < std::numeric_limits<double>::round_error();
  }

  static bool is_negative(double d) { return d < 0.0; }

  static bool is_equal(double d0, double d1) { return is_zero(d0 - d1); }

  static double epsilon() { return 1E-55; }
};

class Vector {
  double _i = 0.0;
  double _j = 0.0;
  double _k = 0.0;
  bool _is_normal = false;

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
    if (_is_normal)
      return *this;

    auto result = (*this) * (1.0 / mag());
    result._is_normal = true;
    return result;
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
  Ray() {}

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

// Rectangle shaped segment of a plane
class RectanglePlaneSegment {
  Plane _container;
  std::array<Vector, 3> _pts;
  Vector _orth_0, _orth_1;
  double _orth_0_begin, _orth_0_end;
  double _orth_1_begin, _orth_1_end;

public:
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
  const Vector &normal() const { return container().normal(); }
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
  Sphere(const Vector &center, double radius)
      : _center(center), _radius(radius), _center_normal(_center.normalize()) {
    _rhs = center * center - radius * radius;
  }

  const Vector &center() const { return _center; }
  double radius() const { return _radius; }

  bool intersect(const Ray &r, double &out) const {
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

class Cube {
  typedef std::array<RectanglePlaneSegment, 6> FacesTy;

  FacesTy _faces;

  static FacesTy compute_faces(const Vector &center, const Vector &normal_a,
                               const Vector &normal_b, double side) {
    Vector n_a = normal_a.normalize();
    Vector n_b = normal_b.normalize();
    Vector n_c = n_a.cross_product(n_b);
    Vector ns[3] = {n_a, n_b, n_c};

    auto compute_face = [&](unsigned face_idx, int sign) {
      const Vector &axis_0 = ns[(face_idx + 1) % 3] * side;
      const Vector &axis_1 = ns[(face_idx + 2) % 3] * side;

      assert(Ruler::is_zero(ns[face_idx] * axis_0) && "Expected orthogonal!");
      assert(Ruler::is_zero(ns[face_idx] * axis_1) && "Expected orthogonal!");

      Vector pt_in_plane = center + sign * ns[face_idx] * side;

      std::array<Vector, 3> pts;
      pts[0] = pt_in_plane + axis_0 + axis_1;
      pts[1] = pt_in_plane - axis_0 + axis_1;
      pts[2] = pt_in_plane - axis_0 - axis_1;

      if (sign == -1)
        std::reverse(pts.begin(), pts.end());

      return RectanglePlaneSegment(pts);
    };

    return {{compute_face(0, -1), compute_face(0, 1), compute_face(1, -1),
             compute_face(1, 1), compute_face(2, -1), compute_face(2, 1)}};
  }

public:
  explicit Cube(const Vector &center, const Vector &normal_a,
                const Vector &normal_b, double side)
      : _faces(Cube::compute_faces(center, normal_a, normal_b, side)) {}

  const FacesTy &faces() const { return _faces; }

  static const int kFaceCount = 6;

  bool intersect(const Ray &r, double &out_k, unsigned &face_idx) const {
    double smallest_k = std::numeric_limits<double>::infinity();
    face_idx = -1;

    for (unsigned i = 0; i < kFaceCount; i++) {
      double k;
      if (_faces[i].intersect(r, k) && k < smallest_k) {
        smallest_k = k;
        face_idx = i;
      }
    }

    if (face_idx == -1)
      return false;

    out_k = smallest_k;
    return true;
  }
};
}

#endif
