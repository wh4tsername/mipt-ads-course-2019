#include <iostream>
#include <string>
#include <vector>

class Trie {
 public:
  explicit Trie(std::vector<std::pair<int, int>>& indexes_of_substrings,
                const std::string& pattern);

  std::vector<int> FindMatches(
      const std::string& text,
      std::vector<std::pair<int, int>>& indexes_of_substrings);

 private:
  static const int ALPHABET_POWER_ = 26;
  static const char ROOT_SYMBOL_ = ' ';

  struct TrieNode {
    TrieNode(int parent, char parent_letter)
        : next_nodes_(ALPHABET_POWER_, -1), go_(ALPHABET_POWER_, -1) {
      parent_ = parent;
      parent_letter_ = parent_letter;
    }

    int parent_;
    char parent_letter_;
    bool is_terminal_ = false;
    std::vector<int> next_nodes_;
    std::vector<int> go_;
    std::vector<int> indexes_of_pattern_;
    int suff_link_ = -1;
    int compressed_suff_link = -1;
  };

  std::vector<TrieNode> trie_;
  std::vector<int> terminal_nodes_;
  const int pattern_size_;

  int GetSuffLink(TrieNode& node);

  int GetCompressedSuffLink(TrieNode& node);

  int GoTo(TrieNode& node, char letter);

  void AddPattern(const std::string& pattern, size_t pattern_index);
};

Trie::Trie(std::vector<std::pair<int, int>>& indexes_of_substrings,
           const std::string& pattern)
    : trie_(1, TrieNode(0, ROOT_SYMBOL_)), pattern_size_(pattern.size()) {
  size_t pattern_index = 0;
  for (auto&& index_pair : indexes_of_substrings) {
    std::string buffer;
    for (int i = index_pair.first; i <= index_pair.second; ++i) {
      buffer.push_back(pattern[i]);
    }
    AddPattern(buffer, pattern_index);
    ++pattern_index;
  }
}

std::vector<int> Trie::FindMatches(
    const std::string& text,
    std::vector<std::pair<int, int>>& indexes_of_substrings) {
  std::vector<int> numbers_of_matches(text.size());

  int condition = 0;
  for (size_t i = 0; i < text.size(); ++i) {
    condition = GoTo(trie_[condition], text[i]);
    int possible_terminal_condition = condition;
    do {
      if (trie_[possible_terminal_condition].is_terminal_) {
        for (auto&& pattern_index :
             trie_[possible_terminal_condition].indexes_of_pattern_) {
          int start_position = static_cast<int>(i) -
                               indexes_of_substrings[pattern_index].second +
                               indexes_of_substrings[pattern_index].first;
          if ((start_position - indexes_of_substrings[pattern_index].first >=
               0) &&
              (start_position - indexes_of_substrings[pattern_index].first +
                   pattern_size_ - 1 <
               text.size())) {
            numbers_of_matches[start_position -
                               indexes_of_substrings[pattern_index].first] += 1;
          }
        }
      }
      possible_terminal_condition =
          GetCompressedSuffLink(trie_[possible_terminal_condition]);
    } while (possible_terminal_condition != 0);
  }

  std::vector<int> answer;
  for (size_t i = 0; i < numbers_of_matches.size(); ++i) {
    if (numbers_of_matches[i] == indexes_of_substrings.size()) {
      answer.emplace_back(i);
    }
  }
  return answer;
}

int Trie::GetSuffLink(TrieNode& node) {
  if (node.suff_link_ == -1) {
    if (node.parent_ == 0 || node.parent_letter_ == ROOT_SYMBOL_) {
      node.suff_link_ = 0;
    } else {
      node.suff_link_ =
          GoTo(trie_[GetSuffLink(trie_[node.parent_])], node.parent_letter_);
    }
  }
  return node.suff_link_;
}

int Trie::GetCompressedSuffLink(TrieNode& node) {
  if (node.compressed_suff_link != -1) {
    return node.compressed_suff_link;
  }
  if (trie_[GetSuffLink(node)].is_terminal_ ||
      trie_[GetSuffLink(node)].parent_letter_ == ROOT_SYMBOL_) {
    node.compressed_suff_link = GetSuffLink(node);
    return node.compressed_suff_link;
  }
  node.compressed_suff_link = GetCompressedSuffLink(trie_[GetSuffLink(node)]);
  return node.compressed_suff_link;
}

int Trie::GoTo(TrieNode& node, char letter) {
  if (node.go_[letter - 'a'] != -1) {
    return node.go_[letter - 'a'];
  }
  for (auto&& next_node : node.next_nodes_) {
    if (trie_[next_node].parent_letter_ == letter) {
      node.go_[letter - 'a'] = next_node;
      return next_node;
    }
  }
  if (node.parent_letter_ == ROOT_SYMBOL_) {
    node.go_[letter - 'a'] = 0;
    return 0;
  }
  node.go_[letter - 'a'] = GoTo(trie_[GetSuffLink(node)], letter);
  return node.go_[letter - 'a'];
}

void Trie::AddPattern(const std::string& pattern, size_t pattern_index) {
  size_t index = 0;
  for (auto&& letter : pattern) {
    if (trie_[index].next_nodes_[letter - 'a'] == -1) {
      trie_.emplace_back(TrieNode(index, letter));
      trie_[index].next_nodes_[letter - 'a'] = trie_.size() - 1;
      index = trie_.size() - 1;
    } else {
      index = trie_[index].next_nodes_[letter - 'a'];
    }
  }
  trie_[index].is_terminal_ = true;
  terminal_nodes_.emplace_back(index);
  trie_[index].indexes_of_pattern_.emplace_back(pattern_index);
}

class Solver {
 public:
  void operator()();

 private:
  std::string pattern_;
  std::string text_;
  std::vector<std::pair<int, int>> indexes_of_substrings_;

  void Read();

  void Solve();
};

void Solver::operator()() {
  Read();

  Solve();
}

void Solver::Read() { std::cin >> pattern_ >> text_; }

void Solver::Solve() {
  std::vector<std::string> dictionary;
  std::string buffer;

  std::pair<int, int> current_substring_position;
  if (pattern_[0] != '?') {
    current_substring_position.first = 0;
  }
  if (pattern_[1] == '?' && pattern_[0] != '?') {
    current_substring_position.second = 0;
    indexes_of_substrings_.emplace_back(current_substring_position);
  }

  for (size_t i = 1; i < pattern_.size() - 1; ++i) {
    if (pattern_[i - 1] == '?' && pattern_[i] != '?') {
      current_substring_position.first = i;
    }
    if (pattern_[i + 1] == '?' && pattern_[i] != '?') {
      current_substring_position.second = i;
      indexes_of_substrings_.emplace_back(current_substring_position);
    }
  }

  if (pattern_[pattern_.size() - 2] == '?' &&
      pattern_[pattern_.size() - 1] != '?') {
    current_substring_position.first = pattern_.size() - 1;
  }
  if (pattern_[pattern_.size() - 1] != '?') {
    current_substring_position.second = pattern_.size() - 1;
    indexes_of_substrings_.emplace_back(current_substring_position);
  }

  Trie trie(indexes_of_substrings_, pattern_);
  std::vector<int> answer = trie.FindMatches(text_, indexes_of_substrings_);
  for (auto&& index : answer) {
    std::cout << index << " ";
  }
}

int main() {
  Solver solver;

  solver();

  return 0;
}
