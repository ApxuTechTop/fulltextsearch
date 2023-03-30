#include <cxxopts.hpp>

#include <iostream>

#include <libindexer/indexer.hpp>

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <rapidcsv.h>

using json = nlohmann::json;

int main(int argc, char **argv) {
	cxxopts::Options options("MyIndexer", "Index docs");
	options.add_options()("index", "Path to index location",
						  cxxopts::value<std::string>());
	options.add_options()("config", "Path to json config file",
						  cxxopts::value<std::string>());
	options.add_options()("csv", "JSON file with documents",
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

	rapidcsv::Document doc(result["csv"].as<std::string>());
	std::vector<int> docs_id = doc.GetColumn<int>("bookID");
	std::vector<std::string> docs_text = doc.GetColumn<std::string>("title");
	indexer::IndexBuilder builder(config);
	for (std::size_t i = 0; i < docs_id.size(); ++i) {
		builder.add_document(docs_id[i], docs_text[i]);
	}

	const indexer::TextIndexWriter writer;
	const std::string path_to_index = result["index"].as<std::string>();
	writer.write(std::filesystem::path{ path_to_index }, builder.get_index());
	return 0;
}
