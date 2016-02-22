#include "euclid.hpp"
#include "test.hpp"

#include <fstream>
#include <iostream>

using namespace ray;

static void test_cross_product() {
  RAY_INIT_TEST("Cross Products");

  Vector v;
  Vector vi = Vector::get_i();
  Vector vj = Vector::get_j();
  Vector vk = Vector::get_k();

  CHECK0(v.cross_product(v).is_zero());
  CHECK0(vi.cross_product(vi).is_zero());
  CHECK0(vj.cross_product(vj).is_zero());
  CHECK0(vk.cross_product(vk).is_zero());

  CHECK0(vi.cross_product(vj) == vk);
  CHECK0(vj.cross_product(vk) == vi);
  CHECK0(vk.cross_product(vi) == vj);
}

static void test_ray_intersection() {
  RAY_INIT_TEST("Ray intersections");

  double x, y;
  {
    Ray r0 = Ray::from_two_points(Vector(), Vector(1.0, 1.0, 0.0));
    Ray r1 = Ray::from_two_points(Vector(0.0, 1.0, 0.0), Vector(1.0, 0.0, 0.0));
    CHECK(r0.intersect(r1, x, y), "Supposed to intersect!");
  }

  {
    Ray r0 = Ray::from_two_points(Vector(), Vector(1.0, 1.0, 0.0));
    Ray r1 = Ray::from_two_points(Vector(), Vector(1.0, 1.0, 0.0));
    CHECK(r0.intersect(r1, x, y), "Supposed to intersect!");
  }

  {
    Ray r0 = Ray::from_two_points(Vector(), Vector(1.0, 1.0, 0.0));
    Ray r1 = Ray::from_two_points(Vector::get_i(), Vector(1.0, 1.0, 1.0));
    CHECK(!r0.intersect(r1, x, y), "Not supposed to intersect!");
  }
}

int main(int argc, char **argv) {
  test_cross_product();
  test_ray_intersection();
  return 0;
}
