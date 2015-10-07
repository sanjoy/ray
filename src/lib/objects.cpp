#include "objects.hpp"

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

      std::vector<Vector> pts;
      pts.push_back(pt_in_plane + axis_0 + axis_1);
      pts.push_back(pt_in_plane + axis_0 - axis_1);
      pts.push_back(pt_in_plane - axis_0 - axis_1);
      pts.push_back(pt_in_plane - axis_0 + axis_1);

      ConvexPlaneSegment cps(p, std::move(pts));
      if (sign == -1)
        _faces[2 * i] = std::move(cps);
      else
        _faces[2 * i + 1] = std::move(cps);
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
  for (ConvexPlaneSegment &cps : _faces) {
    // if (idx != 3 && idx != 2) { idx++; continue; }
    double k;
    if (cps.intersect(incoming, k) && k < smallest_k) {
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

  double grad = incoming.direction().horizontal_gradient();
  double angle_ratio = std::fabs(std::atan(grad) / (M_PI / 2));
  out_k = std::numeric_limits<double>::max();
  out_c = Color(uint8_t(255 * angle_ratio), uint8_t(255 * angle_ratio), 255);
  return true;
}
