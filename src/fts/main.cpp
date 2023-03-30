#include <cxxopts.hpp>

#include <iostream>

#include <libindexer/indexer.hpp>
#include <libsearcher/searcher.hpp>

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static void print_results(const std::vector<searcher::Result>& results, const accessor::IndexAccessor& reader) {
	if (results.empty()) {
		std::cout << "No results\n";
		return;
	}
	double best_score = results[0].score;
	for (std::size_t i = 0; i < results.size(); ++i) {
		const auto& result = results[i];
		if (result.score < best_score / 2) {
			break;
		}
		std::cout << (i+1) << "\t" << result.score << "\t" << result.document_id << "\t" <<reader.load_document(result.document_id) << std::endl;
	}
}

int main(int argc, char **argv) {
	cxxopts::Options options("MyIndexer", "Index docs");
	options.add_options()("index", "Path to index location",
						  cxxopts::value<std::string>());
	options.add_options()("config", "Path to json config file",
						  cxxopts::value<std::string>());
	options.add_options()("query", "JSON file with documents",
						  cxxopts::value<std::string>());

	auto result = options.parse(argc, argv);
	json data;
	std::vector<std::string> stopwords;
	if (result["config"].count() > 0) {
		std::ifstream configfile(result["config"].as<std::string>());
		data = json::parse(configfile);
		stopwords = data["stopwords"].get<std::vector<std::string> >();
	}
	const int min_length
		= (result["config"].count() > 0) ? data["min"].get<int>() : 3;
	const int max_length
		= (result["config"].count() > 0) ? data["max"].get<int>() : 6;

	parser::Configuration config(min_length, max_length, stopwords);
	
	const std::string path_to_index = result["index"].as<std::string>();
	accessor::TextIndexAccessor reader(path_to_index);
	searcher::Searcher finder(reader);
	if (result["query"].count() > 0) {
		std::vector<searcher::Result> results = finder.search(result["query"].as<std::string>(), config);
		print_results(results, reader);
		return 0;
	}
	do {
		std::string input_query;
		std::getline(std::cin, input_query);
		if (input_query.empty()) {
			break;
		}
		std::vector<searcher::Result> results = finder.search(input_query, config);
		print_results(results, reader);
		std::cout << std::endl;

	} while (true);




	return 0;
}
