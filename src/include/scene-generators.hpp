#ifndef RAY_SCENE_GENERATORS_HPP
#define RAY_SCENE_GENERATORS_HPP

#include "camera.hpp"
#include "scene.hpp"

#include <functional>

namespace ray {

typedef std::function<Camera(Scene &)> SceneGeneratorTy;
SceneGeneratorTy get_scene_generator_by_name(const char *);

typedef std::function<void(const char *, SceneGeneratorTy)> SceneGenCallbackTy;
void for_each_scene_generator(SceneGenCallbackTy);

}

#endif
