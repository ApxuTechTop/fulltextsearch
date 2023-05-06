#include <filesystem>
#include <libindexer-c/indexer.h>
#include <libindexer/indexer.hpp>
extern "C" {

void free_index(Index cindex) {
	indexer::IndexBuilder *index
		= static_cast<indexer::IndexBuilder *>(cindex);
	delete index;
}

Index create_index(Configuration *config) {
	std::vector<std::string> stopwords;
	for (size_t i = 0; i < config->count; ++i) {
		stopwords.push_back(config->stopwords[i]);
	}
	parser::Configuration configuration(config->ngrammin, config->ngrammax,
										stopwords);
	indexer::IndexBuilder *index = new indexer::IndexBuilder(configuration);

	return static_cast<Index>(index);
}
void index_add_document(Index cindex, int doc_id, const char *doc_text) {
	indexer::IndexBuilder *index
		= static_cast<indexer::IndexBuilder *>(cindex);
	index->add_document(doc_id, std::string(doc_text));
}
void text_index_write(Index cindex, const char *path) {
	indexer::IndexBuilder *index
		= static_cast<indexer::IndexBuilder *>(cindex);
	indexer::TextIndexWriter writer;
	std::string str_path(path);
	std::filesystem::path path_to_index{ str_path };
	writer.write(path_to_index, index->get_index());
}
}