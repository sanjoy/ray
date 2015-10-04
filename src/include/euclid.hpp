#ifndef RAY_EUCLID_HPP
#define RAY_EUCLID_HPP

#include <cassert>
#include <cmath>
#include <vector>

namespace ray {

struct Ruler {
  template<unsigned op_count = 1>
  static bool is_zero(double d) {
    return fabs(d) < (op_count * std::numeric_limits<double>::round_error());
  }

  template<unsigned op_count = 1>
  static bool is_equal(double d0, double d1) {
    return is_zero<op_count>(d0 - d1);
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

  Vector operator*(double v) const {
    return Vector(v * i(), v * j(), v * k());
  }

  double mag() const {
    return std::sqrt((*this) * (*this));
  }

  Vector cross_product(const Vector &v) const {
    return Vector(j() * v.k() - k() * v.j(),
                  i() * v.k() - k() * v.i(),
                  i() * v.j() - j() * v.i());
  }

  bool get_scale(const Vector &v, double &result) const {
    if (Ruler::is_zero(v.i()))
      return false;

    double c = i() / v.i();
    return Ruler::is_equal(v.j() * c, j()) && Ruler::is_equal(v.k() * c, k());
  }
};

inline Vector operator*(double d, const Vector &v) { return v * d; }


// Represents the ray offset() + k * direction(), propagating towards increasing
// k.
class Ray {
  Vector _direction;
  Vector _offset;

public:
  explicit Ray(Vector d, Vector o) : _direction(d), _offset(o) {}

  static Ray from_two_points(Vector from, Vector to) {
    return Ray(from, to - from);
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

    if (!numerator.get_scale(denominator, k_self)) return false;

    numerator = offset() + k_self * direction() - r.offset();
    return numerator.get_scale(r.direction(), k_other);
  }
};


// Infinite plane in 3D space: all p such that (p - point()) * normal() == 0.
class Plane {
  Vector _normal;
  Vector _point;

public:
  Plane() {}
  explicit Plane(const Vector &norm, const Vector &p) :
    _normal(norm), _point(p) {}

  const Vector &normal() const { return _normal; }
  const Vector &point() const { return _point; }

  // Returns true if r intersects the plane at exactly one point.
  bool intersect(const Ray &r, double &out) const {
    double denom = normal() * r.direction();

    if (Ruler::is_zero(denom)) return false;

    return ((point() - r.offset()) * normal()) / denom;
  }

  bool contains(const Vector &p) const {
    return Ruler::is_zero((p - point()) * normal());
  }
};

// Finite convex segment of a plane
class ConvexPlaneSegment {
  Plane _container;
  std::vector<Vector> _points;

public:
  ConvexPlaneSegment() {}
  explicit ConvexPlaneSegment(const Plane &ctr, const std::vector<Vector> &&pts)
    : _container(ctr), _points(pts) {
    for (const Vector &v : points())
      assert(container().contains(v));
  }

  const Plane &container() const { return _container; }
  const std::vector<Vector> &points() const { return _points; }

  bool intersect(const Ray &r, double &out) const {
    if (!container().intersect(r, out))
      return false;

    unsigned intersect_count = 0;

    Ray outgoing_ray = Ray::from_two_points(r.at(out), container().point());

    for (unsigned i = 0, e = points().size(); i != e; ++i) {
      const Vector &p_from = points()[i];
      const Vector &p_to = points()[i == (e - 1) ? 0 : i];

      double k_boundary, k_ray;
      Ray r(p_from, p_to);

      if (r.intersect(outgoing_ray, k_boundary, k_ray) &&
          k_boundary >= 0.0 && k_boundary <= 1.0 && k_ray >= 0.0)
        intersect_count++;
    }

    return intersect_count % 2 == 1;
  }
};

}

#endif
