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
    s.add_object(
      std::unique_ptr<BoxObject>(
        new ray::BoxObject(
          Vector::get_i() * 3500,
          Vector::get_i() +
          (1.0 / std::sqrt(2)) * (Vector::get_k() + Vector::get_j()),
          Vector::get_i() -
          (1.0 / std::sqrt(2)) * (Vector::get_k() + Vector::get_j()),
          800.0)));

    s.add_object(
      std::unique_ptr<SkyObject>(new ray::SkyObject()));

    ray::Camera c(s, 2.0, 1000, 1000, 50, ray::Vector());
    ray::Bitmap bmp = c.snap();
    std::ofstream out("/tmp/out.bmp", std::ofstream::binary);
    bmp.write(out);
  }

  std::cout << "finished, opening image\n";
  std::system("open /tmp/out.bmp");

  return 0;
}
