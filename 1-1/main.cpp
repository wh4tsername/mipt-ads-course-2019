#include <iostream>
#include <string>
#include <vector>

class SubstringFinder {
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
  std::vector<int> z_function_values_;

  void Read() {
    std::cin >> pattern_;
    std::cin >> string_;
    string_ = pattern_ + "#" + string_;
  }

  void FindSubstring() {
    z_function_values_ = std::vector<int>(pattern_.size());
    int left = 0;
    int right = 0;
    z_function_values_[0] = 0;
    for (int i = 1; i < string_.size(); ++i) {
      int current_z =
          std::max(0, std::min(right - i, z_function_values_[i - left]));

      while (i + current_z < string_.size() &&
             string_[current_z] == string_[i + current_z]) {
        ++current_z;
      }
      if (current_z == pattern_.size()) {
        std::cout << i - pattern_.size() - 1 << " ";
      }
      if (i < pattern_.size()) {
        z_function_values_[i] = current_z;
      }
      if (i + current_z > right) {
        left = i;
        right = i + current_z;
      }
    }
  }
};

int main() {
  SubstringFinder solver;

  solver();

  return 0;
}
