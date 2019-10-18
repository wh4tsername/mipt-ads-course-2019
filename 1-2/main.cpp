#include <cassert>
#include <iostream>
#include <vector>
#include <set>
#include <string>

std::vector<int> PiFunction(const std::string& string) {
  std::vector<int> pi_function_values(string.size());
  pi_function_values[0] = 0;
  for (size_t i = 1; i < string.size(); ++i) {
    int border = pi_function_values[i - 1];
    while (border > 0 && string[i] != string[border]) {
      border = pi_function_values[border - 1];
    }
    if (string[i] == string[border]) {
      ++border;
    }
    pi_function_values[i] = border;
  }
  return pi_function_values;
}

std::vector<int> ZFunction(const std::string& string) {
  std::vector<int> z_function_values(string.size());
  int left = 0;
  int right = 0;
  z_function_values[0] = 0;
  for (int i = 1; i < string.size(); ++i) {
    z_function_values[i] = std::max(0, std::min(right - i, z_function_values[i - left]));
    while (i + z_function_values[i] < string.size() && string[z_function_values[i]] == string[i + z_function_values[i]]) {
      ++z_function_values[i];
    }
    if (i + z_function_values[i] > right) {
      left = i;
      right = i + z_function_values[i];
    }
  }
  z_function_values[0] = string.size();
  return z_function_values;
}

std::vector<int> ConvertZToPi(const std::vector<int>& z_function_values) {
  std::vector<int> pi_function_values(z_function_values.size(), 0);

  for (int i = 1; i < z_function_values.size(); ++i) {
    int j = z_function_values[i] - 1;
    while (j >= 0 && pi_function_values[i + j] <= 0) {
      pi_function_values[i + j] = j + 1;
      --j;
    }
  }

  return pi_function_values;
}

std::vector<int> ConvertPiToZ(std::vector<int>& pi_function_values) {
  std::vector<int> z_function_values(pi_function_values.size());
  for (int i = 1; i < pi_function_values.size(); ++i) {
    if (pi_function_values[i] > 0) {
      z_function_values[i - pi_function_values[i] + 1] = pi_function_values[i];
    }
  }
  z_function_values[0] = pi_function_values.size();
  int i = 1;
  while(i < pi_function_values.size()) {
    int new_index = i;
    if (z_function_values[i] > 0) {
      for (int j = 1; j < z_function_values[i]; ++j) {
        if (z_function_values[i + j] > z_function_values[j]) {
          break;
        }
        z_function_values[i + j] = std::min(z_function_values[j], z_function_values[i] - j);
        new_index = i + j;
      }
    }
    i = new_index + 1;
  }

  return z_function_values;
}

class ConvertFromPiToStr {
 public:
  std::string operator()(std::vector<int>& pi_function_values) {
    pi_function_values_ = std::move(pi_function_values);
    Convert();
    return string_;
  }

  void operator()(std::istream& input_stream, std::ostream& output_stream) {
    Read(input_stream);
    Convert();
    output_stream << string_;
  }

 private:
  std::vector<int> pi_function_values_;
  std::string string_;

  void Read(std::istream& input_stream) {
    int pi_value = 0;
    while (input_stream >> pi_value) {
      pi_function_values_.push_back(pi_value);
    }
  }

  void Convert() {
    std::set<char> used_letters;
    const char start_symbol = 'a';
    used_letters.insert(start_symbol);
    string_.push_back(start_symbol);
    size_t shift = 1;
    for (size_t i = 1; i < pi_function_values_.size(); ++i) {
      if (pi_function_values_[i] == pi_function_values_[i - 1] + 1) {
        string_.push_back(string_[pi_function_values_[i - 1]]);
      } else if (pi_function_values_[i] != 0) {
        string_.push_back(string_[pi_function_values_[i] - 1]);
      } else {
        std::set<char> buffer;
        int border = pi_function_values_[i - 1];
        buffer.insert(string_[border]);
        while (border > 0 && string_[i] != string_[border]) {
          border = pi_function_values_[border - 1];
          buffer.insert(string_[border]);
        }
        if (buffer == used_letters) {
          string_.push_back(start_symbol + shift);
          used_letters.insert(start_symbol + shift);
          ++shift;
        } else {
          for (size_t j = 0; j < 26; ++j) {
            if (buffer.find(start_symbol + j) == buffer.end()) {
              string_.push_back(start_symbol + j);
              break;
            }
          }
        }
      }
    }
  }
};

class ConvertFromZToStr {
 public:
  std::string operator()(std::vector<int>& z_function_values) {
    z_function_values_ = std::move(z_function_values);
    Convert();
    return string_;
  }

  void operator()(std::istream& input_stream, std::ostream& output_stream) {
    Read(input_stream);
    Convert();
    output_stream << string_ << std::endl;
  }

 private:
  std::vector<int> z_function_values_;
  std::string string_;
  const int ALPHABET_SIZE_ = 26;

  void Read(std::istream& input_stream) {
    int z_value = 0;
    while (input_stream >> z_value) {
      z_function_values_.push_back(z_value);
    }
  }

  void Convert() {
    const char start_symbol = 'a';
    std::set<int> index_buffer;
    bool is_last_symbol_in_z_block = true;
    string_.push_back(start_symbol);
    int i = 1;
    while (i < z_function_values_.size()) {
      if (z_function_values_[i] != 0) {
        index_buffer.clear();
        int prefix_length = z_function_values_[i];
        int prefix_index = 0;
        while (prefix_length > 0) {
          if (z_function_values_[i] > prefix_length) {
            prefix_length = z_function_values_[i];
            prefix_index = 0;
          }
          if (z_function_values_[i] >= prefix_length) {
            index_buffer.insert(z_function_values_[i]);
          }
          string_.push_back(string_[prefix_index]);
          ++prefix_index;
          ++i;
          --prefix_length;
        }
        is_last_symbol_in_z_block = true;
      } else {
        if (is_last_symbol_in_z_block) {
          std::set<char> used_symbols;
          for(auto&& index : index_buffer) {
            used_symbols.insert(string_[index]);
          }
          for (size_t shift = 1; shift < ALPHABET_SIZE_; ++shift) {
            if (used_symbols.find(start_symbol + shift) == used_symbols.end()) {
              string_.push_back(start_symbol + shift);
              break;
            }
          }
          is_last_symbol_in_z_block = false;
        } else {
          string_.push_back(start_symbol + 1);
        }
        ++i;
      }
    }
  }
};

void TestAll() {
  std::string test_str("aaaab");
  std::vector<int> pi_test = {0, 1, 2, 3, 0};
  std::vector<int> z_test = {5, 3, 2, 1, 0};
  assert(PiFunction(test_str) == pi_test);
  assert(ZFunction(test_str) == z_test);
  assert(ConvertPiToZ(pi_test) == z_test);
  assert(ConvertZToPi(z_test) == pi_test);
  ConvertFromZToStr from_z_to_str;
  assert(from_z_to_str(z_test) == test_str);
  ConvertFromPiToStr from_pi_to_str;
  assert(from_pi_to_str(pi_test) == test_str);
}

int main() {
  TestAll();

  return 0;
}
