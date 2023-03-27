#pragma once
#include <filesystem>
#include <unordered_map>
#include <vector>

namespace accessor {
class IndexAccessor {

  public:
	virtual std::string load_document(const int document_id) const = 0;
	virtual std::size_t total_docs() const = 0;
	virtual std::unordered_map<int, std::vector<std::size_t> >
	get_term_infos(std::string term) const = 0;
};
class TextIndexAccessor : public IndexAccessor {
  private:
	const std::filesystem::path path_to_index;

  public:
	TextIndexAccessor(const std::filesystem::path &path) : path_to_index(path) {};
	std::string load_document(const int document_id) const;
	std::size_t total_docs() const;
	std::unordered_map<int, std::vector<std::size_t> > get_term_infos(std::string term) const;
};
}