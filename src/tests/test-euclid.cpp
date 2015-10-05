#include "euclid.hpp"

#include <fstream>
#include <iostream>

using namespace ray;

static void test_cross_product() {
  Vector v;
  Vector vi = Vector::get_i();
  Vector vj = Vector::get_j();;
  Vector vk = Vector::get_k();;

  assert(v.cross_product(v).is_zero() && "Should be!");
  assert(vi.cross_product(vi).is_zero() && "Should be!");
  assert(vj.cross_product(vj).is_zero() && "Should be!");
  assert(vk.cross_product(vk).is_zero() && "Should be!");

  assert(vi.cross_product(vj) == vk);
  assert(vj.cross_product(vk) == vi);
  assert(vk.cross_product(vi) == vj);
}

static void test_intersection() {
  double x, y;
  {
    Ray r0 = Ray::from_two_points(Vector(), Vector(1.0, 1.0, 0.0));
    Ray r1 = Ray::from_two_points(Vector(0.0, 1.0, 0.0), Vector(1.0, 0.0, 0.0));
    assert(r0.intersect(r1, x, y) && "Supposed to intersect!");
  }

  {
    Ray r0 = Ray::from_two_points(Vector(), Vector(1.0, 1.0, 0.0));
    Ray r1 = Ray::from_two_points(Vector(), Vector(1.0, 1.0, 0.0));
    assert(r0.intersect(r1, x, y) && "Supposed to intersect!");
  }

  {
    Ray r0 = Ray::from_two_points(Vector(),        Vector(1.0, 1.0, 0.0));
    Ray r1 = Ray::from_two_points(Vector::get_i(), Vector(1.0, 1.0, 1.0));
    assert(!r0.intersect(r1, x, y) && "Not supposed to intersect!");
  }
}

int main(int argc, char **argv) {
  test_cross_product();
  test_intersection();
  return 0;
}
