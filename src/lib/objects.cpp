#include "objects.hpp"

#include "newton.hpp"
#include "scene.hpp"

#include <cmath>
#include <iostream>

using namespace ray;

static void create_cube_faces(const Vector &center, const Vector &normal_a,
                              const Vector &normal_b, double side,
                              std::array<RectanglePlaneSegment, 6> &out_faces) {
  Vector n_a = normal_a.normalize();
  Vector n_b = normal_b.normalize();
  Vector n_c = n_a.cross_product(n_b);

  Vector ns[3] = {n_a, n_b, n_c};

  for (unsigned i = 0; i < 3; i++) {
    const Vector &axis_0 = ns[(i + 1) % 3] * side;
    const Vector &axis_1 = ns[(i + 2) % 3] * side;

    assert(Ruler::is_zero(ns[i] * axis_0) && "Expected orthogonal!");
    assert(Ruler::is_zero(ns[i] * axis_1) && "Expected orthogonal!");

    for (int sign : {-1, 1}) {
      Vector pt_in_plane = center + sign * ns[i] * side;

      std::array<Vector, 3> pts;
      pts[0] = pt_in_plane + axis_0 + axis_1;
      pts[1] = pt_in_plane - axis_0 + axis_1;
      pts[2] = pt_in_plane - axis_0 - axis_1;

      if (sign == -1)
        std::reverse(pts.begin(), pts.end());

      RectanglePlaneSegment rps(pts);
      if (sign == -1)
        out_faces[2 * i] = rps;
      else
        out_faces[2 * i + 1] = rps;
    }
  }
};

template<unsigned N>
static bool intersect_faces(const std::array<RectanglePlaneSegment, N> &faces,
                            const Ray &r, double &out_k, unsigned &out_idx) {
  double smallest_k = std::numeric_limits<double>::infinity();
  unsigned found_idx = -1;

  for (unsigned i = 0; i < N; i++) {
    double k;
    if (faces[i].intersect(r, k) && k < smallest_k) {
      smallest_k = k;
      found_idx = i;
    }
  }

  if (found_idx == -1) return false;

  out_idx = found_idx;
  out_k = smallest_k;
  return true;
}

BoxObj::BoxObj(const Scene &s, const Vector &center, const Vector &normal_a,
               const Vector &normal_b, double side) : Object(s) {

  create_cube_faces(center, normal_a, normal_b, side, _faces);

  _colors[0] = Color(61, 31, 0);
  _colors[1] = Color(102, 0, 60);
  _colors[2] = Color(0, 102, 153);

  _colors[3] = Color(0, 0, 153);
  _colors[4] = Color(51, 153, 50);
  _colors[5] = Color(71, 0, 71);
}

bool BoxObj::incident(Context &, const Ray &incoming, double current_best_k,
		      double &out_k, Color &out_c) const {
  unsigned idx;
  if (intersect_faces<FACE_COUNT>(_faces, incoming, out_k, idx)) {
    out_c = _colors[idx];
    return true;
  }

  return false;
}

bool SkyObj::incident(Context &, const Ray &incoming,
                      double current_best_k, double &out_k,
                      Color &out_c) const {
  if (current_best_k < std::numeric_limits<double>::max())
    return false;

  if (_uniform)
    out_c = Color::create_white();
  else {
    double grad = incoming.direction().horizontal_gradient();
    double angle_ratio = std::fabs(std::atan(grad * 1.8) / (M_PI / 2));
    out_k = std::numeric_limits<double>::max();
    out_c = Color(uint8_t(255 * angle_ratio), uint8_t(255 * angle_ratio), 255);
  }

  return true;
}

bool InfinitePlane::incident(Context &, const Ray &incoming, double current_best_k,
			     double &out_k, Color &out_c) const {
  if (!_plane.intersect(incoming, out_k) || out_k > current_best_k)
    return false;

  Vector pt = incoming.at(out_k);
  int comp_0 = int((pt * _axis_0) / _check_size);
  int comp_1 = int((pt * _axis_1) / _check_size);

  if (((comp_0 % 2) ^ (comp_1 % 2)))
    out_c = Color::create_white();
  else
    out_c = Color::create_black();
  return true;
}

bool SphericalMirrorObj::incident(Context &ctx, const Ray &incoming, double current_best_k,
                                  double &out_k, Color &out_c) const {
  intptr_t &nesting = ctx.get(object_id());
  if (nesting >= SphericalMirrorObj::max_nesting())
    return false;

  if (_sphere.intersect(incoming, out_k) && out_k >= 0.0) {
    auto touch_pt = incoming.at(out_k);
    auto normal = (touch_pt - _sphere.center()).normalize();
    auto new_ray = get_reflected_ray(incoming, touch_pt, normal);

    nesting++;
    out_c = container().render_pixel(new_ray, ctx) * 0.8;
    nesting--;
    return true;
  }

  return false;
}

RefractiveBoxObj::RefractiveBoxObj(const Scene &s, const Vector &center,
                                   const Vector &normal_a,
                                   const Vector &normal_b, double side) : Object(s) {
  create_cube_faces(center, normal_a, normal_b, side, _faces);
}

bool RefractiveBoxObj::incident(Context &ctx, const Ray &incoming, double current_best_k,
                                double &out_k, Color &out_c) const {
  intptr_t &nesting = ctx.get(object_id());
  if (nesting >= RefractiveBoxObj::max_nesting())
    return false;

  unsigned incident_idx;
  if (!intersect_faces<FACE_COUNT>(faces(), incoming, out_k, incident_idx))
    return false;

  auto r_i = incoming;
  double k = out_k;
  const double ratio0 = 1.0 / _relative_refractive_index;
  const double ratio1 = _relative_refractive_index;

  bool is_tir;
  r_i = get_refracted_ray(r_i, k, faces()[incident_idx].normal(),
                          ratio0, is_tir);

  for (int i = 0; i < 30; i++) {
    if (!intersect_faces<FACE_COUNT>(faces(), r_i, k, incident_idx))
      return false;

    const Vector normal = -faces()[incident_idx].normal();
    r_i = get_refracted_ray(r_i, k, normal, ratio1, is_tir);
    if (!is_tir) break;
  }

  if (is_tir) return false;

  nesting++;
  out_c = container().render_pixel(r_i, ctx) * 0.9;
  nesting--;
  return true;
}
