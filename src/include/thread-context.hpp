/// thread-context.hpp: Per-thread, per-object state.

#ifndef RAY_CONTEXT_HPP
#define RAY_CONTEXT_HPP

#include <memory>

#include "support.hpp"

namespace ray {

/// Objects have per render-thread state.  This provides thread local
/// storage for objects.
class ThreadContext {
  std::unique_ptr<intptr_t[]> _obj_data;
  unsigned _obj_count;
  Logger _logger;

public:
  explicit ThreadContext(unsigned obj_count, bool enable_logger)
      : _obj_data(new intptr_t[obj_count]), _obj_count(obj_count),
        _logger(enable_logger) {
    (void)_obj_count;
  }

  Logger &logger() { return _logger; }

  /// Retrieve the thread local storage for the object with object id \p obj_id.
  intptr_t &get(unsigned obj_id) {
    assert(obj_id < _obj_count && "Out of bounds!");
    return _obj_data[obj_id];
  }
};
};

#endif
