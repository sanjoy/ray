#include "camera.hpp"

#include "context.hpp"

using namespace ray;

Camera::Camera(const Scene &s, double focal_length, unsigned screen_width_px,
               unsigned screen_height_px, unsigned screen_resolution,
               const Vector &pos)
    : _scene(s), _focal_length(focal_length), _screen_width_px(screen_width_px),
      _screen_height_px(screen_height_px),
      _screen_resolution(screen_resolution), _focus_position(pos) {}

Bitmap Camera::snap() {
  Bitmap bmp(_screen_height_px, _screen_width_px, Color::create_blue());
  Context ctx(_scene.object_count());
  _scene.init_object_ids(ctx);

  double max_diag_square =
    std::pow(_screen_height_px / 2, 2) + std::pow(_screen_width_px / 2, 2);

  auto scale = [max_diag_square](int x, int y) {
    return 1.0 + (x * x + y * y) / max_diag_square;
  };

  for (int xi = -(_screen_width_px / 2), xe = (_screen_width_px / 2); xi != xe;
       ++xi)
    for (int yi = -(_screen_height_px / 2), ye = (_screen_height_px / 2);
         yi != ye; ++yi) {
      double s = scale(xi, yi);
      const Vector sample_pt(_focal_length, (xi * s) / _screen_resolution,
                             (yi * s) / _screen_resolution);
      Ray r =
          Ray::from_two_points(_focus_position, _focus_position + sample_pt);
      Color c = _scene.render_pixel(r, ctx);
      bmp.at(xi + _screen_width_px / 2, yi + _screen_height_px / 2) = c;
    }

  return bmp;
}
