#ifndef RAY_CAMERA_HPP
#define RAY_CAMERA_HPP

#include "object.hpp"
#include "scene.hpp"

namespace ray {

class Camera {
  const Scene &_scene;

  const double _focal_length;

  const unsigned _screen_width_px;
  const unsigned _screen_height_px;

  // How many pixels fit in one unit length?
  const unsigned _screen_resolution;

  const Vector &_focus_position;

public:
  explicit Camera(const Scene &s, double focal_length,
                  unsigned screen_width_px, unsigned screen_height_px,
                  unsigned screen_resolution, const Vector &pos);

  Bitmap snap();
};

}

#endif
