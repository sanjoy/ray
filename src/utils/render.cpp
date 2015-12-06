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
  printf("usage: ./render scene-name\n");
  printf("scene names:\n");
  for_each_scene_generator([&](const char *sg_name, SceneGeneratorTy) {
      printf("  %s\n", sg_name);
    });
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
  if (auto sg = get_scene_generator_by_name(scene_name)) {
    do_scene(sg);
    return;
  }

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
