#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace accessor {
class IndexAccessor {
  public:
	virtual std::string load_document(const int document_id) const = 0;
	virtual std::size_t total_docs() const = 0;
	virtual std::unordered_map<int, std::vector<std::size_t> >
	get_term_infos(const std::string &term) const = 0;
};
}