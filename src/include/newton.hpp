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
  double inv_ref_index = 1.0 / relative_refractive_index;

  // Snell's law in vector form:
  //
  // s2 = (n1 / n2) (N x (-N x s1)) - N * sqrt(1 - D)
  //  D = ((n1 / n2)^2) ((N x s1) . (N x s1))
  // ref_index = n2 / n1

  Vector n_cross_s1 = normal.cross_product(incoming_dir);
  double D = std::pow(inv_ref_index, 2) * (n_cross_s1 * n_cross_s1);

  if (D <= 1.0) {
    out_total_internal_reflection = false;

    Vector s2 = inv_ref_index * normal.cross_product(-n_cross_s1) -
                (normal * std::sqrt(1 - D));
    return Ray::from_offset_and_direction(pt + normal * Ruler::epsilon(), s2);
  }

  out_total_internal_reflection = true;
  return get_reflected_ray(r, pt, normal);
}
}

#endif
