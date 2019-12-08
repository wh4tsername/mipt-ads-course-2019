#include <algorithm>
#include <iostream>
#include <vector>

struct Point {
  explicit Point(double x = 0, double y = 0) : x_(x), y_(y) {}

  double x_ = 0;
  double y_ = 0;
};

using Vector = Point;

Point operator+(const Point& lhs, const Point& rhs) {
  return Point(lhs.x_ + rhs.x_, lhs.y_ + rhs.y_);
}

Point operator-(const Point& lhs, const Point& rhs) {
  return Point(lhs.x_ - rhs.x_, lhs.y_ - rhs.y_);
}

bool operator<(const Point& lhs, const Point& rhs) {
  return (lhs.x_ < rhs.x_) || (lhs.x_ == rhs.x_ && lhs.y_ < rhs.y_);
}

double Product(const Point& lhs, const Point& rhs) {
  return lhs.x_ * rhs.x_ + lhs.y_ * rhs.y_;
}

bool AreClockwise(const Point& first, const Point& second) {
  return first.y_ * second.x_ <= first.x_ * second.y_;
}

struct Polygon {
  int LowerLeftCornerPoint() {
    int lower_left_corner = 0;
    for (int index = 1; index < polygon_.size(); ++index) {
      if (polygon_[index] < polygon_[lower_left_corner]) {
        lower_left_corner = index;
      }
    }
    return lower_left_corner;
  }

  std::vector<Point> polygon_;
  size_t number_of_vertices_ = 0;
};

class IntersectionChecker {
 public:
  void Read();
  bool HaveIntersection();

 private:
  void MinkowskiAddition();
  void RearrangePolygons(size_t first_min_point_index,
                         size_t second_min_point_index);
  bool HasZero();

  Polygon first_polygon_;
  Polygon second_polygon_;
  Polygon minkowski_addition_;
};

void IntersectionChecker::Read() {
  int number_of_vertices = 0;
  std::cin >> number_of_vertices;
  first_polygon_.number_of_vertices_ = number_of_vertices;

  Point point;
  for (size_t i = 0; i < number_of_vertices; ++i) {
    std::cin >> point.x_ >> point.y_;
    first_polygon_.polygon_.emplace_back(point);
  }
  std::reverse(first_polygon_.polygon_.begin(), first_polygon_.polygon_.end());

  std::cin >> number_of_vertices;
  second_polygon_.number_of_vertices_ = number_of_vertices;

  for (size_t i = 0; i < number_of_vertices; ++i) {
    std::cin >> point.x_ >> point.y_;
    second_polygon_.polygon_.emplace_back(-point.x_, -point.y_);
  }
  std::reverse(second_polygon_.polygon_.begin(),
               second_polygon_.polygon_.end());
}

bool IntersectionChecker::HaveIntersection() {
  MinkowskiAddition();

  return HasZero();
}

void IntersectionChecker::MinkowskiAddition() {
  int first_min_point_index = first_polygon_.LowerLeftCornerPoint();
  int second_min_point_index = second_polygon_.LowerLeftCornerPoint();
  RearrangePolygons(first_min_point_index, second_min_point_index);
  first_polygon_.polygon_.emplace_back(first_polygon_.polygon_[0]);
  second_polygon_.polygon_.emplace_back(second_polygon_.polygon_[0]);

  size_t i = 0;
  size_t j = 0;
  while (i < first_polygon_.number_of_vertices_ - 1 &&
         j < second_polygon_.number_of_vertices_ - 1) {
    minkowski_addition_.polygon_.emplace_back(first_polygon_.polygon_[i] +
                                              second_polygon_.polygon_[j]);

    Vector first_vector =
        first_polygon_.polygon_[i + 1] - first_polygon_.polygon_[i];
    Vector second_vector =
        second_polygon_.polygon_[j + 1] - second_polygon_.polygon_[j];

    if (AreClockwise(first_vector, second_vector)) {
      ++i;
    }
    if (!AreClockwise(first_vector, second_vector) ||
        first_vector.y_ * second_vector.x_ ==
            first_vector.x_ * second_vector.y_) {
      ++j;
    }
  }
  while (i < first_polygon_.number_of_vertices_ - 1) {
    minkowski_addition_.polygon_.emplace_back(first_polygon_.polygon_[i] +
                                              second_polygon_.polygon_[j]);
    ++i;
  }
  while (j < second_polygon_.number_of_vertices_ - 1) {
    minkowski_addition_.polygon_.emplace_back(first_polygon_.polygon_[i] +
                                              second_polygon_.polygon_[j]);
    ++j;
  }
  first_polygon_.polygon_.pop_back();
  second_polygon_.polygon_.pop_back();

  minkowski_addition_.number_of_vertices_ = minkowski_addition_.polygon_.size();
}

void IntersectionChecker::RearrangePolygons(size_t first_min_point_index,
                                            size_t second_min_point_index) {
  std::vector<Point> first_buffer_vector;
  int i = first_min_point_index;
  do {
    first_buffer_vector.emplace_back(first_polygon_.polygon_[i]);
    ++i;
    if (i == first_polygon_.polygon_.size()) {
      i = 0;
    }
  } while (i != first_min_point_index);
  first_polygon_.polygon_ = std::move(first_buffer_vector);

  std::vector<Point> second_buffer_vector;
  i = second_min_point_index;
  do {
    second_buffer_vector.emplace_back(second_polygon_.polygon_[i]);
    ++i;
    if (i == second_polygon_.polygon_.size()) {
      i = 0;
    }
  } while (i != second_min_point_index);
  second_polygon_.polygon_ = second_buffer_vector;
}

bool IntersectionChecker::HasZero() {
  minkowski_addition_.polygon_.emplace_back(minkowski_addition_.polygon_[0]);

  for (size_t i = 0; i < minkowski_addition_.polygon_.size() - 1; ++i) {
    if (!AreClockwise(minkowski_addition_.polygon_[i],
                      minkowski_addition_.polygon_[i + 1])) {
      return false;
    }
  }

  minkowski_addition_.polygon_.pop_back();

  return true;
}

int main() {
  IntersectionChecker checker;

  checker.Read();

  std::cout << (checker.HaveIntersection() ? "YES" : "NO");

  return 0;
}
