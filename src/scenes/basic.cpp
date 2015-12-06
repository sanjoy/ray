#include "bitmap.hpp"
#include "camera.hpp"
#include "objects.hpp"
#include "scene.hpp"
#include "scene-generators.hpp"

using namespace ray;
using namespace std;

Camera ray::generator::generate_basic_scene(Scene &s) {
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
    s.add_object(
        make_unique<BoxObj>(position, init_normal_a, init_normal_b, 200.0));
    init_normal_a = init_normal_a.rotate(0.3, init_normal_b);
    init_normal_b = init_normal_b.rotate(1.3, init_normal_a);
  }

  s.add_object(make_unique<SkyObj>());

  return Camera(6.0, 2000, 2000, 150, ray::Vector());
}
