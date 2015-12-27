#ifndef RAY_OBJECT_HPP
#define RAY_OBJECT_HPP

#include "bitmap.hpp"
#include "thread-context.hpp"
#include "euclid.hpp"
#include "support.hpp"

namespace ray {

class Scene;

/// Represents an object that light can interact with.  Object
/// specific behavior is implemented by overriding virtual methods on
/// this class.
class Object {
  /// The object identifier of this object within the containing scene.
  ///
  /// Every object gets an unique and dense ID in the namespace of the scene
  /// containing it.  This index can be used to index into object specific data
  /// in \p Context.
  unsigned _object_id = 0;

  /// The scene containing this object.  There can only be one.
  const Scene &_container;

  /// A string describing the object.
  std::string _desc;

public:
  explicit Object(const Scene &container, std::string desc)
      : _container(container), _desc(std::move(desc)) {}

  unsigned object_id() const { return _object_id; }
  void set_object_id(unsigned obj_id) { _object_id = obj_id; }

  const Scene &container() const { return _container; }

  /// Subclasses extend this method to initialize the object's data in \p ctx.
  virtual void initialize(ThreadContext &ctx) const {
    ctx.get(object_id()) = 0;
  }

  /// The key method that implements the object's interaction with light.
  ///
  /// This method intersects ray \p r with this object, returning true if the
  /// ray does intersect with this object and false if it does not.  On
  /// intersection, the color of ray is returned in \p out_pixel, and the point
  /// of incidence is returned in \p out_incidence_k.
  virtual bool incident(ThreadContext &ctx, Logger &l, const Ray &r,
                        double current_smallest_k, double &out_incidence_k,
                        Color &out_pixel) const = 0;

  /// Return a string describing the object.
  const std::string &description() const { return _desc; }
};
}

#endif
