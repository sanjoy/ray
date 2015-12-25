/// newton.hpp: Helper routines that encode properties of light.

#ifndef RAY_NEWTON_HPP
#define RAY_NEWTON_HPP

#include "euclid.hpp"
#include "support.hpp"

namespace ray {

/// Reflect the ray \p r given that it is incident at point \p pt with
/// normal \p normal.
inline Ray get_reflected_ray(const Ray &r, const Vector &pt,
                             const Vector &normal) {
  auto incoming_dir_inverse = (-r.direction()).normalize();
  auto normal_scaled = (incoming_dir_inverse * normal) * normal;
  auto new_dir = 2 * normal_scaled - incoming_dir_inverse;
  return Ray::from_offset_and_direction(pt + normal * Ruler::epsilon(),
                                        new_dir);
}

/// Refract the ray \p r given that it is incident at point \p pt with
/// normal \p normal on a boundary with the relative refractive index
/// \p relative.  \p out_total_internal_reflection is set to true if
/// the ray encountered total internal reflection.
inline Ray get_refracted_ray(const Ray &r, const Vector &pt, Vector normal,
                             double relative_refractive_index,
                             bool &out_total_internal_reflection) {
  Vector incoming_dir = r.direction().normalize();
  double cos_of_incoming = incoming_dir * normal;
  double sin_of_incoming_sq = 1.0 - std::pow(cos_of_incoming, 2);
  double sin_of_outgoing_sq =
      std::pow(relative_refractive_index, 2) * sin_of_incoming_sq;

  if (sin_of_outgoing_sq <= 1.0) {
    out_total_internal_reflection = false;
    double tan_of_outgoing_sq = sin_of_outgoing_sq / (1 - sin_of_outgoing_sq);
    double tan_of_outgoing = std::sqrt(tan_of_outgoing_sq);
    Vector orth = (incoming_dir + normal).normalize();
    Vector new_dir = orth * (-1.0 * tan_of_outgoing) - normal;
    return Ray::from_offset_and_direction(pt + normal * Ruler::epsilon(),
                                          new_dir);
  }

  out_total_internal_reflection = true;
  return get_reflected_ray(r, pt, normal);
}
}

#endif
