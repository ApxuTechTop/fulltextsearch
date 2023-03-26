#pragma once
#include <filesystem>
#include <libparser/parser.hpp>
#include <unordered_map>

namespace indexer {
struct Index {
	std::unordered_map<int, std::string> docs;
	std::unordered_map<std::string,
					   std::unordered_map<int, std::vector<std::size_t> > >
		entries;
};
class IndexBuilder {
  private:
	Index index;
	parser::Configuration configuration;

  public:
	IndexBuilder(parser::Configuration config);

	void add_document(const int document_id, const std::string &text);
	Index get_index() const;
};
class IndexWriter {
  public:
	virtual void write(const std::filesystem::path &path,
					   const Index &index) const = 0;
};
class TextIndexWriter : public IndexWriter {
  public:
	TextIndexWriter(){};
	void write(const std::filesystem::path &path,
			   const Index &index) const override;
};
}