#include "objects.hpp"

#include "scene.hpp"

#include <cmath>
#include <iostream>

using namespace ray;

BoxObject::BoxObject(const Vector &center, const Vector &normal_a,
                     const Vector &normal_b, double side) {
  Vector n_a = normal_a.normalize();
  Vector n_b = normal_b.normalize();
  Vector n_c = n_a.cross_product(n_b);

  Vector ns[3] = { n_a, n_b, n_c };

  for (unsigned i = 0; i < 3; i++) {
    const Vector &axis_0 = ns[(i + 1) % 3] * side;
    const Vector &axis_1 = ns[(i + 2) % 3] * side;

    assert(Ruler::is_zero(ns[i] * axis_0) && "Expected orthogonal!");
    assert(Ruler::is_zero(ns[i] * axis_1) && "Expected orthogonal!");

    for (int sign : {-1, 1}) {
      Vector pt_in_plane = center + sign * ns[i] * side;
      Plane p(ns[i], pt_in_plane);

      std::array<Vector, 4> pts;
      pts[0] = pt_in_plane + axis_0 + axis_1;
      pts[1] = pt_in_plane + axis_0 - axis_1;
      pts[2] = pt_in_plane - axis_0 - axis_1;
      pts[3] = pt_in_plane - axis_0 + axis_1;

      RectanglePlaneSegment rps(p, pts);
      if (sign == -1)
        _faces[2 * i] = rps;
      else
        _faces[2 * i + 1] = rps;
    }
  }

  _colors[0] = Color(61, 31, 0);
  _colors[1] = Color(102, 0, 60);
  _colors[2] = Color(0, 102, 153);

  _colors[3] = Color(0, 0, 153);
  _colors[4] = Color(51, 153, 50);
  _colors[5] = Color(71, 0, 71);
}

bool BoxObject::incident(const Scene &scene, const Ray &incoming,
                         double current_best_k,
                         double &out_k, Color &out_c) {
  double smallest_k = std::numeric_limits<double>::infinity();

  unsigned idx = 0;
  unsigned found_idx = -1;
  for (RectanglePlaneSegment &rps : _faces) {
    double k;
    if (rps.intersect(incoming, k) && k < smallest_k) {
      smallest_k = k;
      found_idx = idx;
    }
    idx++;
  }

  if (found_idx == -1) return false;

  out_c = _colors[found_idx];
  out_k = smallest_k;
  return true;
}

bool SkyObject::incident(const Scene &scene, const Ray &incoming,
                         double current_best_k,
                         double &out_k, Color &out_c) {
  if (current_best_k < std::numeric_limits<double>::max())
    return false;

  if (!_uniform) {
    double grad = incoming.direction().horizontal_gradient();
    double angle_ratio = std::fabs(std::atan(grad) / (M_PI / 2));
    out_k = std::numeric_limits<double>::max();
    out_c = Color(uint8_t(255 * angle_ratio), uint8_t(255 * angle_ratio), 255);
    return true;
  }

  out_c = Color::create_white();
  return true;
}


bool SphericalMirror::incident(const Scene &scene, const Ray &incoming,
                               double current_best_k,
                               double &out_k, Color &out_c) {
  if (_sphere.intersect(incoming, out_k)) {
    Vector touch_pt = incoming.at(out_k);
    Vector reflector_normal = (touch_pt - _sphere.center()).normalize();

    // TODO: low hanging fruit for optimization.

    auto incoming_dir_inverse = (-incoming.direction()).normalize();
    auto reflection_plane_normal =
        reflector_normal.cross_product(incoming_dir_inverse);

    auto new_dir = [&]() {
      if (reflection_plane_normal.is_zero())
        return incoming_dir_inverse;

      double angle = std::acos(incoming_dir_inverse * reflector_normal);
      return reflector_normal.rotate(angle, reflection_plane_normal);
    }();

    auto new_ray = Ray::from_offset_and_direction(
        touch_pt + reflector_normal * Ruler::epsilon(), new_dir);

    Color c = scene.render_pixel(new_ray);
    out_c = c * 0.8;
    return true;
  }

  return false;
}
