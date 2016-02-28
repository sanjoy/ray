#include "euclid.hpp"
#include "support.hpp"
#include "test.hpp"

#include <cmath>
#include <fstream>
#include <iostream>

using namespace ray;

static const unsigned kCreateVectMax = 8;
static Vector create_vect(unsigned seed) {
  switch (seed % kCreateVectMax) {
  case 0:
    return Vector(1.3, 3.3, 9.5);
  case 1:
    return Vector(100.3, 100.3, 100.5);
  case 2:
    return Vector(-16, 333.7, -9.5);
  case 3:
    return Vector(-1.3, 0.0, -9.5);
  case 4:
    return Vector::get_i();
  case 5:
    return Vector::get_j();
  case 6:
    return Vector::get_k();
  case 7:
    return Vector::get_i() + Vector::get_j() + Vector::get_k();
  default:
    unreachable("All cases covered!");
  }
}

static void test_vector_properties() {
  RAY_INIT_TEST("Vector properties");

  Vector vi = Vector::get_i();
  Vector vj = Vector::get_j();
  Vector vk = Vector::get_k();

  CHECK0(vi.cross_product(vi).is_zero());
  CHECK0(vj.cross_product(vj).is_zero());
  CHECK0(vk.cross_product(vk).is_zero());

  CHECK0(vi.cross_product(vj) == vk);
  CHECK0(vj.cross_product(vk) == vi);
  CHECK0(vk.cross_product(vi) == vj);

  for (unsigned i = 0; i < kCreateVectMax; i++)
    CHECK0(create_vect(i).cross_product(create_vect(i)).is_zero());

  for (unsigned i = 0; i < kCreateVectMax; i++) {
    for (unsigned j = 0; j < kCreateVectMax; j++) {
      Vector v = create_vect(i);
      Vector r = create_vect(j);
      if (Ruler::is_zero(v * r))
        CHECK0(Ruler::is_zero(v.dot_product(v.rotate(M_PI / 2, r))));
    }
  }

  for (unsigned i = 0; i < kCreateVectMax; i++) {
    for (unsigned j = 0; j < kCreateVectMax; j++) {
      Vector v = create_vect(i);
      Vector r = create_vect(j);
      if (Ruler::is_zero(v * r))
        CHECK0(v.cross_product(v.rotate(M_PI, r)).is_zero()) << r << "\n";
    }
  }

  for (unsigned i = 0; i < kCreateVectMax; i++) {
    for (unsigned j = 0; j < kCreateVectMax; j++) {
      Vector v = create_vect(i);
      Vector r = create_vect(j);
      if (Ruler::is_zero(v * r))
        CHECK0(v.cross_product(v.rotate(M_PI, r)).is_zero());
    }
  }

  for (unsigned i = 0; i < kCreateVectMax; i++)
    for (unsigned j = 0; j < kCreateVectMax; j++) {
      Vector v = create_vect(i);
      Vector r = create_vect(j);
      CHECK0(Ruler::is_equal((v * r), (r * v)));
    }
}

static void test_ray_intersection() {
  RAY_INIT_TEST("Ray intersections");

  Vector origin(0.0, 0.0, 0.0);
  double x, y;

  {
    Ray r0 = Ray::from_two_points(origin, Vector(1.0, 1.0, 0.0));
    Ray r1 = Ray::from_two_points(Vector(0.0, 1.0, 0.0), Vector(1.0, 0.0, 0.0));
    CHECK(r0.intersect(r1, x, y), "Supposed to intersect!");
  }

  {
    Ray r0 = Ray::from_two_points(origin, Vector(1.0, 1.0, 0.0));
    Ray r1 = Ray::from_two_points(origin, Vector(1.0, 1.0, 0.0));
    CHECK(r0.intersect(r1, x, y), "Supposed to intersect!");
  }

  {
    Ray r0 = Ray::from_two_points(origin, Vector(1.0, 1.0, 0.0));
    Ray r1 = Ray::from_two_points(Vector::get_i(), Vector(1.0, 1.0, 1.0));
    CHECK(!r0.intersect(r1, x, y), "Not supposed to intersect!");
  }
}

static Vector ChangeDir(const Vector &v) {
  if (v == Vector::get_j())
    return (v - Vector::get_i()).normalize();
  return (v - Vector::get_j()).normalize();
};

static void test_plane_intersection() {
  RAY_INIT_TEST("Plane intersections");
  // Basic ray-plane intersections

  auto CheckIntersection = [&](const Plane &p, const Vector &from,
                               const Vector &to) {
    Ruler::Real k;
    Ray r = Ray::from_two_points(Vector::get_origin(), to);
    CHECK0(p.intersect(r, k));
    CHECK0(p.contains(r.at(k)));
  };

  CheckIntersection(Plane::get_xy(), Vector::get_origin(), Vector::get_k());
  CheckIntersection(Plane::get_xy(), Vector::get_origin() + Vector::get_i(),
                    Vector::get_k() + Vector::get_j());

  for (unsigned i = 0; i < kCreateVectMax; i++) {
    for (unsigned j = 0; j < kCreateVectMax; j++) {
      Plane p(create_vect(i).normalize(), create_vect(j));
      Vector parallel_dir =
          p.normal().cross_product(ChangeDir(p.normal())).normalize();
      Vector non_parallel_dir = (parallel_dir + p.normal()).normalize();

      assert(Ruler::is_zero(parallel_dir * p.normal()));
      assert(!Ruler::is_zero(non_parallel_dir * p.normal()));

      Vector init_point = create_vect((i + j) % kCreateVectMax);
      Ray parallel_ray =
          Ray::from_offset_and_direction(init_point, parallel_dir);
      Ray non_parallel_ray =
          Ray::from_offset_and_direction(init_point, non_parallel_dir);

      Ruler::Real k;
      CHECK0(p.intersect(non_parallel_ray, k)) << p << " " << non_parallel_ray
                                               << "\n";
      CHECK0(p.contains(non_parallel_ray.at(k)));
      CHECK0(!p.intersect(parallel_ray, k));
    }
  }
}

static void test_rps_intersection() {
  RAY_INIT_TEST("RectanglePlaneSegment intersections");

  for (unsigned i = 0; i < kCreateVectMax; i++) {
    for (unsigned j = 0; j < kCreateVectMax; j++) {
      if (i == j)
        continue;
      //
      //   pt0 <--- pt1
      //             |
      //             |
      //             V
      //            pt2
      Vector pt0 = create_vect(i);
      Vector pt1 = create_vect(j);
      Vector dir = pt0 - pt1;
      Vector dir_perp = dir.cross_product(ChangeDir(dir)).normalize();
      Vector pt2 = pt1 + dir_perp;

      RectanglePlaneSegment rps({{pt0, pt1, pt2}});

      auto real_values = {0.001, 0.1, 0.3, 0.5, 0.7733, 0.8, 0.9999};
      for (Ruler::Real x : real_values)
        for (Ruler::Real y : real_values) {
          Ray r = Ray::from_two_points(Vector::get_origin(),
                                       pt1 + x * dir + y * dir_perp);
          Ruler::Real k;
          CHECK0(rps.intersect(r, k)) << r << " " << rps << "\n";
          CHECK0(rps.container().contains(r.at(k)));
        }
    }
  }
}

int main(int argc, char **argv) {
  test_vector_properties();
  test_ray_intersection();
  test_plane_intersection();
  test_rps_intersection();
  return 0;
}
