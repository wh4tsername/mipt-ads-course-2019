#include <iostream>
#include <string>
#include <vector>

class FindNumberOfSubstrings {
 public:
  int operator()(const std::string& input_string);

 private:
  std::string string_;
  const int ALPHABET_SIZE_ = 256;
};

int FindNumberOfSubstrings::operator()(const std::string& input_string) {
  string_ = input_string + '$';

  std::vector<int> permutation(string_.size());
  std::vector<int> freq_array(ALPHABET_SIZE_);
  std::vector<std::vector<size_t>> classes(1,
                                           std::vector<size_t>(string_.size()));

  for (auto&& symbol : string_) {
    ++freq_array[symbol];
  }
  for (int i = 1; i < freq_array.size(); ++i) {
    freq_array[i] += freq_array[i - 1];
  }
  int index = 0;
  for (auto&& symbol : string_) {
    --freq_array[symbol];
    permutation[freq_array[symbol]] = index;
    ++index;
  }

  classes[0][permutation[0]] = 0;
  int class_index = 1;
  for (int i = 1; i < string_.size(); ++i) {
    if (string_[permutation[i]] != string_[permutation[i - 1]]) {
      ++class_index;
    }
    classes[0][permutation[i]] = class_index - 1;
  }

  std::vector<int> new_permutation(string_.size());
  int step = 0;
  while ((1 << step) < string_.size()) {
    for (size_t i = 0; i < string_.size(); ++i) {
      new_permutation[i] = permutation[i] - (1 << step);
      if (new_permutation[i] < 0) {
        new_permutation[i] += string_.size();
      }
    }

    freq_array.assign(class_index, 0);
    for (size_t i = 0; i < string_.size(); ++i) {
      ++freq_array[classes[step][new_permutation[i]]];
    }
    for (size_t i = 1; i < class_index; ++i) {
      freq_array[i] += freq_array[i - 1];
    }
    for (int i = static_cast<int>(string_.size()) - 1; i >= 0; --i) {
      --freq_array[classes[step][new_permutation[i]]];
      permutation[freq_array[classes[step][new_permutation[i]]]] =
          new_permutation[i];
    }

    classes.emplace_back(string_.size());
    classes[step + 1][new_permutation[0]] = 0;
    class_index = 1;
    for (size_t i = 1; i < string_.size(); ++i) {
      size_t mid1 = (permutation[i] + (1 << step)) % string_.size();
      size_t mid2 = (permutation[i - 1] + (1 << step)) % string_.size();
      if (classes[step][permutation[i]] != classes[step][permutation[i - 1]] ||
          classes[step][mid1] != classes[step][mid2]) {
        ++class_index;
      }
      classes[step + 1][permutation[i]] = class_index - 1;
    }

    ++step;
  }

  std::vector<size_t> lcp(string_.size() - 2, 0);
  for (size_t i = 1; i < string_.size() - 1; ++i) {
    size_t x = permutation[i];
    size_t y = permutation[i + 1];
    for (int k = step - 1; k >= 0; --k)
      if (classes[k][x] == classes[k][y]) {
        lcp[i - 1] += 1 << k;
        x += 1 << k;
        y += 1 << k;
      }
  }

  size_t number_of_subtrings = 0;
  for (int i = 1; i < string_.size(); ++i) {
    number_of_subtrings += (string_.size() - 1) - permutation[i];
  }
  for (int i = 0; i < string_.size() - 2; ++i) {
    number_of_subtrings -= lcp[i];
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
