/// euclid.hpp - Geometry utilities for a euclidian space
///

#ifndef RAY_EUCLID_HPP
#define RAY_EUCLID_HPP

#include <array>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <utility>

namespace ray {

/// Utilities related to measuring scalar distances.
///
struct Ruler {
  /// The type we use to represent all scalar and vector lengths.
  ///
  /// Put another way, this is the field over which we construct our (euclidian)
  /// vector space.  Realistically, this exists so that I can switch between
  /// doubles and floats easily, and also to put all of the "magic numbers"
  /// (e.g. epsilon) in one place.
  typedef double Real;

  /// Return true if \p d is zero.
  static bool is_zero(Real d) {
    return fabs(d) < std::numeric_limits<double>::round_error();
  }

  /// Return true if \p d is negative.
  static bool is_negative(Real d) { return d < 0.0; }

  /// Return true if \p d0 and \p d1 are equal.
  static bool is_equal(Real d0, Real d1) { return is_zero(d0 - d1); }

  /// The smallest meaningful distance.
  static Real epsilon() { return 1E-55; }

  /// Positive infinity.
  static Real infinity() { return std::numeric_limits<Real>::infinity(); }

  static Real zero() { return 0.0; }

  static Real one() { return 1.0; }
};

/// A 3D vector class.
///
/// This can be used to represent both directions and points in 3D space.
///
class Vector {
  Ruler::Real _i = 0.0;
  Ruler::Real _j = 0.0;
  Ruler::Real _k = 0.0;
  unsigned _is_normal : 1;
  unsigned _is_valid : 1;

  /// Return true if this vector is known to be normal (i.e. has unit length).
  ///
  /// If this returns false, then the vector may or may not be normal, but if
  /// this returns true, then the vector is definitely normal.
  bool is_known_normal() const { return _is_normal; }

  /// Return true if this is a properly initialized vector (as opposed to a
  /// default constructed vector).
  ///
  /// To simplify code, is it useful to be able to default construct a
  /// "sentinel vector", but it is still nice to have a guarantee that it
  /// doesn't get used in interesting ways.  This flag provides that guarantee.
  bool is_valid() const { return _is_valid; }

public:
  Vector() : _is_normal(0), _is_valid(0) {}
  explicit Vector(Ruler::Real i, Ruler::Real j, Ruler::Real k)
      : _i(i), _j(j), _k(k), _is_normal(0), _is_valid(1) {}

  static Vector get_i() { return Vector(1.0, 0.0, 0.0); }
  static Vector get_j() { return Vector(0.0, 1.0, 0.0); }
  static Vector get_k() { return Vector(0.0, 0.0, 1.0); }

  static Vector get_origin() { return Vector(0.0, 0.0, 0.0); }

  Ruler::Real i() const { return _i; }
  Ruler::Real j() const { return _j; }
  Ruler::Real k() const { return _k; }

  bool is_zero() const {
    assert(is_valid() && "Invalid vector vector vector!");
    return Ruler::is_zero(i()) && Ruler::is_zero(j()) && Ruler::is_zero(k());
  }

  bool operator==(const Vector &o) const {
    assert(is_valid() && o.is_valid() && "Invalid vector!");
    return Ruler::is_equal(i(), o.i()) && Ruler::is_equal(j(), o.j()) &&
           Ruler::is_equal(k(), o.k());
  }

  void print(std::ostream &out) const {
    assert(is_valid() && "Invalid vector!");
    out << "[ " << i() << " " << j() << " " << k() << " ]";
  }

  Vector operator+(const Vector &other) const {
    assert(is_valid() && other.is_valid() && "Invalid vector!");
    return Vector(i() + other.i(), j() + other.j(), k() + other.k());
  }

  Vector operator-(const Vector &other) const {
    assert(is_valid() && other.is_valid() && "Invalid vector!");
    return Vector(i() - other.i(), j() - other.j(), k() - other.k());
  }

  Vector operator-() const {
    assert(is_valid() && "Invalid vector!");
    return Vector(-i(), -j(), -k());
  }

  Ruler::Real dot_product(const Vector &other) const {
    assert(is_valid() && other.is_valid() && "Invalid vector!");
    return i() * other.i() + j() * other.j() + k() * other.k();
  }

  Ruler::Real operator*(const Vector &other) const {
    return this->dot_product(other);
  }

  Vector operator*(Ruler::Real v) const {
    assert(is_valid() && "Invalid vector!");
    return Vector(v * i(), v * j(), v * k());
  }

  Vector cross_product(const Vector &v) const {
    assert(is_valid() && v.is_valid() && "Invalid vector!");
    return Vector(j() * v.k() - k() * v.j(), k() * v.i() - i() * v.k(),
                  i() * v.j() - j() * v.i());
  }

