/// scene-generators.hpp: predefined scene generators.
///

#ifndef RAY_SCENE_GENERATORS_HPP
#define RAY_SCENE_GENERATORS_HPP

#include "scene.hpp"

#include <functional>

namespace ray {

typedef std::function<Camera(Scene &)> SceneGeneratorTy;

/// Returns the scene generator named \p name, and null if no such
/// scene generator exists.
SceneGeneratorTy get_scene_generator_by_name(const char *name);

typedef std::function<void(const char *, SceneGeneratorTy)> SceneGenCallbackTy;

/// Calls \p callback on each scene generator name & scene generator
/// pair.
void for_each_scene_generator(SceneGenCallbackTy callback);
}

#endif
