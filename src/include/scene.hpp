#ifndef RAY_SCENE_HPP
#define RAY_SCENE_HPP

/// scene.hpp: representing and capturing a scene

#include "object.hpp"
#include "context.hpp"
#include "support.hpp"

#include <memory>
#include <vector>

namespace ray {

class Scene {
  std::vector<std::unique_ptr<Object>> _objects;

public:
  void add_object(std::unique_ptr<Object> o) {
    _objects.push_back(std::move(o));
  }
  Color render_pixel(const Ray &r, Context &ctx) const;
  void init_object_ids(Context &ctx) const;
  unsigned object_count() const { return _objects.size(); }
};

/// Represents the camera in a scene.
///
/// The camera decides which ray each pixel in the generated 2D image
/// maps to.  It also manages concurrency.
class Camera {
  const double _focal_length;

  const unsigned _screen_width_px;
  const unsigned _screen_height_px;

  /// One unit of length corresponds to this many pixels.
  const unsigned _screen_resolution;

  const Vector _focus_position;

public:
  explicit Camera(double focal_length, unsigned screen_width_px,
                  unsigned screen_height_px, unsigned screen_resolution,
                  const Vector &pos);

  Bitmap snap(Scene &s, unsigned thread_count = 12);
};

}

#endif
