#include <cxxopts.hpp>

#include <iostream>

#include <libparser/parser.hpp>

#include <fstream>
#include <nlohmann/json.hpp>

// ./parser --ngram-min=3 --ngram-max=6 --stop-words-file=""
// --stop-words="a","an"

class JSONConfiguration : public parser::Configuration {
  public:
	explicit JSONConfiguration(std::stringstream &is)
		: parser::Configuration(nlohmann::json::parse(is)["ngrammin"],
								nlohmann::json::parse(is)["ngrammax"],
								nlohmann::json::parse(is)["stopwords"]) {}
	explicit JSONConfiguration(std::string &s)
		: parser::Configuration(nlohmann::json::parse(s)["ngrammin"],
								nlohmann::json::parse(s)["ngrammax"],
								nlohmann::json::parse(s)["stopwords"]) {}
};

int main(int argc, char **argv) {
	cxxopts::Options options("MyParser", "Parsing text to terms");
	options.add_options()("text", "Text to parse",
						  cxxopts::value<std::string>());
	options.add_options()("ngram-min", "Minimal length of ngram",
						  cxxopts::value<int>());
	options.add_options()("ngram-max", "Maximum length of ngram",
						  cxxopts::value<int>());
	options.add_options()("config-file", "Name of file with configuration",
						  cxxopts::value<std::string>());
	options.add_options()("stop-words", "Stop words",
						  cxxopts::value<std::vector<std::string> >());
	auto result = options.parse(argc, argv);
	parser::Configuration config;
	if (result.count("config-file") > 0) {
		std::ifstream file(result["config-file"].as<std::string>());
		std::string str{ std::istreambuf_iterator<char>(file),
						 std::istreambuf_iterator<char>() };
		std::cout << result["config-file"].as<std::string>();
		std::cout << str;
		config = JSONConfiguration(str);
	} else {
		config = parser::Configuration(
			result["ngram-min"].as<int>(), result["ngram-max"].as<int>(),
			result["stop-words"].as<std::vector<std::string> >());
	}
	std::string text = result["text"].as<std::string>();
	std::vector<parser::Term> terms = parser::find_ngrams(text, config);
	for (auto &term : terms) {
		std::cout << term.ngram << " " << term.index << " ";
	}
	std::cout << std::endl;
	return 0;
}
