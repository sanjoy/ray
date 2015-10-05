#include "bitmap.hpp"
#include "camera.hpp"
#include "objects.hpp"
#include "scene.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace ray;

int main(int argc, char **argv) {
  {
    ray::Scene s;
    s.add_object(new ray::BoxObject());
    ray::Camera c(s, 1.0, 500, 500, 50, ray::Vector());
    ray::Bitmap bmp = c.snap();
    std::ofstream out("/tmp/out.bmp", std::ofstream::binary);
    bmp.write(out);
  }

  std::cout << "finished, opening image\n";
  std::system("open /tmp/out.bmp");

  return 0;
}
