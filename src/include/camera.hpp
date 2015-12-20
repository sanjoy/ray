#ifndef RAY_CAMERA_HPP
#define RAY_CAMERA_HPP

#include "object.hpp"
#include "scene.hpp"

namespace ray {

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
