#include <cassert>
#include <iostream>
#include <vector>
#include <set>
#include <string>

std::vector<int> PiFunction(std::string& string) {
  std::vector<int> pi(string.size());
  pi[0] = 0;
  for (size_t i = 1; i < string.size(); ++i) {
    int border = pi[i - 1];
    while (border > 0 && string[i] != string[border]) {
      border = pi[border - 1];
    }
    if (string[i] == string[border]) {
      ++border;
    }
    pi[i] = border;
  }
  return pi;
}

std::vector<int> ZFunction(std::string& string) {
  std::vector<int> z(string.size());
  int left = 0;
  int right = 0;
  z[0] = 0;
  for (int i = 1; i < string.size(); ++i) {
    z[i] = std::max(0, std::min(right - i, z[i - left]));
    while (i + z[i] < string.size() && string[z[i]] == string[i + z[i]]) {
      ++z[i];
    }
    if (i + z[i] > right) {
      left = i;
      right = i + z[i];
    }
  }
  z[0] = string.size();
  return z;
}

std::vector<int> ZToPi(std::vector<int>& z) {
  std::vector<int> pi(z.size(), 0);

  for (int i = 1; i < z.size(); ++i) {
    for (int j = z[i] - 1; j >= 0; --j) {
      if (pi[i + j] > 0) {
        break;
      } else {
        pi[i + j] = j + 1;
      }
    }
  }

  return pi;
}

std::vector<int> PiToZ(std::vector<int>& pi) {
  std::vector<int> z(pi.size());
  for (int i = 1; i < pi.size(); ++i) {
    if (pi[i] > 0) {
      z[i - pi[i] + 1] = pi[i];
    }
  }
  z[0] = pi.size();
  int i = 1;
  while(i < pi.size()) {
    int t = i;
    if (z[i] > 0) {
      for (int j = 1; j < z[i]; ++j) {
        if (z[i + j] > z[j]) {
          break;
        }
        z[i + j] = std::min(z[j], z[i] - j);
        t = i + j;
      }
    }
    i = t + 1;
  }

  return z;
}

class RecoverFromPiToStr {
 public:
  std::string operator()(std::vector<int> pi) {
    pi_ = std::move(pi);
    Recover();
    return string_;
  }

  void operator()() {
    Read();
    Recover();
    std::cout << string_;
  }

 private:
  std::vector<int> pi_;
  std::string string_;

  void Read() {
    int pi_value = 0;
    pi_ = std::vector<int>();
    while (std::cin.peek() != EOF && std::cin.peek() != '\n') {
      std::cin >> pi_value;
      pi_.push_back(pi_value);
    }
  }

  void Preprocess(std::string& string) {
    pi_ = std::vector<int>(string.size());
    pi_[0] = 0;
    for (size_t i = 1; i < string.size(); ++i) {
      int border = pi_[i - 1];
      while (border > 0 && string[i] != string[border]) {
        border = pi_[border - 1];
      }
      if (string[i] == string[border]) {
        ++border;
      }
      pi_[i] = border;
    }
  }
  void Recover() {
    std::set<char> letters;
    const char start_symbol = 'a';
    letters.insert(start_symbol);
    string_.push_back(start_symbol);
    size_t shift = 1;
    for (size_t i = 1; i < pi_.size(); ++i) {
      if (pi_[i] == pi_[i - 1] + 1) {
        string_.push_back(string_[pi_[i - 1]]);
      } else if (pi_[i] != 0) {
        string_.push_back(string_[pi_[i] - 1]);
      } else {
        std::set<char> buffer;
        int border = pi_[i - 1];
        buffer.insert(string_[border]);
        while (border > 0 && string_[i] != string_[border]) {
          border = pi_[border - 1];
          buffer.insert(string_[border]);
        }
        if (buffer == letters) {
          string_.push_back(start_symbol + shift);
          letters.insert(start_symbol + shift);
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

class RecoverFromZToStr {
 public:
  std::string operator()(std::vector<int> z) {
    z_ = std::move(z);
    Recover();
    return string_;
  }

  void operator()() {
    Read();
    Recover();
    std::cout << string_ << std::endl;
  }

 private:
  std::vector<int> z_;
  std::string string_;

  void Read() {
    int z_value = 0;
    while (std::cin >> z_value) {
      z_.push_back(z_value);
    }
  }

  void Recover() {
    const char start_symbol = 'a';
    std::set<int> index_buffer;
    bool is_last_symbol_in_z_block = true;
    string_.push_back(start_symbol);
    int i = 1;
    while (i < z_.size()) {
      if (z_[i] != 0) {
        index_buffer.clear();
        int prefix_length = z_[i];
        int prefix_index = 0;
        while (prefix_length > 0) {
          if (z_[i] > prefix_length) {
            prefix_length = z_[i];
            prefix_index = 0;
          }
          if (z_[i] >= prefix_length) {
            index_buffer.insert(z_[i]);
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
          for (size_t shift = 1; shift < 26; ++shift) {
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
  std::vector<int> pi_test({0, 1, 2, 3, 0});
  std::vector<int> z_test({5, 3, 2, 1, 0});
  RecoverFromZToStr from_z_to_str;
  assert(from_z_to_str(z_test) == test_str);
  RecoverFromPiToStr from_pi_to_str;
  assert(from_pi_to_str(pi_test) == test_str);
  assert(PiFunction(test_str) == pi_test);
  assert(ZFunction(test_str) == z_test);
  assert(PiToZ(pi_test) == z_test);
  assert(ZToPi(z_test) == pi_test);
}

int main() {
  TestAll();

  return 0;
}
