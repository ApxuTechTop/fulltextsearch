#pragma once

#include <cstddef>
#include <iostream>
#include <libtrie/trie.hpp>
#include <utility>
#include <cstring>

namespace binary {
	template <typename T>
	concept Vector = std::same_as<T, std::vector<typename T::value_type>>;

	template <typename T>
	concept Trie = std::same_as<T, tech::Trie<typename T::value_type>>;


class BinaryReader {
  private:
	char *data;
	std::size_t current;

  public:
	BinaryReader(char *ptr, std::size_t offset = 0)
		: data(ptr), current(offset) {}


	// template<>
	// std::pair<std::size_t, std::size_t> read<std::size_t>(std::size_t pos) {
	//     return {*(reinterpret_cast<std::size_t*>(data + pos)),
	//     sizeof(std::size_t)};
	// }
	// template<>
	// std::pair<int, std::size_t> read<int>(std::size_t pos) {
	//     return {*(reinterpret_cast<int*>(data + pos)), sizeof(int)};
	// }
	// template<>
	// std::pair<bool, std::size_t> read<bool>(std::size_t pos) {
	//     return {*(reinterpret_cast<bool*>(data + pos)), sizeof(bool)};
	// }

	template <class T> T read() {
		std::cout << "read<" << typeid(T).name() << ">()\n";
		auto [value, readed_bytes] = read<T>(current);
		current += readed_bytes;
		std::cout << "current += readed_bytes == " << current << '\n';
		return value;
	}
	


	template <Vector V>
	std::pair<V, std::size_t> read(std::size_t pos) {
		using T = typename V::value_type;

		V vec;
		std::size_t all_readed_bytes = 0;
		auto [count, readed_bytes] = read<std::uint32_t>(pos);
		all_readed_bytes += readed_bytes;
		pos += readed_bytes;
		vec.reserve(count);
		for (std::size_t i = 0; i < count; ++i) {
			auto [value, readed_bytes] = read<T>(pos);
			vec.push_back(value);
			pos += readed_bytes;
			all_readed_bytes += readed_bytes;
		}
		return { vec, all_readed_bytes };
	}



	template <Trie T>
	typename T::value_type read(std::size_t pos, const std::string &word) {
		using ValueType = typename T::value_type;

		for (const auto &sym : word) {
			auto [child_count, readed_bytes] = read<std::uint32_t>(pos);
			pos += readed_bytes;
			if (child_count == 0) {
				auto [is_leaf, readed_bytes] = read<bool>(pos);
				pos += readed_bytes;
				if (is_leaf) {
					auto [value, readed_bytes]
						= read<ValueType>(pos);
					return value;
				}
			}
			uint32_t i;
			for (i = 0; i < child_count; ++i) {
				auto [letter, readed_bytes] = read<char>(pos);
				pos += readed_bytes;
				if (letter == sym) {
					break;
				}
			}
			pos += child_count - (i + 1);
			pos += sizeof(std::uint32_t) * i;
			auto new_pos = std::get<0>(read<std::uint32_t>(pos));
			pos = current + new_pos;
		}
		throw std::runtime_error(std::string("Didn't find value for word = ")
								 + word);
	}

	template <Trie T> typename T::value_type read(const std::string &word) {
		std::cout << current << '\n';
		std::cout.flush();
		auto value = read<T>(current, word);
		return value;
	}

	template <class T> std::pair<T, std::size_t> read(std::size_t pos) {
		T value;
		std::cout << "read<" << typeid(T).name() << ">(" << pos << ")\n";
		std::memcpy(&value, data + pos, sizeof(T));
		return { value, sizeof(T) };
	}
};

template <>
std::pair<std::string, std::size_t>
BinaryReader::read<std::string>(std::size_t pos) {
	std::cout << "read<std::string>(" << pos << ")\n";
	auto [length, readed_bytes] = read<std::uint8_t>(pos);
	std::string str(data + pos + readed_bytes, length);
	return { std::move(str), readed_bytes + length };
}

}