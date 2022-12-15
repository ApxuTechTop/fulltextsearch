#include <cxxopts.hpp>

#include <iostream>

#include <libparser/parser.hpp>

// ./parser --ngram-min=3 --ngram-max=6 --stop-words-file=""
// --stop-words="a","an"

int main(int argc, char **argv) {
	cxxopts::Options options("MyParser", "Parsing text to terms");
	options.add_options()("text", "Text to parse",
						  cxxopts::value<std::string>());
	options.add_options()("ngram-min", "Minimal length of ngram",
						  cxxopts::value<int>());
	options.add_options()("ngram-max", "Maximum length of ngram",
						  cxxopts::value<int>());
	options.add_options()("stop-words-file", "Name of file with stop words",
						  cxxopts::value<std::string>());
	options.add_options()("stop-words", "Stop words",
						  cxxopts::value<std::vector<std::string> >());
	auto result = options.parse(argc, argv);
	parser::Configuration config(
		result["ngram-min"].as<int>(), result["ngram-max"].as<int>(),
		result["stop-words"].as<std::vector<std::string> >());
	std::string text = result["text"].as<std::string>();
	std::vector<parser::Term> terms = parser::find_ngrams(text, config);
	for (auto &term : terms) {
		std::cout << term.ngram << " " << term.index << " " << std::endl;
	}

	return 0;
}
