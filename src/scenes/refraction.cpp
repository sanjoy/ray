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

Camera ray::generator::generate_refract_scene(Scene &s) {
  Vector init_normal_a =
      Vector::get_i() +
      (1.0 / std::sqrt(2)) * (Vector::get_k() + Vector::get_j());

  Vector init_normal_b =
      Vector::get_i() -
      (1.0 / std::sqrt(2)) * (Vector::get_k() + Vector::get_j());

  init_normal_a = init_normal_a.rotate(0.1, init_normal_b);
  init_normal_b = init_normal_b.rotate(0.1, init_normal_a);

  for (int i = 0; i < 8; i++) {
    Vector position = Vector::get_i() * 3500 +
                      Vector::get_j() * 1500 * (i - 4) +
                      Vector::get_k() * 1200 * ((i % 4) - 2);
    if (i % 3 == 0)
      s.add_object(make_unique<RefractiveBoxObj>(position, init_normal_a,
                                                 init_normal_b, 200.0));
    else
      s.add_object(
          make_unique<BoxObj>(position, init_normal_a, init_normal_b, 200.0));

    init_normal_a = init_normal_a.rotate(0.3, init_normal_b);
    init_normal_b = init_normal_b.rotate(1.3, init_normal_a);
  }

  init_normal_a = init_normal_a.rotate(0.3, init_normal_b);
  init_normal_b = init_normal_b.rotate(1.3, init_normal_a);

  s.add_object(make_unique<SkyObj>());

  auto sphere_pos_a =
      Vector::get_i() * 4500 + Vector::get_j() * 2000 + Vector::get_k() * 2000;

  auto sphere_pos_b =
      Vector::get_i() * 4500 - Vector::get_j() * 2000 - Vector::get_k() * 2000;

  auto sphere_pos_c = Vector::get_i() * 3500;

  s.add_object(make_unique<SphericalMirrorObj>(sphere_pos_a, 600));
  s.add_object(make_unique<SphericalMirrorObj>(sphere_pos_b, 600));
  s.add_object(make_unique<SphericalMirrorObj>(sphere_pos_c, 600));

  auto refractive_pos_a =
      Vector::get_i() * 1500 - Vector::get_k() * 1500 + Vector::get_j() * 500;

  s.add_object(make_unique<RefractiveBoxObj>(refractive_pos_a, init_normal_a,
                                             init_normal_b, 200.0));

  return Camera(6.0, 5000, 2500, 200, ray::Vector());
}
