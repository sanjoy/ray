#ifndef RAY_CAMERA_HPP
#define RAY_CAMERA_HPP

#include "object.hpp"
#include "scene.hpp"

namespace ray {

class Camera {
  const double _focal_length;

  const unsigned _screen_width_px;
  const unsigned _screen_height_px;

  // How many pixels fit in one unit length?
  const unsigned _screen_resolution;

  const Vector &_focus_position;

public:
  explicit Camera(double focal_length, unsigned screen_width_px,
                  unsigned screen_height_px, unsigned screen_resolution,
                  const Vector &pos);

  Bitmap snap(Scene &s);
};
}

#endif
