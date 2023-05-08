#pragma once

#include <iostream>
#include <cstddef>
#include <libtrie/trie.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstring>

namespace binary {
class BinaryBuffer {
  private:
	std::vector<char> buffer;

  public:
	using offset_type = std::uint32_t;
	std::vector<char> &get() { return buffer; }
	std::size_t write_to(const void *data, std::size_t size,
						 std::size_t offset) {
		std::memcpy(buffer.data() + offset, data, size);
		return offset;
	}
	void write(std::nullptr_t) {}
	std::pair<std::size_t, std::size_t> write(const void *data,
											  std::size_t size) {
		auto start_offset = buffer.size();
		buffer.resize(buffer.size() + size);
		std::memcpy(buffer.data() + start_offset, data, size);
		return {start_offset, buffer.size() };
	}
	std::pair<std::size_t, std::size_t> write(const std::string &str) {
		auto start_offset = std::get<0>(write(static_cast<std::uint8_t>(str.size())));
		write(static_cast<const void *>(str.data()), str.size());
		return {start_offset, buffer.size()};
	}
	template<class T>
	std::pair<std::size_t, std::size_t> write(const T& value) {
		return write(static_cast<const void *>(&value), sizeof(T));
	}
	std::pair<std::size_t, std::size_t> write(const BinaryBuffer &bbuf) {
		std::size_t offset = buffer.size();
		buffer.insert(buffer.end(), bbuf.buffer.begin(), bbuf.buffer.end());
		return { offset, buffer.size() };
	}
	template <class T>
	std::pair<std::size_t, std::vector<offset_type> >
	write(std::vector<T> container) {
		std::vector<offset_type> offsets;
		offsets.reserve(container.size());
		auto offset = std::get<0>(write(container.size()));
		for (const auto& value : container) {
			offsets.push_back(std::get<0>(write(value)));
		}
		return { offset, offsets };
	}
	template <class T, class U>
	std::tuple<std::size_t, std::unordered_map<T, offset_type>, std::size_t>
	write(std::unordered_map<T, U> container) {
		std::unordered_map<T, offset_type> offsets;
		auto offset = std::get<0>(write(container.size()));
		for (const auto &[key, value] : container) {
			offsets[key] = std::get<0>(write(value));
		}
		return { offset, offsets, buffer.size() };
	}

	template <class T, class I, class V>
	std::tuple<std::size_t, std::unordered_map<T, offset_type>, std::size_t>
	write(std::unordered_map<T, std::unordered_map<I, V> > container,
		  std::unordered_map<I, offset_type> other_offsets) {
		std::size_t offset;
		std::unordered_map<T, offset_type> offsets;
		offset = std::get<0>(write(container.size()));
		for (const auto &[key, value] : container) {
			offsets[key] = std::get<0>(write(value, other_offsets));
		}
		return { offset, offsets, buffer.size() };
	}
	template <class T, class U, class V>
	std::tuple<std::size_t, std::unordered_map<T, offset_type>, std::size_t>
	write(std::unordered_map<T, U> container, V other_offsets) {
		std::size_t offset;
		std::unordered_map<T, offset_type> offsets;
		offset = std::get<0>(write(container.size()));
		for (const auto &[key, value] : container) {
			write(other_offsets[key]);
			offsets[key] = std::get<0>(write(value));
		}
		return { offset, offsets, buffer.size() };
	}
	std::pair<std::size_t, std::size_t>
	write(const tech::Trie<offset_type> &trie) {
		std::unordered_map<std::shared_ptr<tech::Trie<offset_type>::Node>, std::size_t>
			child_offsets;
		std::size_t offset_start = buffer.size();
		auto nodes = trie.nodes();
		for (auto node : nodes) {
			//std::cout << "auto [my_offset, my_offset_end] = write(node->has());\n";
			if (node == nodes.root) {
				std::cout << "in root\n";
			}
			auto my_offset
				= static_cast<offset_type>(std::get<0>(write(static_cast<offset_type>(node->has())))); // записываю себя
			//std::cout << "if (child_offsets.contains(node))\n";
			if (child_offsets.contains(node)) { // проверяю: а не записывал ли кто нибудь мою позицию
				//std::cout << "write_to(&my_offset, sizeof(my_offset), child_offsets[node]);\n";
				
				write_to(&my_offset, sizeof(offset_type),
						 child_offsets[node]); // если да то надо подправить
			}
			//std::cout << "node->children\n";
			for (auto [letter, child] : node->children) {
				write(letter);
			}
			//std::cout << "node->children offset\n";
			for (auto [letter, child] : node->children) {
				//std::cout << "auto [child_offset, child_offset_end] = write(my_offset);\n";
				auto [child_offset, child_offset_end]
					= write(static_cast<offset_type>(0)); // my_offset просто ради std::size_t
				//std::cout << "child_offsets[child] = child_offset;\n";
				child_offsets[child] = child_offset;
			}
			//std::cout << "write(node->is_leaf);\n";
			write(node->is_leaf);
			if (node->is_leaf) {
				//std::cout << "node->value\n";
				write(node->value);
				//std::cout << "/node->value\n";
			}
		};
		return { offset_start, buffer.size() };
	}
};
class BinaryReader {};
}