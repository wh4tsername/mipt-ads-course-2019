#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

struct Vector : std::enable_shared_from_this<Vector> {
  explicit Vector(double x = 0, double y = 0, double z = 0, int id = -1)
      : x_(x), y_(y), z_(z), id_(id) {}

  bool Act() {
    std::shared_ptr<Vector> temp(this);
    if (prev_->next_ == temp) {
      prev_->next_ = next_;
      next_->prev_ = prev_;
      return false;
    }
    prev_->next_ = temp;
    next_->prev_ = temp;
    return true;
  }

  double x_ = 0;
  double y_ = 0;
  double z_ = 0;
  int id_ = -1;
  std::shared_ptr<Vector> prev_ = nullptr;
  std::shared_ptr<Vector> next_ = nullptr;
};

using Point = Vector;

void RotateAroundThirdDimension(double& x, double& y, double angle) {
  double new_x = 0;
  double new_y = 0;

  new_x = x * cos(angle) + y * sin(angle);
  new_y = -x * sin(angle) + y * cos(angle);
  x = new_x;
  y = new_y;
}

Vector VectorProduct(const Vector& first_vector, const Vector& second_vector) {
  return Vector(
      first_vector.y_ * second_vector.z_ - first_vector.z_ * second_vector.y_,
      first_vector.z_ * second_vector.x_ - first_vector.x_ * second_vector.z_,
      first_vector.x_ * second_vector.y_ - first_vector.y_ * second_vector.x_);
}

Point operator+(const Point& lhs, const Point& rhs) {
  return Point(lhs.x_ + rhs.x_, lhs.y_ + rhs.y_, lhs.z_ + rhs.z_);
}

Point operator-(const Point& lhs, const Point& rhs) {
  return Point(lhs.x_ - rhs.x_, lhs.y_ - rhs.y_, lhs.z_ - rhs.z_);
}

class ConvexHull {
 public:
  explicit ConvexHull(std::vector<Point> point);

  void Print();

 private:
  using Face = std::tuple<size_t, size_t, size_t>;

  static void Permute(Face& face) {
    size_t first_point = std::get<0>(face);
    size_t second_point = std::get<1>(face);
    size_t third_point = std::get<2>(face);
    if (second_point < first_point && second_point < third_point) {
      face = {second_point, third_point, first_point};
    } else if (third_point < first_point && third_point < second_point) {
      face = {third_point, first_point, second_point};
    }
  }

  void InitializeDownHull(size_t left_size, size_t right_size);
  std::vector<std::shared_ptr<Point>> DownHull(size_t left_size,
                                               size_t right_size);

  static bool IsClockwise(const std::shared_ptr<Point>& first_point,
                          const std::shared_ptr<Point>& second_point,
                          const std::shared_ptr<Point>& third_point);
  static double Time(const std::shared_ptr<Point>& first_point,
                     const std::shared_ptr<Point>& second_point,
                     const std::shared_ptr<Point>& third_point);

  static void FindSupportingRib(std::shared_ptr<Point>& u,
                                std::shared_ptr<Point>& v);
  static void ProcessStates(
      std::shared_ptr<Point>& u, std::shared_ptr<Point>& v,
      std::pair<std::vector<std::shared_ptr<Point>>,
                std::vector<std::shared_ptr<Point>>>& parts_of_hull,
      std::vector<std::shared_ptr<Point>> part_of_hull);

  constexpr static const double INF = 1e9;
  int number_of_vertices_ = 0;
  std::vector<Point> points_;
  std::vector<Face> hull_;
  bool is_down_hull_built = false;
};

ConvexHull::ConvexHull(std::vector<Point> points) : points_(std::move(points)) {
  number_of_vertices_ = points_.size();
  InitializeDownHull(0, number_of_vertices_);

  for (auto&& point : points_) {
    point.next_ = nullptr;
    point.prev_ = nullptr;
    point.z_ *= -1;
  }
  is_down_hull_built = true;

  InitializeDownHull(0, number_of_vertices_);
  for (auto&& face : hull_) {
    Permute(face);
  }
  std::sort(hull_.begin(), hull_.end());
}

void ConvexHull::InitializeDownHull(size_t left_size, size_t right_size) {
  std::vector<std::shared_ptr<Point>> events = DownHull(left_size, right_size);

  for (auto&& event : events) {
    Face face(event->prev_->id_, event->id_, event->next_->id_);

    if (!event->Act() ^ is_down_hull_built) {
      std::swap(std::get<0>(face), std::get<1>(face));
    }
    hull_.emplace_back(face);
  }
}

void ConvexHull::FindSupportingRib(std::shared_ptr<Point>& u,
                                   std::shared_ptr<Point>& v) {
  while (true) {
    if (IsClockwise(u, v, v->next_)) {
      v = v->next_;
    } else if (IsClockwise(u->prev_, u, v)) {
      u = u->prev_;
    } else {
      break;
    }
  }
}

