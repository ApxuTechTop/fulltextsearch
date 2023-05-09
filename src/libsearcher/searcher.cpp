#include <algorithm>
#include <cmath>
#include <exception>
#include <iostream>
#include <libsearcher/searcher.hpp>

std::vector<searcher::Result>
searcher::Searcher::search(std::string query,
						   const parser::Configuration &config) const {
	std::vector<searcher::Result> results;
	std::unordered_map<int, double> scores;

	std::vector<parser::Term> parsed_terms
		= find_ngrams(std::move(query), config);
	for (const auto &term : parsed_terms) {
		std::unordered_map<int, std::vector<std::size_t> > term_info
			= accessor.get_term_infos(term.ngram); // извлеченны документы в
												   // которых встречается термS
		auto df = term_info.size();
		for (const auto &[doc_id, entries] : term_info) {
			auto tfdt = entries.size();
			if (scores.contains(doc_id)) {
				scores[doc_id] = 0.0;
			}
			scores[doc_id]
				+= static_cast<double>(tfdt)
				   * std::log(static_cast<double>(accessor.total_docs())
							  / static_cast<double>(df));
		}
	}
	try {
		results.reserve(scores.size());
	} catch (const std::exception &e) {
		std::cout << e.what() << ' ' << scores.size() << '\n';
	}

	for (const auto [doc_id, score] : scores) {
		results.push_back(searcher::Result{ score, doc_id });
	}
	std::sort(results.begin(), results.end(),
			  [](searcher::Result &left, searcher::Result &right) {
				  bool answer = left.score > right.score;
				  if (left.score == right.score) {
					  answer = left.document_id < right.document_id;
				  }
				  return answer;
			  });

	return results;
}

std::vector<searcher::Result>
searcher::Searcher::search(std::string query) const {
	parser::Configuration config;
	return search(std::move(query), config);
}