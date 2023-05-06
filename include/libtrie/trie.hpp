#include <cstddef>
namespace tech {
template <class T> class Trie {
  private:
	static const std::size_t ALPHABET_SIZE = 26;

  public:
	/* member types */

	struct Node {
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
		Iterator begin() { return Iterator(children, 0); }
		Iterator end() { return Iterator(children, ALPHABET_SIZE); }
		bool is_leaf = false;
		T value;
		char letter;
		Node *children[ALPHABET_SIZE] = { nullptr };
		Node *parent = nullptr;
		// check
		Node *&operator[](char c) { return children[c - 'a']; }
		std::size_t has() const {
			std::size_t count = 0;
			for (std::size_t i = 0; i < ALPHABET_SIZE; ++i) {
				if (children[i]) {
					count++;
				}
			}
			return count;
		}
		bool has(std::size_t count) const {
			for (auto i = 0; i < ALPHABET_SIZE; ++i) {
				if (children[i]) {
					count--;
					if (count < 0)
						return false;
				}
			}
			return count == 0;
		}
		Node() = default;
		Node(Node *ptr, char c) : letter(c), parent(ptr) {}
		Node(const Node &node) {
			is_leaf = node.is_leaf;
			value = node.value;
			parent = node.parent;
			letter = node.letter;
			for (std::size_t i = 0; i < ALPHABET_SIZE; ++i) {
				if (node.children[i]) {
					children[i] = new Node(*(node.children[i]));
				}
			}
		}
		Node(Node &&node) {
			is_leaf = node.is_leaf;
			value = std::move(node.value);
			parent = node.parent;
			letter = node.letter;
			for (auto i = 0; i < ALPHABET_SIZE; ++i) {
				if (node.children[i]) {
					children[i] = new Node(std::move(*(node.children[i])));
				}
			}
		}
		Node(const T &_value) : value(_value), is_leaf(true) {}
		~Node() {
			for (std::size_t i = 0; i < ALPHABET_SIZE; ++i) {
				delete children[i];
			}
		}
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
	Trie() { root = new Node(); }

	/* rule of 5 */
	~Trie() { clean(); }
	Trie &operator=(const Trie &trie) {
		clean();
		root = new Node(*trie.root);
	}
	Trie &operator=(Trie &&trie) {
		clean();
		root = trie.root;
	}
	Trie(const Trie &trie) { root = new Node(*trie.root, 0); }
	Trie(Trie &&trie) {
		root = trie.root;
		trie.root = nullptr;
	}
	class Nodes {
		Node *root;

	  public:
		class Iterator {
		  protected:
			Node *current;

		  public:
			using difference_type = std::ptrdiff_t;
			using value_type = Node;
			using pointer = Node *;
			using reference = Node &;
			using iterator_category = std::forward_iterator_tag;
			explicit Iterator(Node *ptr) : current(ptr) {}
			Iterator(const Iterator &it) = default;
			reference operator*() { return *current; }
			pointer operator->() { return current; }
			bool operator==(const Iterator &another) const {
				return current == another.current;
			}
			bool operator!=(const Iterator &another) const {
				return current != another.current;
			}
			Iterator &operator++() {
				if (!current) {
					return *this;
				}
				if (current->begin() == current->end()) {
					// если у текущего узла нет потомков
					// то необходимо вернуться к родителю
					// и перейти к потомку который после него
					Node *parent = current->parent;
					auto it = parent->begin();
					while (*it != current) {
						++it;
					}
					current = *(++it);
					// если этого потомка нет (крайний правый)
					// то ничего не делаем?
				} else {
					current = *(++(current->begin()));
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
		Nodes(Node *node) : root(node) {}
	};
	Nodes nodes() const { return Nodes(root); }

	void add(const std::string &text, T value) {
		Node *current = root;
		for (const char &sym : text) {
			if ((*current)[sym]) {
				(*current)[sym] = new Node(current, sym);
			}
			current = (*current)[sym];
		}
		current->is_leaf = true;
		current->value = value;
	}
	void remove(const std::string &text) {
		Node *last_branch = nullptr;
		Node *current = root;
		for (const auto &sym : text) {
			if (!(*current)[sym]) {
				return;
			}
			if (current->has() > 1) {
				last_branch = (*current)[sym]; // CHECK
			}
			current = (*current)[sym];
		}
		delete last_branch;
	}

  private:
	Node *root;
	void clean() { delete root; }
};
}