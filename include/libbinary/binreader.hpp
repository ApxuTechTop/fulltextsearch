#pragma once

#include <cstddef>
#include <cstring>
#include <iostream>
#include <libtrie/trie.hpp>
#include <utility>

namespace binary {
template <typename T>
concept Vector = std::same_as<T, std::vector<typename T::value_type> >;

template <typename T>
concept Trie = std::same_as<T, tech::Trie<typename T::value_type> >;

class BinaryReader {
  private:
	const char *data;
	std::size_t current;

  public:
	BinaryReader(const char *ptr, std::size_t offset = 0)
		: data(ptr), current(offset) {}

	template <class T> T read() {
		auto [value, readed_bytes] = read<T>(current);
		current += readed_bytes;
		return value;
	}

	template <Vector V> std::pair<V, std::size_t> read(std::size_t pos) {
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
				throw std::runtime_error(
					std::string("Didn't find value for word = ") + word);
			}
			uint32_t i;
			for (i = 0; i < child_count; ++i) {
				auto [letter, readed_bytes] = read<char>(pos);
				pos += readed_bytes;
				if (letter == sym) {
					break; // нашли нужный символ
				}
			}
			pos += child_count - (i + 1); // пропускаем оставшиеся символы
			pos += sizeof(std::uint32_t) * i; // находим нужное нам смещение
			auto new_pos = std::get<0>(read<std::uint32_t>(pos)); // переходим
			pos = current + new_pos; // не забываем про смещение секции
		}
		// прошлись по слову, мы на месте, ищем значение
		auto [child_count, readed_bytes]
			= read<std::uint32_t>(pos); // надо пропустить всех его потомков
		pos += readed_bytes;
		pos += child_count + sizeof(std::uint32_t) * child_count;
		auto [is_leaf, _] = read<bool>(pos); // смотрим есть ли значение
		pos += sizeof(bool);
		if (is_leaf) {
			auto [value, readed_bytes] = read<ValueType>(pos); // считываем его
			return value;
		}
		throw std::runtime_error(std::string("Didn't find value for word = ")
								 + word);
	}

	template <Trie T> typename T::value_type read(const std::string &word) {
		auto value = read<T>(current, word);
		return value;
	}

	template <class T> std::pair<T, std::size_t> read(std::size_t pos) {
		T value; // как быть если нет дефолтного конструктора?
		// char value[sizeof(T)] только так получается
		std::memcpy(&value, data + pos, sizeof(T));
		return { value, sizeof(T) };
	}
};

template <>
std::pair<std::string, std::size_t>
BinaryReader::read<std::string>(std::size_t pos) {
	auto [length, readed_bytes] = read<std::uint8_t>(pos);
	std::string str(data + pos + readed_bytes, length);
	return { std::move(str), readed_bytes + length };
}

}