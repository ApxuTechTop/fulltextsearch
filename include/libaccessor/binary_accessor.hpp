#pragma once

#include <filesystem>
#include <libaccessor/accessor.hpp>
#include <vector>

namespace accessor {
class DictionaryAccessor {
  public:
	char *data = nullptr;
	std::size_t section_offset = 0;
	DictionaryAccessor(char *ptr = nullptr);
	std::uint32_t retrieve(const std::string &word) const;
};

class EntriesAccessor {
  public:
	char *data = nullptr;
	std::size_t section_offset = 0;
	EntriesAccessor(char *ptr = nullptr);
};

class DocumentsAccessor {
  public:
	char *data = nullptr;
	std::size_t section_offset = 0;
	DocumentsAccessor(char *ptr = nullptr);
	std::uint32_t total_docs() const;
	std::string load_document(const int document_id) const;
};

class BinaryIndexAccessor : public IndexAccessor {
  private:
	const std::filesystem::path path_to_index;
	std::vector<char> data;
	DictionaryAccessor dictionary_accessor;
	EntriesAccessor entries_accessor;
	DocumentsAccessor documents_accessor;
	void set(char *ptr);

  public:
	BinaryIndexAccessor(const std::filesystem::path &path);
	std::string load_document(const int document_id) const override;
	std::size_t total_docs() const override;
	std::unordered_map<int, std::vector<std::size_t> >
	get_term_infos(const std::string &term) const override;
};
}
