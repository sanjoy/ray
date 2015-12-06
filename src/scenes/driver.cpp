#include "scene.hpp"
#include "scene-generators.hpp"

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>

using namespace ray;
using namespace std;

static void print_usage() {
  printf("usage: driver scene-name\n");
  printf("scene names:\n");
#define PRINT_SCENE_NAME(str_name, name) printf("  %s\n", str_name);
  SCENE_GENERATOR_DO(PRINT_SCENE_NAME);
#undef PRINT_SCENE_NAME
}

static void do_scene(std::function<Camera(Scene &s)> scene_gen) {
  Scene s;
  Camera c = scene_gen(s);
  Bitmap bmp = c.snap(s);
  ofstream out("/tmp/out.bmp", std::ofstream::binary);
  bmp.write(out);

  printf("Finished rendering, opening image\n");
  system("open /tmp/out.bmp");
}

static void do_scene(const char *scene_name) {
  Scene s;

#define GEN_SCENE(str_name, name)                                              \
  if (!strcmp(str_name, scene_name)) {                                         \
    do_scene(ray::generator::generate_##name##_scene);                         \
    return;                                                                    \
  }

  SCENE_GENERATOR_DO(GEN_SCENE);

#undef GEN_SCENE

  printf("unknown scene: \"%s\"\n", scene_name);
  print_usage();
}

int main(int argc, char **argv) {
  if (argc != 2) {
    print_usage();
    return 1;
  }

  do_scene(argv[1]);
  return 0;
}
