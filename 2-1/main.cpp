#include <iostream>
#include <string>
#include <vector>

class FindNumberOfSubstrings {
 public:
  int operator()(const std::string& input_string);

 private:
  void SplitIntoEquivalenceClasses();
  void FindLcp();

  std::string string_;
  std::vector<std::vector<size_t>> classes_;
  std::vector<int> permutation_;
  int step_ = 0;
  std::vector<size_t> lcp_;
  const int ALPHABET_SIZE_ = 256;
};

void FindNumberOfSubstrings::FindLcp() {
  lcp_.assign(string_.size() - 2, 0);
  for (size_t i = 1; i < string_.size() - 1; ++i) {
    size_t x = permutation_[i];
    size_t y = permutation_[i + 1];
    for (int k = step_ - 1; k >= 0; --k)
      if (classes_[k][x] == classes_[k][y]) {
        lcp_[i - 1] += 1 << k;
        x += 1 << k;
        y += 1 << k;
      }
  }
}

void FindNumberOfSubstrings::SplitIntoEquivalenceClasses() {
  permutation_.assign(string_.size(), 0);
  std::vector<int> count_array(ALPHABET_SIZE_);
  classes_.assign(1, std::vector<size_t>(string_.size()));

  for (auto&& symbol : string_) {
    ++count_array[symbol];
  }
  for (int i = 1; i < count_array.size(); ++i) {
    count_array[i] += count_array[i - 1];
  }
  int index = 0;
  for (auto&& symbol : string_) {
    --count_array[symbol];
    permutation_[count_array[symbol]] = index;
    ++index;
  }

  classes_[0][permutation_[0]] = 0;
  int class_index = 1;
  for (int i = 1; i < string_.size(); ++i) {
    if (string_[permutation_[i]] != string_[permutation_[i - 1]]) {
      ++class_index;
    }
    classes_[0][permutation_[i]] = class_index - 1;
  }

  std::vector<int> new_permutation(string_.size());
  while ((1 << step_) < string_.size()) {
    for (size_t i = 0; i < string_.size(); ++i) {
      new_permutation[i] = permutation_[i] - (1 << step_);
      if (new_permutation[i] < 0) {
        new_permutation[i] += string_.size();
      }
    }

    count_array.assign(class_index, 0);
    for (size_t i = 0; i < string_.size(); ++i) {
      ++count_array[classes_[step_][new_permutation[i]]];
    }
    for (size_t i = 1; i < class_index; ++i) {
      count_array[i] += count_array[i - 1];
    }
    for (int i = static_cast<int>(string_.size()) - 1; i >= 0; --i) {
      --count_array[classes_[step_][new_permutation[i]]];
      permutation_[count_array[classes_[step_][new_permutation[i]]]] =
          new_permutation[i];
    }

    classes_.emplace_back(string_.size());
    classes_[step_ + 1][new_permutation[0]] = 0;
    class_index = 1;
    for (size_t i = 1; i < string_.size(); ++i) {
      size_t mid1 = (permutation_[i] + (1 << step_)) % string_.size();
      size_t mid2 = (permutation_[i - 1] + (1 << step_)) % string_.size();
      if (classes_[step_][permutation_[i]] !=
              classes_[step_][permutation_[i - 1]] ||
          classes_[step_][mid1] != classes_[step_][mid2]) {
        ++class_index;
      }
      classes_[step_ + 1][permutation_[i]] = class_index - 1;
    }

    ++step_;
  }
}

int FindNumberOfSubstrings::operator()(const std::string& input_string) {
  string_ = input_string + '$';

  SplitIntoEquivalenceClasses();
  FindLcp();

  size_t number_of_subtrings = 0;
  for (int i = 1; i < string_.size(); ++i) {
    number_of_subtrings += (string_.size() - 1) - permutation_[i];
  }
  for (int i = 0; i < string_.size() - 2; ++i) {
    number_of_subtrings -= lcp_[i];
  }

  return number_of_subtrings;
}

int main() {
  std::string input_string;

  std::cin >> input_string;

  FindNumberOfSubstrings solver;
  std::cout << solver(input_string);

  return 0;
}
