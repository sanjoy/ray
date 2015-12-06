#ifndef RAY_NEWTON_HPP
#define RAY_NEWTON_HPP

#include "euclid.hpp"

namespace ray {

inline Ray get_reflected_ray(const Ray &r, const Vector &pt,
                             const Vector &normal) {
  auto incoming_dir_inverse = (-r.direction()).normalize();
  auto normal_scaled = (incoming_dir_inverse * normal) * normal;
  auto new_dir = 2 * normal_scaled - incoming_dir_inverse;
  return
    Ray::from_offset_and_direction(pt + normal * Ruler::epsilon(), new_dir);
}

inline Ray get_refracted_ray(const Ray &r, const Vector &pt,
                             const Vector &normal, double refractive_index,
                             bool &out_total_internal_reflection) {
  double cos_of_incoming = r.direction().normalize() * normal;

  double sin_sqr_theta_t =
    std::pow(refractive_index, 2) * (1 - std::pow(cos_of_incoming, 2));
  double disc = 1 - sin_sqr_theta_t;
  if (disc >= 0.0) {
    // Refraction
    out_total_internal_reflection = false;
    double normal_factor = refractive_index * cos_of_incoming - std::sqrt(disc);
    Vector new_dir = refractive_index * r.direction() + normal * normal_factor;

    return Ray::from_offset_and_direction(pt, new_dir);
  }

  // Total internal reflection
  out_total_internal_reflection = true;

  auto r_dir_inverse = (-r.direction()).normalize();
  auto reflector_normal_scaled = (r_dir_inverse * normal) * normal;
  auto new_dir = 2 * reflector_normal_scaled - r_dir_inverse;
  return
    Ray::from_offset_and_direction(pt + normal * Ruler::epsilon(), new_dir);
}

}

#endif
