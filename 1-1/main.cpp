#include <iostream>
#include <string>
#include <vector>

class Solver {
 public:
  void operator()() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    Read();
    FindSubstring();
  }

 private:
  std::string pattern_;
  std::string string_;
  std::vector<int> z_;

  void Read() {
    std::cin >> pattern_;
    std::cin >> string_;
    string_ = pattern_ + "#" + string_;
  }

  void FindSubstring() {
    z_ = std::vector<int>(pattern_.size());
    int left = 0;
    int right = 0;
    z_[0] = 0;
    for (int i = 1; i < string_.size(); ++i) {
      int current_z = std::max(0, std::min(right - i, z_[i - left]));
      
      while (i + current_z < string_.size() &&
             string_[current_z] == string_[i + current_z]) {
        ++current_z;
      }
      if (current_z == pattern_.size()) {
        std::cout << i - pattern_.size() - 1 << " ";
      }
      if (i < pattern_.size()) {
        z_[i] = current_z;
      }
      if (i + current_z > right) {
        left = i;
        right = i + current_z;
      }
    }
  }
};

int main() {
  Solver solver;

  solver();

  return 0;
}
