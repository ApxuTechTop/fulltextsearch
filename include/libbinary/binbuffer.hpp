#pragma once

#include <cstddef>
#include <libtrie/trie.hpp>
#include <string>
#include <unordered_map>
#include <vector>
namespace binary {
class BinaryBuffer {
  private:
	std::vector<char> buffer;

  public:
	std::vector<char> &get() { return buffer; }
	std::size_t write_to(const void *data, std::size_t size,
						 std::size_t offset) {
		for (std::size_t i = 0; i < size; ++i) {
			char c = *(reinterpret_cast<const char *>(data) + i);
			buffer.insert(std::next(buffer.begin(), offset + i), c); // ?
		}
		return offset;
	}
	void write(std::nullptr_t) {}
	std::pair<std::size_t, std::size_t> write(const void *data,
											  std::size_t size) {
		return { write_to(data, size, buffer.size()), buffer.size() };
	}
	std::pair<std::size_t, std::size_t> write(const std::string &str) {
		return {
			write(static_cast<const void *>(str.data()), str.size()).first,
			buffer.size()
		};
	}
	std::pair<std::size_t, std::size_t> write(const std::size_t value) {
		return { write(static_cast<const void *>(&value), sizeof(value)).first,
				 buffer.size() };
	}
	std::pair<std::size_t, std::size_t> write(const char value) {
		return { write(static_cast<const void *>(&value), sizeof(value)).first,
				 buffer.size() };
	}
	std::pair<std::size_t, std::size_t>
	write(const bool value) { // удостовериться
		return { write(static_cast<const void *>(&value), sizeof(value)).first,
				 buffer.size() };
	}
	std::pair<std::size_t, std::size_t> write(const BinaryBuffer &bbuf) {
		std::size_t offset = buffer.size();
		buffer.insert(buffer.end(), bbuf.buffer.begin(), bbuf.buffer.end());
		return { offset, buffer.size() };
	}
	template <class T>
	std::pair<std::size_t, std::vector<std::size_t> >
	write(std::vector<T> container) {
		std::vector<std::size_t> offsets;
		auto offset = std::get<0>(write(container.size()));
		for (std::size_t i = 0; i < container.size(); ++i) {
			offsets[i] = std::get<0>(write(container[i]));
		}
		return { offset, offsets };
	}
	template <class T, class U>
	std::tuple<std::size_t, std::unordered_map<T, std::size_t>, std::size_t>
	write(std::unordered_map<T, U> container) {
		std::unordered_map<T, std::size_t> offsets;
		auto offset = std::get<0>(write(container.size()));
		for (const auto &[key, value] : container) {
			offsets[key] = std::get<0>(write(value));
		}
		return { offset, offsets, buffer.size() };
	}

	template <class T, class I, class V>
	std::tuple<std::size_t, std::unordered_map<T, std::size_t>, std::size_t>
	write(std::unordered_map<T, std::unordered_map<I, V> > container,
		  std::unordered_map<I, std::size_t> other_offsets) {
		std::size_t offset;
		std::unordered_map<T, std::size_t> offsets;
		offset = write(container.size()).first;
		for (const auto &[key, value] : container) {
			offsets[key] = std::get<0>(write(value, other_offsets));
		}
		return { offset, offsets, buffer.size() };
	}
	template <class T, class U, class V>
	std::tuple<std::size_t, std::unordered_map<T, std::size_t>, std::size_t>
	write(std::unordered_map<T, U> container, V other_offsets) {
		std::size_t offset;
		std::unordered_map<T, std::size_t> offsets;
		offset = write(container.size()).first;
		for (const auto &[key, value] : container) {
			write(other_offsets[key]);
			offsets[key] = std::get<0>(write(value));
		}
		return { offset, offsets, buffer.size() };
	}
	std::pair<std::size_t, std::size_t>
	write(const tech::Trie<std::size_t> &trie) {
		std::unordered_map<tech::Trie<std::size_t>::Node *, std::size_t>
			child_offsets;
		std::size_t offset_start = buffer.size();
		auto nodes = trie.nodes();
		for (auto node : nodes) {
			auto [my_offset, my_offset_end]
				= write(node.has()); // записываю себя
			if (child_offsets.count(&node)
				> 0) { // проверяю: а не записывал ли кто нибудь мою позицию
				write_to(&my_offset, sizeof(my_offset),
						 child_offsets[&node]); // если да то надо подправить
			}
			for (auto *child : node) {
				write(child->letter);
			}
			for (auto *child : node) {
				auto [child_offset, child_offset_end]
					= write(my_offset); // my_offset просто ради std::size_t
				child_offsets[child] = child_offset;
			}
			write(node.is_leaf);
			if (node.is_leaf) {
				write(node.value);
			}
		}
		return { offset_start, buffer.size() };
	}
};
class BinaryReader {};
}