  /// Find \p result such that this vector is equal to \p result * \p v.
  ///
  /// If such a \p result does not exist, return false.
  bool get_scale(const Vector &v, Ruler::Real &result) const {
    assert(is_valid() && v.is_valid() && "Invalid vector!");
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

  /// Return tangent of the angle this vector makes with the horizontal plane.
  Ruler::Real horizontal_gradient() const {
    assert(is_valid() && "Invalid vector!");
    return k() / std::sqrt(i() * i() + j() * j());
  }

  /// Compute the length of this vector.
  Ruler::Real mag() const {
    assert(is_valid() && "Invalid vector!");
    return std::sqrt(i() * i() + j() * j() + k() * k());
  }

  /// Compute the distance between the 3D point represented by this vector and
  /// the 3D point represented by \p other.
  Ruler::Real dist(const Vector &other) const {
    assert(is_valid() && other.is_valid() && "Invalid vector!");
    return (*this - other).mag();
  }

  /// Return this vector scaled to unit length.
  ///
  /// Note: because we remember that a vector is normalized once it is
  /// normalized, calling this repeatedly on the same vector is not expensive.
  Vector normalize() const {
    assert(is_valid() && "Invalid vector!");
    assert(!Ruler::is_zero(mag()) && "Cannot normalize a zero vector!");
    if (is_known_normal())
      return *this;

    auto result = (*this) * (1.0 / mag());
    result._is_normal = true;
    return result;
  }

  /// Rotate this vector by \p radian radians, with \p orth being a normal to
  /// the rotation plane.
  Vector rotate(Ruler::Real radian, const Vector &orth) {
    assert(is_valid() && orth.is_valid() && "Invalid vector!");
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

inline Vector operator*(Ruler::Real d, const Vector &v) { return v * d; }

/// Represents a ray in 3D euclidian space.
///
/// The ray consists of all points p = offset() + k * direction() with k >= 0
///
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
    out << "[O: " << offset() << " D: " << direction() << "]";
  }

  const Vector &direction() const { return _direction; }
  const Vector &offset() const { return _offset; }

  Vector at(Ruler::Real k) const { return offset() + k * direction(); }

  /// Find a the point where \p r intersects this ray, if such an unique point
  /// exists.
  ///
  bool intersect(const Ray &r, Ruler::Real &k_self,
                 Ruler::Real &k_other) const {
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

  bool contains(const Vector &v, Ruler::Real &out_k) const {
    if (direction().is_zero())
      return false;
    return (v - offset()).get_scale(direction(), out_k);
  }
};

inline std::ostream &operator<<(std::ostream &out, const Ray &r) {
  r.print(out);
  return out;
}

/// Represents the infinite plane in 3D space.
///
/// The plane contains all p such that (p - point()) * normal() == 0.
///
/// The sign of the normal vector has semantic meaning in some contexts -- the
/// "outside" is often denoted by as the side the normal points to, and e.g. the
/// code to compute reflection and refraction use this information.
class Plane {
  Vector _normal;
  Vector _point;

public:
  /// Construct a plane from a normal and a point in the plane.
  explicit Plane(const Vector &norm, const Vector &p)
      : _normal(norm), _point(p) {}

  /// Construct a plane from three points.
  ///
  /// This constructor follows the right hand rule for computing the direction
  /// of the normal.
  explicit Plane(const std::array<Vector, 3> &pts) {
    _normal = (pts[1] - pts[0]).cross_product(pts[2] - pts[0]).normalize();
    _point = pts[0];
  }

  static Plane get_xy() { return Plane(Vector::get_k(), Vector::get_origin()); }

  static Plane get_yz() { return Plane(Vector::get_i(), Vector::get_origin()); }

  static Plane get_zx() { return Plane(Vector::get_j(), Vector::get_origin()); }

  const Vector &normal() const { return _normal; }
  const Vector &point() const { return _point; }

  Plane translate(const Vector &disp) {
    return Plane(normal(), point() + disp);
  }

  /// Returns true if \p r intersects the plane at exactly one point, and
  /// returns the offset at which \p r intersects this plane at \p out.
  bool intersect(const Ray &r, Ruler::Real &out) const {
    Ruler::Real denom = normal() * r.direction();

    if (Ruler::is_zero(denom))
      return false;

    out = ((point() - r.offset()) * normal()) / denom;
    return true;
  }

  bool contains(const Vector &p) const {
    return Ruler::is_zero((p - point()) * normal());
  }

  void print(std::ostream &out) const {
    out << "[N: " << normal() << " P: " << point() << "]";
  }
};

inline std::ostream &operator<<(std::ostream &out, const Plane &p) {
  p.print(out);
  return out;
}

/// Rectangle shaped segment of a plane.
///
/// The RectanglePlaneSegment contains all p such all of the following are true:
///
/// - _container contains p
/// - _orth_0_begin <= p * _orth_0 < _orth_0_end
/// - _orth_1_begin <= p * _orth_1 < _orth_1_end
class RectanglePlaneSegment {
  Plane _container;
  Vector _orth_0, _orth_1;
  Ruler::Real _orth_0_begin, _orth_0_end;
  Ruler::Real _orth_1_begin, _orth_1_end;

public:
  /// Construct a RectanglePlaneSegment from three conrners of a rectangle (the
  /// fourth one is unique given the three).
  explicit RectanglePlaneSegment(const std::array<Vector, 3> &pts)
      : _container(pts) {
#ifndef NDEBUG
    for (const Vector &v : pts)
      assert(container().contains(v));
#endif

    _orth_0 = (pts[0] - pts[1]).normalize();
    _orth_1 = (pts[2] - pts[1]).normalize();

    _orth_0_begin = pts[1] * _orth_0;
    _orth_0_end = pts[0] * _orth_0;

    _orth_1_begin = pts[1] * _orth_1;
    _orth_1_end = pts[2] * _orth_1;

    assert(Ruler::is_zero(_orth_0 * _orth_1) && "Expected orthogonal vectors!");
  }

  const Plane &container() const { return _container; }
  const Vector &normal() const { return container().normal(); }

  /// Return true if \p r intersects this RectanglePlaneSegment, returning the
  /// offset of the ray in \p out.
  bool intersect(const Ray &r, Ruler::Real &out) const {
    if (!container().intersect(r, out))
      return false;

    Vector isection = r.at(out);

    Ruler::Real orth_0_component = isection * _orth_0;
    if (orth_0_component > _orth_0_end || orth_0_component < _orth_0_begin)
      return false;

    Ruler::Real orth_1_component = isection * _orth_1;
    return (orth_1_component <= _orth_1_end &&
            orth_1_component >= _orth_1_begin);
  }
};

/// Represents a sphere in 3D space.
///
/// The sphere consists of all points p such that |p - _center| == _radius.
class Sphere {
  Vector _center;
  Ruler::Real _radius;

  /// This is a cached value that can always be recomputed from _center and
  /// _radius.  Caching this helps us make the implementation of intersect
  /// faster.
  Ruler::Real _rhs;

public:
  explicit Sphere(const Vector &center, Ruler::Real radius)
      : _center(center), _radius(radius) {
    _rhs = center * center - radius * radius;
  }

  const Vector &center() const { return _center; }
  Ruler::Real radius() const { return _radius; }

  /// Return true if \p r intersects this sphere, returning the offset in \p out
  /// if so.
  bool intersect(const Ray &r, Ruler::Real &out) const {
    // Try form an equation of the form "k^2 * a + k * b + c = 0".

    Ruler::Real a = r.direction() * r.direction();
    Ruler::Real b =
        2 * (r.direction() * r.offset()) - 2 * (r.direction() * center());
    Ruler::Real c = r.offset() * r.offset() - 2 * r.offset() * center() + _rhs;

    assert(!Ruler::is_zero(a) && "Direction vector has unit length!");

    Ruler::Real disc_sqr = b * b - 4 * a * c;
    if (Ruler::is_negative(disc_sqr))
      return false;

    Ruler::Real disc = std::sqrt(disc_sqr);
    Ruler::Real k1 = (-b + disc) / (2 * a);
    Ruler::Real k2 = (-b - disc) / (2 * a);

    out = k1 < k2 ? k1 : k2;
    return true;
  }
};

/// Represents a cube in 3D space.
///
/// The cube is essentially a 6-tuple of RectanglePlaneSegments.
class Cube {
  typedef std::array<RectanglePlaneSegment, 6> FacesTy;

  FacesTy _faces;

  static FacesTy compute_faces(const Vector &center, const Vector &normal_a,
                               const Vector &normal_b, Ruler::Real side) {
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

      RectanglePlaneSegment rps(pts);
      assert(rps.normal() == sign * ns[face_idx] && "Bad normal!");
      return rps;
    };

    return {{compute_face(0, -1), compute_face(0, 1), compute_face(1, -1),
             compute_face(1, 1), compute_face(2, -1), compute_face(2, 1)}};
  }

public:
  /// Construct a cube such that
  ///
  ///  - \p center is its geometric center
  ///  - \p normal_a and \p normal_b are normals to two orthogonal faces (all
  ///    other normals are functions of these two normals)
  ///  - each side of the cube is \p side units long
  explicit Cube(const Vector &center, const Vector &normal_a,
                const Vector &normal_b, Ruler::Real side)
      : _faces(Cube::compute_faces(center, normal_a, normal_b, side)) {}

  const FacesTy &faces() const { return _faces; }

  static const int kFaceCount = 6;

  /// Return true if \p r intersects this Cube, and return the ray offset in \p
  /// out_k and the face of intersection in \p face_idx.
  bool intersect(const Ray &r, Ruler::Real &out_k, unsigned &face_idx) const {
    Ruler::Real smallest_k = std::numeric_limits<Ruler::Real>::infinity();
    face_idx = -1;

    for (unsigned i = 0; i < kFaceCount; i++) {
      Ruler::Real k;
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
