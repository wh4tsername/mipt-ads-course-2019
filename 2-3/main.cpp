#include <algorithm>
#include <iostream>
#include <map>
#include <stack>
#include <vector>

class SuffixTree {
 public:
  SuffixTree(const std::string& string, int first_part_size);

  std::string FindKthStatistic(long long k);

 private:
  struct Vertex {
    Vertex() = default;

    Vertex(int left, int right, int parent)
        : left_(left), right_(right), parent_(parent), suf_link_(0) {}

    int left_ = -1;
    int right_ = -1;
    int parent_ = -1;
    int suf_link_ = 0;
    std::map<char, int> next_vertices_;
    bool has_first_terminal = false;
    bool has_second_terminal = false;
  };

  void MarkSubstringVertices();
  void UpdateTerminalInfo(int current_vertex);
  void FillResultString(int current_vertex);

  void InsertLetter(char letter);

  std::vector<Vertex> tree_;
  int position_of_new_vertex_ = 2;
  std::string string_;
  int first_part_size_;
  int position_in_string_ = 0;
  int vertex_of_current_suffix = 0;
  int position_on_edge = 0;
  std::vector<bool> visited_;

  std::vector<int> vertex_numbers;
  int vertex_number = 1;

  long long k_ = 0;
  long long counter_ = 0;
  std::string kth_statistic_;
};

SuffixTree::SuffixTree(const std::string& string, int first_part_size) {
  string_ = string;
  first_part_size_ = first_part_size;

  tree_.resize(2 * string_.size());
  tree_[0] = Vertex();

  for (char symbol = 'a'; symbol <= 'z'; ++symbol) {
    tree_[1].next_vertices_[symbol] = 0;
  }
  tree_[1].next_vertices_['#'] = 0;
  tree_[1].next_vertices_['$'] = 0;
  tree_[0].suf_link_ = 1;

  for (size_t i = 2; i < tree_.size(); ++i) {
    tree_[i].right_ = static_cast<int>(string_.size()) - 1;
  }

  vertex_of_current_suffix = 0;
  position_on_edge = 0;

  for (auto&& symbol : string_) {
    InsertLetter(symbol);
    ++position_in_string_;
  }
}

void SuffixTree::InsertLetter(char letter) {
  while (true) {
    if (tree_[vertex_of_current_suffix].right_ < position_on_edge) {
      if (tree_[vertex_of_current_suffix].next_vertices_.find(letter) ==
          tree_[vertex_of_current_suffix].next_vertices_.end()) {
        tree_[vertex_of_current_suffix].next_vertices_[letter] =
            position_of_new_vertex_;
        tree_[position_of_new_vertex_].left_ = position_in_string_;
        tree_[position_of_new_vertex_].parent_ = vertex_of_current_suffix;
        ++position_of_new_vertex_;

        vertex_of_current_suffix = tree_[vertex_of_current_suffix].suf_link_;
        position_on_edge = tree_[vertex_of_current_suffix].right_ + 1;

        continue;
      }
      vertex_of_current_suffix =
          tree_[vertex_of_current_suffix].next_vertices_[letter];
      position_on_edge = tree_[vertex_of_current_suffix].left_;
    }

    if (position_on_edge == -1 || letter == string_[position_on_edge]) {
      ++position_on_edge;
      break;
    }

    tree_[position_of_new_vertex_] =
        Vertex(tree_[vertex_of_current_suffix].left_, position_on_edge - 1,
               tree_[vertex_of_current_suffix].parent_);
    tree_[position_of_new_vertex_].next_vertices_[string_[position_on_edge]] =
        vertex_of_current_suffix;
    tree_[position_of_new_vertex_].next_vertices_[letter] =
        position_of_new_vertex_ + 1;
    tree_[position_of_new_vertex_ + 1].left_ = position_in_string_;
    tree_[position_of_new_vertex_ + 1].parent_ = position_of_new_vertex_;

    tree_[vertex_of_current_suffix].left_ = position_on_edge;
    tree_[vertex_of_current_suffix].parent_ = position_of_new_vertex_;
    tree_[tree_[position_of_new_vertex_].parent_]
        .next_vertices_[string_[tree_[position_of_new_vertex_].left_]] =
        position_of_new_vertex_;

    position_of_new_vertex_ += 2;

    vertex_of_current_suffix =
        tree_[tree_[position_of_new_vertex_ - 2].parent_].suf_link_;
    position_on_edge = tree_[position_of_new_vertex_ - 2].left_;

    while (position_on_edge <= tree_[position_of_new_vertex_ - 2].right_) {
      vertex_of_current_suffix = tree_[vertex_of_current_suffix]
                                     .next_vertices_[string_[position_on_edge]];
      position_on_edge += tree_[vertex_of_current_suffix].right_ -
                          tree_[vertex_of_current_suffix].left_ + 1;
    }

    if (position_on_edge == tree_[position_of_new_vertex_ - 2].right_ + 1) {
      tree_[position_of_new_vertex_ - 2].suf_link_ = vertex_of_current_suffix;
    } else {
      tree_[position_of_new_vertex_ - 2].suf_link_ = position_of_new_vertex_;
    }

    position_on_edge =
        tree_[vertex_of_current_suffix].right_ -
        (position_on_edge - tree_[position_of_new_vertex_ - 2].right_) + 2;
  }
}

