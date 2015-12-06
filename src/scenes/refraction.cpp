#include "bitmap.hpp"
#include "camera.hpp"
#include "objects.hpp"
#include "scene.hpp"
#include "scene-generators.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>

using namespace ray;
using namespace std;

Camera ray::generator::generate_refraction_scene(Scene &s) {
  Plane plane(-Vector::get_i(), Vector::get_i() * 250);
  s.add_object(make_unique<InfinitePlane>(plane, Vector::get_j(), 150));

  auto refractive_pos_a =
      Vector::get_i() * 150 - Vector::get_k() * 15 + Vector::get_j() * 50;

  Vector init_normal_a =
      Vector::get_i() +
      (1.0 / std::sqrt(2)) * (Vector::get_k() + Vector::get_j());

  Vector init_normal_b =
      Vector::get_i() -
      (1.0 / std::sqrt(2)) * (Vector::get_k() + Vector::get_j());

  init_normal_a = init_normal_a.rotate(0.1, init_normal_b);

  s.add_object(make_unique<RefractiveBoxObj>(refractive_pos_a, init_normal_a,
                                             init_normal_b, 50.0));

  return Camera(6.0, 5000, 2500, 20, ray::Vector());
}
