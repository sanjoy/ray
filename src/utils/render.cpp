#include "scene.hpp"
#include "scene-generators.hpp"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

using namespace ray;
using namespace std;

static void print_usage() {
  printf_cr("usage: ./render [ --threads thread-count ] scene-name");
  printf_cr("  thread-count has to be a positive integer in [1, 1024)");
  printf_cr("scene names:");
  for_each_scene_generator([&](const char *sg_name, SceneGeneratorTy) {
      printf_cr("  %s", sg_name);
    });
}

static void do_scene(std::function<Camera(Scene &s)> scene_gen,
                     unsigned thread_count) {
  Scene s;
  Camera c = scene_gen(s);
  Bitmap bmp = c.snap(s, thread_count);
  ofstream out("/tmp/out.bmp", std::ofstream::binary);
  bmp.write(out);

  printf_cr("Finished rendering, opening image");
  system("open /tmp/out.bmp");
}

static void do_scene(const char *scene_name, unsigned thread_count) {
  if (auto sg = get_scene_generator_by_name(scene_name)) {
    do_scene(sg, thread_count);
    return;
  }

  printf_cr("unknown scene: \"%s\"", scene_name);
  print_usage();
}

struct Arguments {
  std::string scene_name;
  std::string exec_name;
  unsigned thread_count;
};

static bool parse_args(Arguments &args, int argc, char **argv) {
  args.exec_name = argv[0];
  argc--;
  argv++;
  bool found_non_pos_arg = false;

  while (argc != 0) {
    char *current = argv[0];
    argc--;
    argv++;

    if (current[0] == '-' && current[1] == '-') {
      if (!strcmp(current, "--threads")) {
        if (argc == 0)
          return false;

        char *thread_count_str = argv[0];
        argc--;
        argv++;

        char *endptr;
        long val = strtol(thread_count_str, &endptr, 10);
        if (endptr != &thread_count_str[strlen(thread_count_str)] ||
            val < 1 || val >= 1024)
          return false;
        args.thread_count = val;
      } else {
        return false;
      }
    } else {
      if (found_non_pos_arg)
        return false;
      args.scene_name = current;
      found_non_pos_arg = current;
    }
  }

  return true;
}

int main(int argc, char **argv) {
  Arguments args;
  if (!parse_args(args, argc, argv)) {
    print_usage();
    return 1;
  }

  do_scene(args.scene_name.c_str(), args.thread_count);
  return 0;
}