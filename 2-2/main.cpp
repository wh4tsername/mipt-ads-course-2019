#include <iostream>
#include <map>
#include <vector>

class SuffixTree {
 public:
  SuffixTree(const std::string& string, int first_part_size);

  void Print();

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
  };

  void DFS(int current_vertex);

  void InsertLetter(char letter);

  std::vector<Vertex> tree_;
  int last_added_vertex_index_ = 2;
  std::string string_;
  int first_part_size_;
  int position_in_string_ = 0;
  std::pair<int, int> current_state_;
  std::vector<bool> visited_;
  std::vector<int> vertex_numbers;
  int vertex_number = 1;
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

  current_state_ = {0, 0};  // suff_vertex and pos_on_edge

  for (auto&& symbol : string_) {
    InsertLetter(symbol);
    ++position_in_string_;
  }
}

void SuffixTree::InsertLetter(char letter) {
  while (true) {
    if (tree_[current_state_.first].right_ < current_state_.second) {
      if (tree_[current_state_.first].next_vertices_.find(letter) ==
          tree_[current_state_.first].next_vertices_.end()) {
        tree_[current_state_.first].next_vertices_[letter] =
            last_added_vertex_index_;
        tree_[last_added_vertex_index_].left_ = position_in_string_;
        tree_[last_added_vertex_index_].parent_ = current_state_.first;
        ++last_added_vertex_index_;

        current_state_.first = tree_[current_state_.first].suf_link_;
        current_state_.second = tree_[current_state_.first].right_ + 1;

        continue;
      }
      current_state_.first = tree_[current_state_.first].next_vertices_[letter];
      current_state_.second = tree_[current_state_.first].left_;
    }

    if (current_state_.second == -1 ||
        letter == string_[current_state_.second]) {
      ++current_state_.second;
      break;
    }

    tree_[last_added_vertex_index_] =
        Vertex(tree_[current_state_.first].left_, current_state_.second - 1,
               tree_[current_state_.first].parent_);
    tree_[last_added_vertex_index_]
        .next_vertices_[string_[current_state_.second]] = current_state_.first;
    tree_[last_added_vertex_index_].next_vertices_[letter] =
        last_added_vertex_index_ + 1;
    tree_[last_added_vertex_index_ + 1].left_ = position_in_string_;
    tree_[last_added_vertex_index_ + 1].parent_ = last_added_vertex_index_;

    tree_[current_state_.first].left_ = current_state_.second;
    tree_[current_state_.first].parent_ = last_added_vertex_index_;
    tree_[tree_[last_added_vertex_index_].parent_]
        .next_vertices_[string_[tree_[last_added_vertex_index_].left_]] =
        last_added_vertex_index_;

    last_added_vertex_index_ += 2;

    current_state_.first =
        tree_[tree_[last_added_vertex_index_ - 2].parent_].suf_link_;
    current_state_.second = tree_[last_added_vertex_index_ - 2].left_;

    while (current_state_.second <=
           tree_[last_added_vertex_index_ - 2].right_) {
      current_state_.first =
          tree_[current_state_.first]
              .next_vertices_[string_[current_state_.second]];
      current_state_.second += tree_[current_state_.first].right_ -
                               tree_[current_state_.first].left_ + 1;
    }

    if (current_state_.second ==
        tree_[last_added_vertex_index_ - 2].right_ + 1) {
      tree_[last_added_vertex_index_ - 2].suf_link_ = current_state_.first;
    } else {
      tree_[last_added_vertex_index_ - 2].suf_link_ = last_added_vertex_index_;
    }

    current_state_.second =
        tree_[current_state_.first].right_ -
        (current_state_.second - tree_[last_added_vertex_index_ - 2].right_) +
        2;
  }
}

void SuffixTree::Print() {
  printf("%d\n", last_added_vertex_index_ - 1);
  visited_.resize(last_added_vertex_index_);
  vertex_numbers.resize(last_added_vertex_index_);
  DFS(0);
}

void SuffixTree::DFS(int current_vertex) {
  visited_[current_vertex] = true;
  if (current_vertex != 0) {
    vertex_numbers[current_vertex] = vertex_number;
    ++vertex_number;
    printf("%d ", vertex_numbers[tree_[current_vertex].parent_]);

    int string_index = (tree_[current_vertex].left_ < first_part_size_) ? 0 : 1;
    printf("%d ", string_index);
    printf("%d ",
           tree_[current_vertex].left_ - first_part_size_ * string_index);
    printf("%d\n",
           (!string_index && tree_[current_vertex].right_ >= first_part_size_)
               ? first_part_size_
               : tree_[current_vertex].right_ + 1 -
                     first_part_size_ * string_index);
  }

  for (auto&& pair : tree_[current_vertex].next_vertices_) {
    if (!visited_[pair.second]) {
      DFS(pair.second);
    }
  }
}

int main() {
  std::string first_string;
  std::string second_string;

  std::cin >> first_string >> second_string;

  second_string = first_string + second_string;

  SuffixTree tree(second_string, first_string.size());

  tree.Print();

  return 0;
}
