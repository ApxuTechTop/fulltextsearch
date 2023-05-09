#include <iostream>
#include <libaccessor/text_accessor.hpp>
#include <libsearcher-c/searcher.h>
#include <libsearcher/searcher.hpp>

extern "C" {
vector_wrap accessor_search(index_accessor accessor, const char *query,
							Configuration *config) {
	std::vector<std::string> stopwords;
	for (size_t i = 0; i < config->count; ++i) {
		stopwords.emplace_back(config->stopwords[i]);
	}
	parser::Configuration configuration(config->ngrammin, config->ngrammax,
										stopwords);
	std::string string_query(query);
	std::vector<searcher::Result> *results = nullptr; // ????
	if (accessor.type == index_accessor_type_text) {
		accessor::TextIndexAccessor reader(std::string(accessor.path));
		searcher::Searcher finder(reader);
		results = new std::vector<searcher::Result>(
			finder.search(string_query, configuration)); // ????
	} else {
		results = new std::vector<searcher::Result>();
	}
	vector_wrap v; // ????
	v.vector_ptr = static_cast<void *>(results);
	v.data = results->data();
	v.count = results->size();
	return v;
}
}
