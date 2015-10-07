#include "bitmap.hpp"
#include "camera.hpp"
#include "objects.hpp"
#include "scene.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>

using namespace ray;

int main(int argc, char **argv) {
  {
    ray::Scene s;

    Vector init_normal_a = Vector::get_i() +
      (1.0 / std::sqrt(2)) * (Vector::get_k() + Vector::get_j());

    Vector init_normal_b = Vector::get_i() -
      (1.0 / std::sqrt(2)) * (Vector::get_k() + Vector::get_j());

    init_normal_a = init_normal_a.rotate(0.1, init_normal_b);
    init_normal_b = init_normal_b.rotate(0.1, init_normal_a);

    for (int i = 0; i < 8; i++) {
      Vector position =
        Vector::get_i() * 3500 + Vector::get_j() * 1500 * (i - 4) +
        Vector::get_k() * 1200 * ((i % 4) - 2);
      std::unique_ptr<BoxObject> box(
        new ray::BoxObject(position, init_normal_a, init_normal_b, 200.0));
      s.add_object(std::move(box));

      init_normal_a = init_normal_a.rotate(0.3, init_normal_b);
      init_normal_b = init_normal_b.rotate(1.3, init_normal_a);
    }

    s.add_object(std::unique_ptr<SkyObject>(new ray::SkyObject(true)));
    s.add_object(std::unique_ptr<SphericalMirror>(new ray::SphericalMirror(
        Vector::get_i() * 4500 + Vector::get_j() * 2000, 600)));

    ray::Camera c(s, 6.0, 2000, 2000, 150, ray::Vector());
    ray::Bitmap bmp = c.snap();
    std::ofstream out("/tmp/out.bmp", std::ofstream::binary);
    bmp.write(out);
  }

  std::cout << "finished, opening image\n";
  std::system("open /tmp/out.bmp");

  return 0;
}