void ConvexHull::ProcessStates(
    std::shared_ptr<Point>& u, std::shared_ptr<Point>& v,
    std::pair<std::vector<std::shared_ptr<Point>>,
              std::vector<std::shared_ptr<Point>>>& parts_of_hull,
    std::vector<std::shared_ptr<Point>> part_of_hull) {
  double time = -INF;
  size_t first_part = 0;
  size_t second_part = 0;
  while (true) {
    std::shared_ptr<Point> left = nullptr;
    std::shared_ptr<Point> right = nullptr;
    std::vector<double> next_time(6, INF);

    if (first_part < parts_of_hull.first.size()) {
      left = parts_of_hull.first[first_part];
      next_time[0] = Time(left->prev_, left, left->next_);
    }
    if (second_part < parts_of_hull.second.size()) {
      right = parts_of_hull.second[second_part];
      next_time[1] = Time(right->prev_, right, right->next_);
    }
    next_time[2] = Time(u->prev_, u, v);
    next_time[3] = Time(u, u->next_, v);
    next_time[4] = Time(u, v, v->next_);
    next_time[5] = Time(u, v->prev_, v);

    int min_time_index = -1;
    double min_time = INF;
    for (size_t i = 0; i < 6; ++i) {
      if (next_time[i] > time && next_time[i] < min_time) {
        min_time = next_time[i];
        min_time_index = i;
      }
    }
    if (min_time_index == -1 || min_time >= INF) {
      break;
    }

    switch (min_time_index) {
      case 0:
        if (left->x_ < u->x_) {
          part_of_hull.emplace_back(left);
        }
        left->Act();
        ++first_part;
        break;
      case 1:
        if (right->x_ > v->x_) {
          part_of_hull.emplace_back(right);
        }
        right->Act();
        ++second_part;
        break;
      case 2:
        part_of_hull.emplace_back(u);
        u = u->prev_;
        break;
      case 3:
        u = u->next_;
        part_of_hull.emplace_back(u);
        break;
      case 4:
        part_of_hull.emplace_back(v);
        v = v->next_;
        break;
      case 5:
        v = v->prev_;
        part_of_hull.emplace_back(v);
        break;
      default:
        break;
    }
    time = min_time;
  }
}

std::vector<std::shared_ptr<Point>> ConvexHull::DownHull(size_t left_size,
                                                         size_t right_size) {
  if (right_size - left_size == 1) {
    return std::vector<std::shared_ptr<Point>>();
  }

  size_t middle = (left_size + right_size) / 2;
  std::pair<std::vector<std::shared_ptr<Point>>,
            std::vector<std::shared_ptr<Point>>>
      parts_of_hull;
  parts_of_hull.first = DownHull(left_size, middle);
  parts_of_hull.second = DownHull(middle, right_size);

  std::vector<std::shared_ptr<Point>> part_of_hull;
  std::shared_ptr<Point> u = std::make_shared<Point>(points_[middle - 1]);
  std::shared_ptr<Point> v = std::make_shared<Point>(points_[middle]);

  FindSupportingRib(u, v);
  ProcessStates(u, v, parts_of_hull, part_of_hull);

  u->next_ = v;
  v->prev_ = u;
  for (int i = static_cast<int>(part_of_hull.size() - 1); i >= 0; --i) {
    std::shared_ptr<Point> current = part_of_hull[i];
    if (current->x_ > u->x_ && current->x_ < v->x_) {
      u->next_ = current;
      v->prev_ = current;
      current->prev_ = u;
      current->next_ = v;
      if (current->x_ <= points_[middle - 1].x_) {
        u = current;
      } else {
        v = current;
      }
    } else {
      current->Act();
      if (current == u) {
        u = u->prev_;
      }
      if (current == v) {
        v = v->next_;
      }
    }
  }

  return part_of_hull;
}

bool ConvexHull::IsClockwise(const std::shared_ptr<Point>& first_point,
                             const std::shared_ptr<Point>& second_point,
                             const std::shared_ptr<Point>& third_point) {
  if (first_point == nullptr || second_point == nullptr ||
      third_point == nullptr) {
    return false;
  }
  return VectorProduct(Vector(*second_point - *first_point),
                       Vector(*third_point - *second_point))
             .z_ < 0;
}

double ConvexHull::Time(const std::shared_ptr<Point>& first_point,
                        const std::shared_ptr<Point>& second_point,
                        const std::shared_ptr<Point>& third_point) {
  if (first_point == nullptr || second_point == nullptr ||
      third_point == nullptr) {
    return INF;
  }

  Vector vector_product = VectorProduct(Vector(*second_point - *first_point),
                                        Vector(*third_point - *second_point));

  return -vector_product.y_ / vector_product.z_;
}

void ConvexHull::Print() {
  std::cout << hull_.size() << std::endl;
  for (auto&& face : hull_) {
    std::cout << "3 " << std::get<0>(face) << " " << std::get<1>(face) << " "
              << std::get<2>(face) << std::endl;
  }
}

int main() {
  size_t number_of_tests = 0;
  std::cin >> number_of_tests;

  for (size_t i = 0; i < number_of_tests; ++i) {
    const double SHIFT_ANGLE = 0.01;
    size_t number_of_vertices = 0;
    std::cin >> number_of_vertices;
    std::vector<Point> points(number_of_vertices, Point());
    int id = 0;
    for (auto&& point : points) {
      std::cin >> point.x_ >> point.y_ >> point.z_;
      point.id_ = id;
      ++id;
      RotateAroundThirdDimension(point.x_, point.y_, SHIFT_ANGLE);
      RotateAroundThirdDimension(point.z_, point.x_, SHIFT_ANGLE);
      RotateAroundThirdDimension(point.y_, point.z_, SHIFT_ANGLE);
    }

    std::sort(points.begin(), points.end(),
              [](auto lhs, auto rhs) { return lhs.x_ < rhs.x_; });

    ConvexHull convex_hull(points);

    convex_hull.Print();
  }

  return 0;
}
