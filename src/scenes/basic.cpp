#include "bitmap.hpp"
#include "camera.hpp"
#include "scene.hpp"

#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
  ray::Scene s;
  ray::Camera c(s, 1.0, 500, 500, 50, ray::Vector());
  ray::Bitmap bmp = c.snap();
  std::ofstream out("/tmp/out.bmp", std::ofstream::binary);
  bmp.write(out);
  std::cout << "hello world!\n";
  return 0;
}
