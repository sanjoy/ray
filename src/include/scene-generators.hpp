#ifndef RAY_SCENE_GENERATORS_HPP
#define RAY_SCENE_GENERATORS_HPP

#define SCENE_GENERATOR_DO(F)                                                  \
  F("basic", basic)                                                            \
  F("sphere", sphere)                                                          \
  F("refract", refract)

namespace ray {
namespace generator {

#define DECL_FN(str_name, name) Camera generate_##name##_scene(Scene &);

SCENE_GENERATOR_DO(DECL_FN)

#undef DECL_FN
}
}

#endif
