#pragma once

#include <filesystem>
#include <libaccessor/accessor.hpp>

namespace accessor {
class TextIndexAccessor : public IndexAccessor {
  private:
	const std::filesystem::path path_to_index;

  public:
	TextIndexAccessor(const std::filesystem::path &path)
		: path_to_index(path){};
	std::string load_document(const int document_id) const override;
	std::size_t total_docs() const override;
	std::unordered_map<int, std::vector<std::size_t> >
	get_term_infos(const std::string &term) const override;
};
}
