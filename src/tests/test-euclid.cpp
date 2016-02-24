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
        CHECK0(v.cross_product(v.rotate(M_PI, r)).is_zero());
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
}

int main(int argc, char **argv) {
  test_vector_properties();
  test_ray_intersection();
  test_plane_intersection();
  return 0;
}
