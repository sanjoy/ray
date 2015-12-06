#include "bitmap.hpp"
#include "camera.hpp"
#include "objects.hpp"
#include "scene.hpp"
#include "scene-generators.hpp"

using namespace ray;
using namespace std;

static Camera generate_basic_scene(Scene &s) {
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

static Camera generate_sphere_scene(Scene &s) {
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

  auto sphere_pos_a =
      Vector::get_i() * 4500 + Vector::get_j() * 2000 + Vector::get_k() * 2000;

  auto sphere_pos_b =
      Vector::get_i() * 4500 - Vector::get_j() * 2000 - Vector::get_k() * 2000;

  auto sphere_pos_c = Vector::get_i() * 3500;

  s.add_object(make_unique<SphericalMirrorObj>(sphere_pos_a, 600));
  s.add_object(make_unique<SphericalMirrorObj>(sphere_pos_b, 600));
  s.add_object(make_unique<SphericalMirrorObj>(sphere_pos_c, 600));

  return Camera(6.0, 5000, 2500, 200, ray::Vector());
}

static Camera generate_refraction_scene(Scene &s) {
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

void ray::for_each_scene_generator(SceneGenCallbackTy callback) {
  callback("basic", generate_basic_scene);
  callback("sphere", generate_sphere_scene);
  callback("refraction", generate_refraction_scene);
}

SceneGeneratorTy ray::get_scene_generator_by_name(const char *name) {
  SceneGeneratorTy result;

  for_each_scene_generator([&](const char *sg_name, SceneGeneratorTy sg) {
      if (!strcmp(sg_name, name))
        result = sg;
    });

  return result;
}
