#pragma once

#include <memory>
#include <unordered_map>
#include <cstddef>
namespace tech {
template <class T> class Trie {
  private:

  public:
	/* member types */
	using value_type = T;
	struct Node {
		/*
		class Iterator {
		  protected:
			Node **children;
			std::size_t pos;

		  public:
			using difference_type = std::ptrdiff_t;
			using value_type = Node *;
			using pointer = Node **;
			using reference = Node *&;
			using iterator_category = std::forward_iterator_tag;
			explicit Iterator(Node **ptr, std::size_t cur_pos)
				: children(ptr), pos(cur_pos) {
				while ((pos < ALPHABET_SIZE) && !children[pos++]) {
				}
			}
			Iterator(const Iterator &it) = default;
			reference operator*() { return children[pos]; }
			pointer operator->() { return &(children[pos]); }
			bool operator==(const Iterator &another) const {
				return (children == another.children) && (pos == another.pos);
			}
			bool operator!=(const Iterator &another) const {
				return !((children == another.children)
						 && (pos == another.pos));
			}
			Iterator &operator++() {
				while ((pos < ALPHABET_SIZE) && !children[pos++]) {
				}
				return *this;
			}
			Iterator operator++(int) {
				auto old = *this;
				++(*this);
				return old;
			}
		};
		*/
		//auto begin() { return children.begin(); }
		//auto end() { return children.end(); }
		bool is_leaf = false;
		T value;
		std::unordered_map<char, std::shared_ptr<Node>> children;
		std::weak_ptr<Node> parent;
		std::size_t has() const {
			return children.size();
		}
		bool has(std::size_t count) const {
			return children.size() == count;
		}
		Node() = default;
		Node(std::weak_ptr<Node> ptr) : parent(ptr) {}
		Node(const Node &node) {
			is_leaf = node.is_leaf;
			value = node.value;
			parent = node.parent;
			children = node.children;
		}
		Node(Node &&node) {
			is_leaf = node.is_leaf;
			value = std::move(node.value);
			parent = std::move(node.parent);
			children = std::move(node.children);
		}
		Node(const T &_value) : value(_value), is_leaf(true) {}
	};
	class Iterator {
	  protected:
		Node *current;

	  public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T *;
		using reference = T &;
		using iterator_category = std::forward_iterator_tag;
		explicit Iterator(Node *ptr) : current(ptr) {}
		Iterator(const Iterator &it) = default;
		reference operator*() { return current->value; }
		pointer operator->() { return &(current->value); }
		bool operator==(const Iterator &another) const {
			return current == another.current;
		}
		bool operator!=(const Iterator &another) const {
			return current != another.current;
		}
		Iterator &operator++() {
			if (current) {
				current = current->next;
			}
			return *this;
		}
		Iterator operator++(int) {
			auto old = *this;
			++(*this);
			return old;
		}
	};

	/* iterators */
	Iterator begin() { return Iterator(root); }
	Iterator end() { return Iterator(nullptr); }

	/* constructors */
	Trie() { root = std::shared_ptr<Node>(new Node()); }

	/* rule of 5 */
	Trie &operator=(const Trie &trie) {
		root = std::shared_ptr<Node>(new Node(trie.root));
	}
	Trie &operator=(Trie &&trie) {
		root = std::move(trie.root);
	}
	Trie(const Trie &trie) { root = std::shared_ptr<Node>(new Node(trie.root)); }
	Trie(Trie &&trie) {
		root = trie.root;
		trie.root = nullptr;
	}
	class Nodes {
		public:
		std::shared_ptr<Node> root;
		class Iterator {
		  protected:
			std::shared_ptr<Node> current;

		  public:
			using difference_type = std::ptrdiff_t;
			using value_type = std::shared_ptr<Node>;
			using pointer = Node *;
			using reference = std::shared_ptr<Node> &;
			using iterator_category = std::forward_iterator_tag;
			explicit Iterator(std::shared_ptr<Node> ptr) : current(ptr) {}
			Iterator(const Iterator &it) = default;
			reference operator*() { return current; }
			pointer operator->() { return current; }
			bool operator==(const Iterator &another) const {
				return current == another.current;
			}
			Iterator &operator++() {
				if (!current) {
					return *this;
				}
				if (current->children.begin() == current->children.end()) {
					// если у текущего узла нет потомков
					// то необходимо вернуться к родителю
					// и перейти к потомку который после него
					std::shared_ptr<Node> parent;
					while (parent = current->parent.lock()) {
						auto it = parent->children.begin();
						while (it->second != current) {
							++it;
						}
						++it;
						if (it == parent->children.end()) {
							current = parent;
						} else {
							current = (it)->second;
							break;
						}
					}
					if (parent == nullptr) {
						current = nullptr;
					}
					// if (std::shared_ptr<Node> parent = current->parent.lock()) {
					// 	auto it = parent->children.begin();
					// 	while (it->second != current) {
					// 		++it;
					// 	}
					// 	// it->second == current значит мы нашли себя
					// 	++it; // следующий в очереди после нас
					// 	if (it == parent->children.end()) { // если после нас никого нет
					// 		current = nullptr;
					// 	} else {
					// 		current = (it)->second;
					// 	}
					// 	// если этого потомка нет (крайний правый)
					// 	// то ничего не делаем?
					// }
				} else {
					current = ((current->children.begin()))->second;
				}

				return *this;
			}
			Iterator operator++(int) {
				auto old = *this;
				++(*this);
				return old;
			}
		};
		Iterator begin() { return Iterator(root); }
		Iterator end() { return Iterator(nullptr); }
		Nodes(std::shared_ptr<Node> node) : root(node) {}
	};
	Nodes nodes() const { return Nodes(root); }

	void add(const std::string &text, T value) {
		std::shared_ptr<Node> current = root;
		for (const char &sym : text) {
			if (!current->children[sym]) {
				current->children[sym] = std::shared_ptr<Node>(new Node(current));
			}
			current = current->children[sym];
		}
		current->is_leaf = true;
		current->value = value;
	}
	void remove(const std::string &text) {
		std::shared_ptr<Node> current = root;
		std::shared_ptr<Node>& last_branch = current;
		for (const auto &sym : text) {
			if (!current->children[sym]) {
				return;
			}
			if (current->has() > 1) {
				last_branch = current->children[sym]; // CHECK
			}
			current = current->children[sym];
		}
		last_branch.reset();
	}

  private:
	std::shared_ptr<Node> root;
};
}