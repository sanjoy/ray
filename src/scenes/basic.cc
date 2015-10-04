#include "bitmap.hpp"
#include "scene.hpp"

#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
  ray::Bitmap bmp(100, 200, ray::Color::create_green());
  std::ofstream out("/tmp/out.bmp", std::ofstream::binary);
  bmp.write(out);
  std::cout << "hello world!\n";
  return 0;
}
