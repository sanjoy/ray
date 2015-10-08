#ifndef RAY_OBJECT_HPP
#define RAY_OBJECT_HPP

#include "bitmap.hpp"
#include "euclid.hpp"

namespace ray {

class Scene;

class Object {
public:
  virtual bool incident(const Scene &scene, const Ray &r,
                        double current_smallest_k, double &out_incidence_k,
                        Color &out_pixel) = 0;
};
}

#endif
