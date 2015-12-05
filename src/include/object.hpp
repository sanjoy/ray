#ifndef RAY_OBJECT_HPP
#define RAY_OBJECT_HPP

#include "bitmap.hpp"
#include "context.hpp"
#include "euclid.hpp"

namespace ray {

class Scene;

class Object {
  unsigned _object_id = 0;

public:
  unsigned object_id() const { return _object_id; }
  void set_object_id(unsigned obj_id) { _object_id = obj_id; }

  virtual void initialize(Context &ctx) const { ctx.get(object_id()) = 0; }

  virtual bool incident(Context &ctx, const Scene &scene, const Ray &r,
                        double current_smallest_k, double &out_incidence_k,
                        Color &out_pixel) const = 0;
};
}

#endif
