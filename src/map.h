#ifndef TS_STL_MAP_H_
#define TS_STL_MAP_H_

#include "src/utils.h"
#include <cstddef>
#include <utility>

namespace ts_stl {

template <typename K, typename V, typename Compare = std::less<K>> class Map {
public:
  using key_type = K;
  using value_type = V;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using key_compare = Compare;
  using reference = value_type &;
  using const_reference = const value_type &;

private:
  class Node {
  public:
    Node *parent_ = nullptr;

    Node *left_child_ = nullptr;

    Node *right_child_ = nullptr;

    size_type size_ = 1;

    size_type random_value_ = Random();

    key_type key_;

    value_type value_;

    Node(const key_type &key, const value_type &value)
        : key_(key), value_(value) {}

    Node(const key_type &key, value_type &&value)
        : key_(key), value_(std::move(value)) {}

    ~Node() {
      delete left_child_;
      delete right_child_;
    }

    void PushUp() {
      size_ = 1;
      if (left_child_) {
        size_ += left_child_->size_;
        left_child_->parent_ = this;
      }
      if (right_child_) {
        size_ += right_child_->size_;
        right_child_->parent_ = this;
      }
    }

    static void SplitL(Node *node, const key_type &key, Node *&left_tree,
                       Node *&right_tree) {
      if (!node) {
        left_tree = nullptr;
        right_tree = nullptr;
        return;
      }
      if (Compare()(node->key_, key)) {
        left_tree = node;
        SplitL(node->right_child_, key, left_tree->right_child_, right_tree);
      } else {
        right_tree = node;
        SplitL(node->left_child_, key, left_tree, right_tree->left_child_);
      }
      node->PushUp();
    }

    static void SplitLE(Node *node, const key_type &key, Node *&left_tree,
                        Node *&right_tree) {
      if (!node) {
        left_tree = nullptr;
        right_tree = nullptr;
        return;
      }
      if (Compare()(key, node->key_)) {
        right_tree = node;
        SplitLE(node->left_child_, key, left_tree, right_tree->left_child_);
      } else {
        left_tree = node;
        SplitLE(node->right_child_, key, left_tree->right_child_, right_tree);
      }
      node->PushUp();
    }

    static auto Merge(Node *left_tree, Node *right_tree) -> Node * {
      if (!left_tree) {
        return right_tree;
      }
      if (!right_tree) {
        return left_tree;
      }
      if (left_tree->random_value_ < right_tree->random_value_) {
        left_tree->right_child_ = Merge(left_tree->right_child_, right_tree);
        left_tree->PushUp();
        return left_tree;
      }
      right_tree->left_child_ = Merge(left_tree, right_tree->left_child_);
      right_tree->PushUp();
      return right_tree;
    }

    static auto Clone(Node *node) -> Node * {
      if (!node) {
        return nullptr;
      }
      Node *new_node = new Node(node->key_, node->value_);
      new_node->left_child_ = Clone(node->left_child_);
      new_node->right_child_ = Clone(node->right_child_);
      new_node->PushUp();
      return new_node;
    }

    static void Debug(Node *node) {
      if (!node) {
        std::cerr << "nullptr" << std::endl;
        return;
      }
      std::cerr << "(" << node->key_ << ", " << node->value_
                << ", size = " << node->size_ << ")," << std::endl;
      if (node->left_child_) {
        std::cerr << "left_child: {" << std::endl;
        Debug(node->left_child_);
        std::cerr << "}," << std::endl;
      }
      if (node->right_child_) {
        std::cerr << "right_child: {" << std::endl;
        Debug(node->right_child_);
        std::cerr << "}," << std::endl;
      }
    }
  };

  Node *root_ = nullptr;

public:
  class iterator {
  private:
    Map *map_;

    Node *position_;

    auto Previous() -> Node * {
      if (!position_) {
        return map_->back().position_;
      }
      if (position_->left_child_) {
        Node *p = position_->left_child_;
        while (p->right_child_) {
          p = p->right_child_;
        }
        return p;
      }
      Node *p = position_;
      while (p->parent_ && p->parent_->left_child_ == p) {
        p = p->parent_;
      }
      return p->parent_;
    }

    auto Next() -> Node * {
      if (!position_) {
        return map_->begin().position_;
      }
      if (position_->right_child_) {
        Node *p = position_->right_child_;
        while (p->left_child_) {
          p = p->left_child_;
        }
        return p;
      }
      Node *p = position_;
      while (p->parent_ && p->parent_->right_child_ == p) {
        p = p->parent_;
      }
      return p->parent_;
    }

  public:
    iterator() = delete;

    iterator(Map *map, Node *position) : map_(map), position_(position) {}

    iterator(const iterator &) = default;

    iterator(iterator &&) = default;

    ~iterator() = default;

    auto operator=(const iterator &) -> iterator & = default;

    auto operator=(iterator &&) -> iterator & = default;

    auto operator++() -> iterator & {
      position_ = Next();
      return *this;
    }

    auto operator++(int) -> iterator {
      iterator tmp = *this;
      position_ = Next();
      return tmp;
    }

    auto operator--() -> iterator & {
      position_ = Previous();
      return *this;
    }

    auto operator--(int) -> iterator {
      iterator tmp = *this;
      position_ = Previous();
      return tmp;
    }

    auto operator*() -> std::pair<key_type &, value_type &> {
      Assert(position_, "Map::iterator::opreator*(): Invalid iterator!");
      return {position_->key_, position_->value_};
    }

    auto operator==(const iterator &other) const -> bool {
      return map_ == other.map_ && position_ == other.position_;
    }

    auto operator!=(const iterator &other) const -> bool {
      return map_ != other.map_ || position_ != other.position_;
    }
  };

  class const_iterator {
  private:
    const Map *map_;

    const Node *position_;

    auto Previous() -> Node * {
      if (!position_) {
        return map_->back().position_;
      }
      if (position_->left_child_) {
        Node *p = position_->left_child_;
        while (p->right_child_) {
          p = p->right_child_;
        }
        return p;
      }
      Node *p = position_;
      while (p->parent_ && p->parent_->left_child_ == p) {
        p = p->parent_;
      }
      return p->parent_;
    }

    auto Next() -> Node * {
      if (!position_) {
        return map_->begin().position_;
      }
      if (position_->right_child_) {
        Node *p = position_->right_child_;
        while (p->left_child_) {
          p = p->left_child_;
        }
        return p;
      }
      Node *p = position_;
      while (p->parent_ && p->parent_->right_child_ == p) {
        p = p->parent_;
      }
      return p->parent_;
    }

  public:
    const_iterator() = delete;

    const_iterator(Map *map, Node *position) : map_(map), position_(position) {}

    const_iterator(const const_iterator &) = default;

    const_iterator(const_iterator &&) = default;

    ~const_iterator() = default;

    auto operator=(const const_iterator &) -> const_iterator & = default;

    auto operator=(const_iterator &&) -> const_iterator & = default;

    auto operator++() -> const_iterator & {
      position_ = Next();
      return *this;
    }

    auto operator++(int) -> const_iterator {
      const_iterator tmp = *this;
      position_ = Next();
      return tmp;
    }

    auto operator--() -> const_iterator & {
      position_ = Previous();
      return *this;
    }

    auto operator--(int) -> const_iterator {
      const_iterator tmp = *this;
      position_ = Previous();
      return tmp;
    }

    auto operator*() -> std::pair<const key_type &, const value_type &> {
      Assert(position_, "Map::iterator::opreator*(): Invalid iterator!");
      return {position_->key_, position_->value_};
    }

    auto operator==(const const_iterator &other) const -> bool {
      return map_ == other.map_ && position_ == other.position_;
    }

    auto operator!=(const const_iterator &other) const -> bool {
      return map_ != other.map_ || position_ != other.position_;
    }
  };

private:
  // Todo
  // iterator begin_, back_, end_;

public:
  Map() = default;

  ~Map() { delete root_; }

  Map(const Map &other) : root_(Node::Clone(other.root_)) {}

  Map(Map &&other) : root_(other.root_) { other.root_ = nullptr; }

  auto begin() -> iterator {
    if (!root_) {
      return end();
    }
    Node *p = root_;
    while (p->left_child_) {
      p = p->left_child_;
    }
    return iterator(this, p);
  }

  auto begin() const -> const_iterator {
    if (!root_) {
      return end();
    }
    Node *p = root_;
    while (p->left_child_) {
      p = p->left_child_;
    }
    return const_iterator(this, p);
  }

  auto cbegin() const -> const_iterator { return begin(); }

  auto end() -> iterator { return iterator(this, nullptr); }

  auto end() const -> const_iterator { return const_iterator(this, nullptr); }

  auto cend() const -> const_iterator { return end(); }

  auto back() -> iterator {
    if (!root_) {
      return end();
    }
    Node *p = root_;
    while (p->right_child_) {
      p = p->right_child_;
    }
    return iterator(this, p);
  }

  auto back() const -> const_iterator {
    if (!root_) {
      return end();
    }
    Node *p = root_;
    while (p->right_child_) {
      p = p->right_child_;
    }
    return const_iterator(this, p);
  }

  auto Size() const -> size_type { return root_ ? root_->size_ : 0; }

  auto operator=(const Map &other) -> Map & {
    if (this != &other) {
      delete root_;
      root_ = Node::Clone(other.root_);
    }
    return *this;
  }

  auto operator=(Map &&other) -> Map & {
    if (this != &other) {
      delete root_;
      root_ = other.root_;
      other.root_ = nullptr;
    }
    return *this;
  }

  auto Empty() const -> bool { return !root_; }

  void Clear() {
    delete root_;
    root_ = nullptr;
  }

  void Insert(const key_type &key, const value_type &value) {
    Node *p = root_;
    while (p) {
      if (Compare()(key, p->key_)) {
        p = p->left_child_;
      } else if (Compare()(p->key_, key)) {
        p = p->right_child_;
      } else {
        p->value_ = value;
        return;
      }
    }
    Node *middle_tree = new Node(key, value);
    Node *right_tree;
    Node::SplitL(root_, key, root_, right_tree);
    root_ = Node::Merge(root_, middle_tree);
    root_ = Node::Merge(root_, right_tree);
  }

  void Insert(const key_type &key, value_type &&value) {
    Node *p = root_;
    while (p) {
      if (Compare()(key, p->key_)) {
        p = p->left_child_;
      } else if (Compare()(p->key_, key)) {
        p = p->right_child_;
      } else {
        p->value_ = std::move(value);
        return;
      }
    }
    Node *middle_tree = new Node(key, std::move(value));
    Node *right_tree;
    Node::SplitL(root_, key, root_, right_tree);
    root_ = Node::Merge(root_, middle_tree);
    root_ = Node::Merge(root_, right_tree);
  }

  auto Delete(const key_type &key) -> bool {
    // std::cerr << "Delete: " << key << std::endl;
    // Node::Debug(root_);

    Node *middle_tree;
    Node *right_tree;
    Node::SplitL(root_, key, root_, right_tree);

    // std::cerr << "Delete1 root_: " << std::endl;
    // Node::Debug(root_);
    // std::cerr << "Delete1 right_tree: " << std::endl;
    // Node::Debug(right_tree);

    Node::SplitLE(right_tree, key, middle_tree, right_tree);

    // std::cerr << "Delete2 root_: " << std::endl;
    // Node::Debug(root_);
    // std::cerr << "Delete2 middle_tree: " << std::endl;
    // Node::Debug(middle_tree);
    // std::cerr << "Delete2 right_tree: " << std::endl;
    // Node::Debug(right_tree);

    // std::cerr << "Deleted size: " << (root_ ? root_->size_ : 0) << ' '
    //           << (middle_tree ? middle_tree->size_ : 0) << ' '
    //           << (right_tree ? right_tree->size_ : 0) << std::endl;

    root_ = Node::Merge(root_, right_tree);
    if (middle_tree) {
      delete middle_tree;
      return true;
    }
    return false;
  }

  auto Contains(const key_type &key) const -> bool {
    Node *p = root_;
    while (p) {
      if (Compare()(key, p->key_)) {
        p = p->left_child_;
      } else if (Compare()(p->key_, key)) {
        p = p->right_child_;
      } else {
        return true;
      }
    }
    return false;
  }

  auto SplitL(const key_type &key) -> Map {
    Map other_map;
    Node::SplitL(root_, key, root_, other_map.root_);
    return other_map;
  }

  auto SplitLE(const key_type &key) -> Map {
    Map other_map;
    Node::SplitLE(root_, key, root_, other_map.root_);
    return other_map;
  }

  void Merge(Map &&other_map) {
    // Todo
    Assert(false, "Map::Merge(): Unimplemented!");
  }

  // Less than
  auto FindL(const key_type &key) -> iterator {
    Node *p = root_, *result = nullptr;
    while (p) {
      if (Compare()(p->key_, key)) {
        result = p;
        p = p->right_child_;
      } else {
        p = p->left_child_;
      }
    }
    return iterator(this, result);
  }

  auto FindL(const key_type &key) const -> const_iterator {
    Node *p = root_, *result = nullptr;
    while (p) {
      if (Compare()(p->key_, key)) {
        result = p;
        p = p->right_child_;
      } else {
        p = p->left_child_;
      }
    }
    return const_iterator(this, result);
  }

  // Less than or equal to
  auto FindLE(const key_type &key) -> iterator {
    Node *p = root_, *result = nullptr;
    while (p) {
      if (!Compare()(key, p->key_)) {
        result = p;
        p = p->right_child_;
      } else {
        p = p->left_child_;
      }
    }
    return iterator(this, result);
  }

  auto FindLE(const key_type &key) const -> const_iterator {
    Node *p = root_, *result = nullptr;
    while (p) {
      if (!Compare()(key, p->key_)) {
        result = p;
        p = p->right_child_;
      } else {
        p = p->left_child_;
      }
    }
    return const_iterator(this, result);
  }

  // Greater than
  auto FindG(const key_type &key) -> iterator {
    Node *p = root_, *result = nullptr;
    while (p) {
      if (Compare()(key, p->key_)) {
        result = p;
        p = p->left_child_;
      } else {
        p = p->right_child_;
      }
    }
    return iterator(this, result);
  }

  auto FindG(const key_type &key) const -> const_iterator {
    Node *p = root_, *result = nullptr;
    while (p) {
      if (Compare()(key, p->key_)) {
        result = p;
        p = p->left_child_;
      } else {
        p = p->right_child_;
      }
    }
    return const_iterator(this, result);
  }

  // Greater than or equal to
  auto FindGE(const key_type &key) -> iterator {
    Node *p = root_, *result = nullptr;
    while (p) {
      if (!Compare()(p->key_, key)) {
        result = p;
        p = p->left_child_;
      } else {
        p = p->right_child_;
      }
    }
    return iterator(this, result);
  }

  auto FindGE(const key_type &key) const -> const_iterator {
    Node *p = root_, *result = nullptr;
    while (p) {
      if (!Compare()(p->key_, key)) {
        result = p;
        p = p->left_child_;
      } else {
        p = p->right_child_;
      }
    }
    return const_iterator(this, result);
  }

  auto Find(const key_type &key) -> iterator {
    Node *p = root_;
    while (p) {
      if (Compare()(key, p->key_)) {
        p = p->left_child_;
      } else if (Compare()(p->key_, key)) {
        p = p->right_child_;
      } else {
        return iterator(this, p);
      }
    }
    return end();
  }

  auto Find(const key_type &key) const -> const_iterator {
    Node *p = root_;
    while (p) {
      if (Compare()(key, p->key_)) {
        p = p->left_child_;
      } else if (Compare()(p->key_, key)) {
        p = p->right_child_;
      } else {
        return const_iterator(this, p);
      }
    }
    return end();
  }

  auto operator[](const key_type &key) -> reference {
    Node *p = root_;
    while (p) {
      if (Compare()(key, p->key_)) {
        p = p->left_child_;
      } else if (Compare()(p->key_, key)) {
        p = p->right_child_;
      } else {
        return p->value_;
      }
    }
    Insert(key, value_type());
    return operator[](key);
  }

  auto operator[](const key_type &key) const -> const_reference {
    Node *p = root_;
    while (p) {
      if (Compare()(key, p->key_)) {
        p = p->left_child_;
      } else if (Compare()(p->key_, key)) {
        p = p->right_child_;
      } else {
        return p->value_;
      }
    }
    Assert(false, "Map::operator[](): Invalid key!");
  }
};

template <typename K, typename V, typename Compare = std::less<K>>
class SyncMap {
public:
  using key_type = K;
  using value_type = V;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using key_compare = Compare;
  using reference = value_type &;
  using const_reference = const value_type &;

private:
public:
  SyncMap() { Assert(false, "SyncMap: Unimplemented!"); }

  ~SyncMap() = default;
};

} // namespace ts_stl

#endif