#pragma once
#include <libaccessor/accessor.hpp>
#include <libparser/parser.hpp>

namespace searcher {
struct Result {
	double score;
	int document_id;
};
class Searcher {
  private:
	const accessor::IndexAccessor &accessor;

  public:
	Searcher(const accessor::IndexAccessor &indexAccessor)
		: accessor(indexAccessor){};
	std::vector<Result> search(std::string query) const;
	std::vector<Result> search(std::string query,
							   const parser::Configuration &config) const;
};

}