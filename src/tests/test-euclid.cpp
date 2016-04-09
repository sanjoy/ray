#include "euclid.hpp"
#include "support.hpp"
#include "test.hpp"

#include "gtest/gtest.h"

#include <cmath>
#include <fstream>
#include <iostream>

using namespace ray;

namespace {
class EuclidFixture : public ::testing::Test {
protected:
  std::vector<Vector> _vectors;
  std::vector<Plane> _planes;

  EuclidFixture() {
    _vectors.emplace_back(1.3, 3.3, 9.5);
    _vectors.emplace_back(100.3, 100.3, 100.5);
    _vectors.emplace_back(-16, 333.7, -9.5);
    _vectors.emplace_back(-1.3, 0.0, -9.5);
    _vectors.push_back(Vector::get_i());
    _vectors.push_back(Vector::get_j());
    _vectors.push_back(Vector::get_k());
    _vectors.push_back(Vector::get_i() + Vector::get_j() + Vector::get_k());

    for (auto &x : _vectors)
      for (auto &y : _vectors)
        _planes.emplace_back(x.normalize(), y);
  }

  static Vector change_dir(const Vector &v) {
    if (v == Vector::get_j())
      return (v - Vector::get_i()).normalize();
    return (v - Vector::get_j()).normalize();
  };
};
}

TEST_F(EuclidFixture, basic_vector_properties) {
  Vector vi = Vector::get_i();
  Vector vj = Vector::get_j();
  Vector vk = Vector::get_k();

  EXPECT_TRUE(vi.cross_product(vi).is_zero());
  EXPECT_TRUE(vj.cross_product(vj).is_zero());
  EXPECT_TRUE(vk.cross_product(vk).is_zero());

  EXPECT_TRUE(vi.cross_product(vj) == vk);
  EXPECT_TRUE(vj.cross_product(vk) == vi);
  EXPECT_TRUE(vk.cross_product(vi) == vj);

  for (auto &v : _vectors)
    EXPECT_TRUE(v.cross_product(v).is_zero());

  for (auto &v : _vectors)
    for (auto &r : _vectors)
      if (Ruler::is_zero(v * r))
        EXPECT_TRUE(Ruler::is_zero(v.dot_product(v.rotate(M_PI / 2, r))));

  for (auto &v : _vectors)
    for (auto &r : _vectors)
      if (Ruler::is_zero(v * r))
        EXPECT_TRUE(v.cross_product(v.rotate(M_PI, r)).is_zero()) << r << "\n";

  for (auto &v : _vectors)
    for (auto &r : _vectors)
      if (Ruler::is_zero(v * r))
        EXPECT_TRUE(v.cross_product(v.rotate(M_PI, r)).is_zero());

  for (auto &v : _vectors)
    for (auto &r : _vectors)
      EXPECT_TRUE(Ruler::is_equal((v * r), (r * v)));
}

TEST_F(EuclidFixture, ray_intersection) {
  Vector origin(0.0, 0.0, 0.0);
  double x, y;

  {
    Ray r0 = Ray::from_two_points(origin, Vector(1.0, 1.0, 0.0));
    Ray r1 = Ray::from_two_points(Vector(0.0, 1.0, 0.0), Vector(1.0, 0.0, 0.0));
    EXPECT_TRUE(r0.intersect(r1, x, y));
  }

  {
    Ray r0 = Ray::from_two_points(origin, Vector(1.0, 1.0, 0.0));
    Ray r1 = Ray::from_two_points(origin, Vector(1.0, 1.0, 0.0));
    EXPECT_TRUE(r0.intersect(r1, x, y));
  }

  {
    Ray r0 = Ray::from_two_points(origin, Vector(1.0, 1.0, 0.0));
    Ray r1 = Ray::from_two_points(Vector::get_i(), Vector(1.0, 1.0, 1.0));
    EXPECT_FALSE(r0.intersect(r1, x, y));
  }
}

TEST_F(EuclidFixture, plane_intersection) {
  // Basic ray-plane intersections

  auto CheckIntersection = [&](const Plane &p, const Vector &from,
                               const Vector &to) {
    Ruler::Real k;
    Ray r = Ray::from_two_points(Vector::get_origin(), to);
    EXPECT_TRUE(p.intersect(r, k));
    EXPECT_TRUE(p.contains(r.at(k)));
  };

  CheckIntersection(Plane::get_xy(), Vector::get_origin(), Vector::get_k());
  CheckIntersection(Plane::get_xy(), Vector::get_origin() + Vector::get_i(),
                    Vector::get_k() + Vector::get_j());

  unsigned vect_index = 0;

  for (auto &p : _planes) {
    Vector parallel_dir =
        p.normal().cross_product(change_dir(p.normal())).normalize();
    Vector non_parallel_dir = (parallel_dir + p.normal()).normalize();

    EXPECT_TRUE(Ruler::is_zero(parallel_dir * p.normal()));
    EXPECT_FALSE(Ruler::is_zero(non_parallel_dir * p.normal()));

    Vector init_point = _vectors[vect_index];
    vect_index = (vect_index + 1) % _vectors.size();

    Ray parallel_ray = Ray::from_offset_and_direction(init_point, parallel_dir);
    Ray non_parallel_ray =
        Ray::from_offset_and_direction(init_point, non_parallel_dir);

    Ruler::Real k;
    EXPECT_TRUE(p.intersect(non_parallel_ray, k)) << p << " "
                                                  << non_parallel_ray << "\n";
    EXPECT_TRUE(p.contains(non_parallel_ray.at(k)));
    EXPECT_TRUE(!p.intersect(parallel_ray, k));
  }
}

TEST_F(EuclidFixture, rectangle_plane_segment_intersection) {
  for (auto &pt0 : _vectors) {
    for (auto &pt1 : _vectors) {
      if (&pt0 == &pt1)
        continue;

      // The scheme:
      //
      //   pt0 <--- pt1
      //             |
      //             |
      //             V
      //            pt2

      Vector dir = pt0 - pt1;
      Vector dir_perp = dir.cross_product(change_dir(dir)).normalize();
      Vector pt2 = pt1 + dir_perp;

      RectanglePlaneSegment rps({{pt0, pt1, pt2}});

      auto real_values = {0.001, 0.1, 0.3, 0.5, 0.7733, 0.8, 0.9999};
      for (Ruler::Real x : real_values)
        for (Ruler::Real y : real_values) {
          Ray r = Ray::from_two_points(Vector::get_origin(),
                                       pt1 + x * dir + y * dir_perp);
          Ruler::Real k;
          EXPECT_TRUE(rps.intersect(r, k)) << r << " " << rps << "\n";
          EXPECT_TRUE(rps.container().contains(r.at(k)));
        }
    }
  }
}
