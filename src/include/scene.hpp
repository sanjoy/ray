/// scene.hpp: Tools for representing and capturing a scene.
///

#ifndef RAY_SCENE_HPP
#define RAY_SCENE_HPP

#include "object.hpp"
#include "thread-context.hpp"
#include "support.hpp"

#include <memory>
#include <vector>

namespace ray {

/// A scene is a collection of \code Object s with some related raytracing
/// logic.
///
/// \see Object
class Scene {
  std::vector<std::unique_ptr<Object>> _objects;

public:
  /// Add object \p o to the objects contained in this scene.
  void add_object(std::unique_ptr<Object> o) {
    _objects.push_back(std::move(o));
  }

  /// Render a single pixel, with the thread context passed in as \p ctx.
  ///
  /// Return the color of the rendered pixel.
  Color render_pixel(const Ray &r, ThreadContext &ctx, Logger &l) const;

  /// Initialize the object_id fields of the contained objects, related state in
  /// \p ctx.
  void init_object_ids(ThreadContext &ctx) const;

  /// Return the number of objects contained in this scene.
  unsigned object_count() const { return _objects.size(); }
};

/// Represents the camera in a scene.
///
/// The camera decides which ray each pixel in the generated 2D image
/// maps to.  It also manages concurrency.
class Camera {
  const Ruler::Real _focal_length;

  const unsigned _screen_width_px;
  const unsigned _screen_height_px;

  /// One unit of length corresponds to this many pixels.
  const unsigned _screen_resolution;

  const Vector _focus_position;

public:
  explicit Camera(Ruler::Real focal_length, unsigned screen_width_px,
                  unsigned screen_height_px, unsigned screen_resolution,
                  const Vector &pos);

  Bitmap snap(Scene &s, unsigned thread_count = 12);
};
}

#endif
