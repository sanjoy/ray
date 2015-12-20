#include "camera.hpp"

#include <thread>

#include "context.hpp"

using namespace ray;

template <typename RenderFnTy> struct ThreadTask {
public:
  class Point {
    int _x;
    int _y;

  public:
    /* implict */ Point(int x, int y) : _x(x), _y(y) {}

    int x() const { return _x; }
    int y() const { return _y; }
  };

  explicit ThreadTask(Point top_left, Point bottom_right, RenderFnTy &render_fn,
                      Scene &s)
      : _top_left(top_left), _bottom_right(bottom_right), _render_fn(render_fn),
        _ctx(s.object_count()) {
    _result.reset(new Color[(bottom_right.x() - top_left.x()) *
                            (bottom_right.y() - top_left.y())]);
    s.init_object_ids(_ctx);
  }

  void do_threaded_work() {
    auto &render_fn = _render_fn;
    for (int xi = _top_left.x(), xe = _bottom_right.x(); xi != xe; ++xi)
      for (int yi = _top_left.y(), ye = _bottom_right.y(); yi != ye; ++yi)
        at(xi, yi) = render_fn(xi, yi, _ctx);
  }

  Context context() { return _ctx; }

  template <typename DrainFnTy> void drain_work(const DrainFnTy &drain_fn) {
    for (int xi = _top_left.x(), xe = _bottom_right.x(); xi != xe; ++xi)
      for (int yi = _top_left.y(), ye = _bottom_right.y(); yi != ye; ++yi)
        drain_fn(xi, yi, at(xi, yi));
  }

private:
  Point _top_left;
  Point _bottom_right;
  RenderFnTy &_render_fn;
  Context _ctx;
  std::unique_ptr<Color[]> _result;

  Color &at(int x, int y) {
    int x_offset = x - _top_left.x();
    int y_offset = y - _top_left.y();
    int y_size = _bottom_right.y() - _top_left.y();

    return _result[x_offset * y_size + y_offset];
  }
};

Camera::Camera(double focal_length, unsigned screen_width_px,
               unsigned screen_height_px, unsigned screen_resolution,
               const Vector &pos)
    : _focal_length(focal_length), _screen_width_px(screen_width_px),
      _screen_height_px(screen_height_px),
      _screen_resolution(screen_resolution), _focus_position(pos) {}

Bitmap Camera::snap(Scene &scene, unsigned thread_count) {
  Bitmap bmp(_screen_height_px, _screen_width_px, Color::create_blue());

  double max_diag_square =
      std::pow(_screen_height_px / 2, 2) + std::pow(_screen_width_px / 2, 2);

  double focal_length = _focal_length;
  unsigned resolution = _screen_resolution;
  auto focus = _focus_position;

  auto render_one_pixel = [&](int x, int y, Context &ctx) {
    double scale = 1.0 + (x * x + y * y) / max_diag_square;
    const Vector sample_pt(focal_length, (x * scale) / resolution,
                           (y * scale) / resolution);
    return scene.render_pixel(Ray::from_two_points(focus, focus + sample_pt),
                              ctx);
  };

  typedef decltype(render_one_pixel) RenderFnTy;

  std::vector<ThreadTask<RenderFnTy>> subtasks;
  std::vector<std::thread> threads;

  int x_begin = -(_screen_width_px / 2);
  int x_delta = _screen_width_px / thread_count;

  for (int i = 0; i < thread_count; i++) {
    int x_end =
        i == (thread_count - 1) ? (_screen_width_px / 2) : (x_begin + x_delta);
    ThreadTask<RenderFnTy>::Point p0(x_begin, -(_screen_height_px / 2));
    ThreadTask<RenderFnTy>::Point p1(x_end, (_screen_height_px / 2));
    subtasks.emplace_back(p0, p1, render_one_pixel, scene);
    x_begin = x_end;
  }

  for (int i = 0; i < thread_count; i++) {
    auto *task = &subtasks[i];
    threads.emplace_back([task]() { task->do_threaded_work(); });
  }

  int x_bmp_delta = _screen_width_px / 2, y_bmp_delta = _screen_height_px / 2;

  for (int i = 0; i < thread_count; i++) {
    threads[i].join();
    subtasks[i].drain_work(
        [&bmp, x_bmp_delta, y_bmp_delta](int x, int y, const Color &c) {
          bmp.at(x + x_bmp_delta, y + y_bmp_delta) = c;
        });
  }

  return bmp;
}
