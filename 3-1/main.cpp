#include <cmath>
#include <iomanip>
#include <iostream>

struct Segment;

struct Vector {
  explicit Vector(double x = 0, double y = 0, double z = 0)
      : x_(x), y_(y), z_(z) {}

  explicit Vector(const Segment& segment);

  double Module() { return pow((x_ * x_ + y_ * y_ + z_ * z_), 0.5); }

  double x_;
  double y_;
  double z_;
};

using Point = Vector;

struct Segment {
  explicit Segment(const Point& first_point, const Point& second_point)
      : first_point_(first_point), second_point_(second_point) {}

  Point first_point_;
  Point second_point_;
};

Vector::Vector(const Segment& segment) {
  x_ = segment.first_point_.x_ - segment.second_point_.x_;
  y_ = segment.first_point_.y_ - segment.second_point_.y_;
  z_ = segment.first_point_.z_ - segment.second_point_.z_;
}

Vector operator-(const Vector& first_vector, const Vector& second_vector) {
  Vector result;
  result.x_ = first_vector.x_ - second_vector.x_;
  result.y_ = first_vector.y_ - second_vector.y_;
  result.z_ = first_vector.z_ - second_vector.z_;
  return result;
}

Vector operator+(const Vector& first_vector, const Vector& second_vector) {
  Vector result;
  result.x_ = first_vector.x_ + second_vector.x_;
  result.y_ = first_vector.y_ + second_vector.y_;
  result.z_ = first_vector.z_ + second_vector.z_;
  return result;
}

Vector operator*(double constant, const Vector& vector) {
  Vector result;
  result.x_ = constant * vector.x_;
  result.y_ = constant * vector.y_;
  result.z_ = constant * vector.z_;
  return result;
}

double Product(const Vector& first_vector, const Vector& second_vector) {
  return first_vector.x_ * second_vector.x_ +
         first_vector.y_ * second_vector.y_ +
         first_vector.z_ * second_vector.z_;
}

double FindDistance(const Point& point, const Segment& segment) {
  Vector vector = (-1) * Vector(segment);
  auto p0 = Vector(point);
  auto b = Vector(segment.first_point_);
  double parameter =
      (Product(p0, vector) - Product(b, vector)) / (Product(vector, vector));
  if (parameter <= 1 && parameter >= 0) {
    return (b + parameter * vector - p0).Module();
  }
  if (parameter < 0) {
    return (b - p0).Module();
  }
  return (b + vector - p0).Module();
}

double FindDistance(const Segment& first_segment,
                    const Segment& second_segment) {
  Vector u(first_segment);
  Vector v(second_segment);
  u = (-1) * u;
  v = (-1) * v;

  Point w0 = first_segment.first_point_ - second_segment.first_point_;

  double a = Product(u, u);
  double b = Product(u, v);
  double c = Product(v, v);
  double d = Product(u, Vector(w0));
  double e = Product(v, Vector(w0));

  if (a * c != b * b) {
    double s_c = (b * e - c * d) / (a * c - b * b);
    double t_c = (a * e - b * d) / (a * c - b * b);
    if (s_c <= 1 && s_c >= 0 && t_c >= 0 && t_c <= 1) {
      double x = w0.x_ + s_c * u.x_ - t_c * v.x_;
      double y = w0.y_ + s_c * u.y_ - t_c * v.y_;
      double z = w0.z_ + s_c * u.z_ - t_c * v.z_;

      Vector distance(x, y, z);

      return distance.Module();
    }
  }
  double j1 = FindDistance(first_segment.first_point_, second_segment);
  double j2 = FindDistance(first_segment.second_point_, second_segment);
  double j3 = FindDistance(second_segment.first_point_, first_segment);
  double j4 = FindDistance(second_segment.second_point_, first_segment);
  double dist = std::min(std::min(j1, j2), std::min(j3, j4));
  return dist;
}

int main() {
  Point first_point;
  Point second_point;
  Point third_point;
  Point fourth_point;

  std::cin >> first_point.x_ >> first_point.y_ >> first_point.z_;
  std::cin >> second_point.x_ >> second_point.y_ >> second_point.z_;
  std::cin >> third_point.x_ >> third_point.y_ >> third_point.z_;
  std::cin >> fourth_point.x_ >> fourth_point.y_ >> fourth_point.z_;

  Segment first_segment(first_point, second_point);
  Segment second_segment(third_point, fourth_point);

  std::cout << std::setprecision(12)
            << FindDistance(first_segment, second_segment);

  return 0;
}
