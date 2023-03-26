#include <algorithm>
#include <cctype>
#include <libparser/parser.hpp>
#include <regex>

static std::vector<std::string> normalize_single_pass(std::string text) {

	auto found = std::remove_if(text.begin(), text.end(), [](unsigned char x) {
		return std::ispunct(x);
	});
	text.erase(found, text.end());
	for (auto &sym : text) {
		sym = static_cast<char>(std::tolower(static_cast<unsigned char>(sym)));
	}

	std::regex word_regex("(\\w+)");
	auto words_begin
		= std::sregex_token_iterator(text.begin(), text.end(), word_regex);
	auto words_end = std::sregex_token_iterator();
	std::vector<std::string> words{ words_begin, words_end };

	return words;
}

namespace parser {
std::vector<parser::Term> find_ngrams(std::string text,
									  const parser::Configuration &config) {
	std::vector<parser::Term> terms;
	auto words = normalize_single_pass(std::move(text));
	auto found = std::remove_if(
		words.begin(), words.end(), [&config](const std::string &x) {
			const std::string &word = x;
			return std::find_if(config.stopwords.begin(),
								config.stopwords.end(),
								[&word](const std::string &stopword) {
									return stopword == word;
								})
				   != config.stopwords.end();
		});
	words.erase(found, words.end());
	for (size_t i = 0; i < words.size(); ++i) {
		const std::string &word = words[i];
		const int wl = static_cast<int>(word.length());
		const int maxl
			= std::min(wl, (config.ngrammax == 0) ? wl : config.ngrammax);
		for (int l = config.ngrammin; l <= maxl; ++l) {
			parser::Term term{ word.substr(0, l), i };
			terms.push_back(term);
		}
	}
	return terms;
}
} // namespace parser