void SuffixTree::MarkSubstringVertices() {
  visited_.resize(position_of_new_vertex_);
  vertex_numbers.resize(position_of_new_vertex_);
  UpdateTerminalInfo(0);
}

void SuffixTree::UpdateTerminalInfo(int current_vertex) {
  visited_[current_vertex] = true;
  if (current_vertex != 0) {
    vertex_numbers[current_vertex] = vertex_number;
    ++vertex_number;

    int string_index = (tree_[current_vertex].left_ < first_part_size_) ? 0 : 1;

    int rg = (!string_index && tree_[current_vertex].right_ >= first_part_size_)
                 ? first_part_size_
                 : tree_[current_vertex].right_ + 1 -
                       first_part_size_ * string_index;

    if (rg == first_part_size_ && string_index == 0) {
      tree_[current_vertex].has_first_terminal = true;
      int ind = tree_[current_vertex].parent_;
      while (ind != 0 && !tree_[ind].has_first_terminal) {
        tree_[ind].has_first_terminal = true;
        ind = tree_[ind].parent_;
      }
    }
    if (rg == string_.size() - first_part_size_ && string_index == 1) {
      tree_[current_vertex].has_second_terminal = true;
      int ind = tree_[current_vertex].parent_;
      while (ind != 0 && !tree_[ind].has_second_terminal) {
        tree_[ind].has_second_terminal = true;
        ind = tree_[ind].parent_;
      }
    }
  }

  for (auto&& pair : tree_[current_vertex].next_vertices_) {
    if (!visited_[pair.second]) {
      UpdateTerminalInfo(pair.second);
    }
  }
}

std::string SuffixTree::FindKthStatistic(long long k) {
  MarkSubstringVertices();

  visited_.assign(position_of_new_vertex_, false);
  vertex_numbers.assign(position_of_new_vertex_, 0);
  k_ = k;
  counter_ = 0;
  kth_statistic_.clear();

  FillResultString(0);

  std::reverse(kth_statistic_.begin(), kth_statistic_.end());

  if (kth_statistic_.empty()) {
    return "-1";
  }

  return kth_statistic_;
}

void SuffixTree::FillResultString(int current_vertex) {
  visited_[current_vertex] = true;
  if (current_vertex != 0 && tree_[current_vertex].has_first_terminal &&
      tree_[current_vertex].has_second_terminal && counter_ < k_) {
    int lf = tree_[current_vertex].left_;
    int rg = tree_[current_vertex].right_;
    if (counter_ + rg - lf + 1 < k_) {
      counter_ += rg - lf + 1;
    } else {
      lf = tree_[current_vertex].left_;
      rg = lf + k_ - counter_ - 1;

      std::stack<char> buffer;

      for (int i = lf; i <= rg; ++i) {
        buffer.push(string_[i]);
      }

      while (!buffer.empty()) {
        kth_statistic_.push_back(buffer.top());
        buffer.pop();
      }

      int ind = tree_[current_vertex].parent_;
      while (ind != -1 && tree_[ind].has_second_terminal &&
             tree_[ind].has_first_terminal) {
        lf = tree_[ind].left_;
        rg = tree_[ind].right_;

        for (int i = lf; i <= rg; ++i) {
          buffer.push(string_[i]);
        }

        while (!buffer.empty()) {
          kth_statistic_.push_back(buffer.top());
          buffer.pop();
        }

        ind = tree_[ind].parent_;
      }

      counter_ = k_;
    }
  }

  for (auto&& pair : tree_[current_vertex].next_vertices_) {
    if (!visited_[pair.second]) {
      FillResultString(pair.second);
    }
  }
}

int main() {
  std::string first_string;
  std::string second_string;

  std::cin >> first_string >> second_string;

  std::string input_string = first_string + '$' + second_string + '#';

  SuffixTree tree(input_string, first_string.size() + 1);

  long long k = 0;
  std::cin >> k;

  std::string kth_statistic_ = tree.FindKthStatistic(k);

  for (auto&& symbol : kth_statistic_) {
    printf("%c", symbol);
  }

  return 0;
}
