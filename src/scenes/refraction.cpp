#include "bitmap.hpp"
#include "camera.hpp"
#include "objects.hpp"
#include "scene.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>

using namespace ray;
using namespace std;

int main(int argc, char **argv) {
  {
    Scene s;

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
      if (i % 3 == 0) {
        unique_ptr<RefractiveBoxObj> box(
          new RefractiveBoxObj(position, init_normal_a, init_normal_b, 200.0));
        s.add_object(std::move(box));
      } else {
        unique_ptr<BoxObj> box(
          new BoxObj(position, init_normal_a, init_normal_b, 200.0));
        s.add_object(std::move(box));
      }

      init_normal_a = init_normal_a.rotate(0.3, init_normal_b);
      init_normal_b = init_normal_b.rotate(1.3, init_normal_a);
    }

    init_normal_a = init_normal_a.rotate(0.3, init_normal_b);
    init_normal_b = init_normal_b.rotate(1.3, init_normal_a);

    s.add_object(std::unique_ptr<SkyObj>(new SkyObj));

    auto sphere_pos_a = Vector::get_i() * 4500 + Vector::get_j() * 2000 +
                        Vector::get_k() * 2000;

    auto sphere_pos_b = Vector::get_i() * 4500 - Vector::get_j() * 2000 -
                        Vector::get_k() * 2000;

    auto sphere_pos_c = Vector::get_i() * 3500;

    s.add_object(unique_ptr<SphericalMirrorObj>(
        new SphericalMirrorObj(sphere_pos_a, 600)));

    s.add_object(unique_ptr<SphericalMirrorObj>(
        new SphericalMirrorObj(sphere_pos_b, 600)));

    s.add_object(unique_ptr<SphericalMirrorObj>(
        new SphericalMirrorObj(sphere_pos_c, 600)));

    auto refractive_pos_a = Vector::get_i() * 1500 - Vector::get_k() * 500 +
      Vector::get_j() * 500;

    unique_ptr<RefractiveBoxObj> box(
      new RefractiveBoxObj(refractive_pos_a, init_normal_a, init_normal_b,
                           200.0));

    s.add_object(std::move(box));

    Camera c(s, 6.0, 5000, 2500, 200, ray::Vector());
    Bitmap bmp = c.snap();
    ofstream out("/tmp/out.bmp", std::ofstream::binary);
    bmp.write(out);
  }

  cout << "finished, opening image\n";
  system("open /tmp/out.bmp");

  return 0;
}
