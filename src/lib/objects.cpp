#include "objects.hpp"

using namespace ray;

BoxObject::BoxObject() {
  Plane p(Vector::get_i(), Vector::get_i() * 500);
  std::vector<Vector> points;

  Vector base = Vector::get_i() * 500;
  Vector side_a = (Vector::get_k() - Vector::get_j()) * 100;
  Vector side_b = (Vector::get_k() + Vector::get_j()) * 100;
  points.push_back(base + side_a);
  points.push_back(base + side_b);
  points.push_back(base - side_a);
  points.push_back(base - side_b);

  _plane = ConvexPlaneSegment(p, std::move(points));
}

bool BoxObject::incident(const Scene &scene, const Ray &incoming,
                         double &out_k, Color &out_c) {
  if (_plane.intersect(incoming, out_k)) {
    out_c = Color::create_red();
    return true;
  }
  return false;
}
