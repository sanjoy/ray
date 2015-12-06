#include "objects.hpp"

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

      if (sign == 1)
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

BoxObj::BoxObj(const Vector &center, const Vector &normal_a,
               const Vector &normal_b, double side) {

  create_cube_faces(center, normal_a, normal_b, side, _faces);

  _colors[0] = Color(61, 31, 0);
  _colors[1] = Color(102, 0, 60);
  _colors[2] = Color(0, 102, 153);

  _colors[3] = Color(0, 0, 153);
  _colors[4] = Color(51, 153, 50);
  _colors[5] = Color(71, 0, 71);
}

bool BoxObj::incident(Context &, const Scene &, const Ray &incoming,
                      double current_best_k, double &out_k,
                      Color &out_c) const {
  unsigned idx;
  if (intersect_faces<FACE_COUNT>(_faces, incoming, out_k, idx)) {
    out_c = _colors[idx];
    return true;
  }

  return false;
}

bool SkyObj::incident(Context &, const Scene &, const Ray &incoming,
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

bool SphericalMirrorObj::incident(Context &ctx, const Scene &scene,
                                  const Ray &incoming, double current_best_k,
                                  double &out_k, Color &out_c) const {
  intptr_t &nesting = ctx.get(object_id());
  if (nesting >= SphericalMirrorObj::max_nesting())
    return false;

  if (_sphere.intersect(incoming, out_k) && out_k >= 0.0) {
    auto touch_pt = incoming.at(out_k);
    auto reflector_normal = (touch_pt - _sphere.center()).normalize();
    auto incoming_dir_inverse = (-incoming.direction()).normalize();
    auto reflector_normal_scaled =
        (incoming_dir_inverse * reflector_normal) * reflector_normal;
    auto new_dir = 2 * reflector_normal_scaled - incoming_dir_inverse;
    auto new_ray = Ray::from_offset_and_direction(
        touch_pt + reflector_normal * Ruler::epsilon(), new_dir);

    nesting++;
    out_c = scene.render_pixel(new_ray, ctx) * 0.8;
    nesting--;
    return true;
  }

  return false;
}

RefractiveBoxObj::RefractiveBoxObj(const Vector &center, const Vector &normal_a,
                                   const Vector &normal_b, double side) {
  create_cube_faces(center, normal_a, normal_b, side, _faces);
}

bool RefractiveBoxObj::incident(Context &ctx, const Scene &scene,
                                const Ray &incoming, double current_best_k,
                                double &out_k, Color &out_c) const {
  intptr_t &nesting = ctx.get(object_id());
  if (nesting >= RefractiveBoxObj::max_nesting())
    return false;

  auto refract_ray = [&](const Ray &r, const double refract_ratio,
                         bool invert_face_normals, Ray &out_r,
                         double &out_incident_k) {
    unsigned incident_idx;
    if (!intersect_faces<FACE_COUNT>(faces(), r, out_incident_k, incident_idx))
      return false;

    Vector normal = faces()[incident_idx].normal();
    if (invert_face_normals)
      normal = normal * (-1.0);

    double cos_of_incoming = r.direction().normalize() * normal;

    double sin_sqr_theta_t =
      std::pow(refract_ratio, 2) * (1 - std::pow(cos_of_incoming, 2));
    double disc = 1 - sin_sqr_theta_t;
    if (disc >= 0.0) {
      // Refraction
      double normal_factor = refract_ratio * cos_of_incoming - std::sqrt(disc);
      Vector new_dir = refract_ratio * r.direction() + normal * normal_factor;

      out_r =
          Ray::from_offset_and_direction(incoming.at(out_incident_k), new_dir);
      return true;
    }

    // Total internal reflection
    auto r_dir_inverse = (-r.direction()).normalize();
    auto reflector_normal_scaled = (r_dir_inverse * normal) * normal;
    auto new_dir = 2 * reflector_normal_scaled - r_dir_inverse;
    out_r = Ray::from_offset_and_direction(
        incoming.at(out_incident_k) + normal * Ruler::epsilon(), new_dir);
    return true;
  };

  constexpr double ratio0 = 1.0 / _relative_refractive_index;
  constexpr double ratio1 = _relative_refractive_index;
  Ray r0, r1;
  double second_incident_k;
  if (!refract_ray(incoming, ratio0, false, r0, out_k) ||
      !refract_ray(r0, ratio1, true, r1, second_incident_k))
    return false;

  nesting++;
  out_c = scene.render_pixel(r1, ctx) * 0.9;
  nesting--;
  return true;
}
