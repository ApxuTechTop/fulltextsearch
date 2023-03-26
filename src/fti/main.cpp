#include <cxxopts.hpp>

#include <iostream>

#include <libindexer/indexer.hpp>

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(int argc, char **argv) {
	cxxopts::Options options("MyIndexer", "Index docs");
	options.add_options()("index", "Path to index location",
						  cxxopts::value<std::string>());
	options.add_options()("stopfile", "Path to stop-words file",
						  cxxopts::value<std::string>());
	options.add_options()("docsfile", "JSON file with documents",
						cxxopts::value<std::string>());
	


	auto result = options.parse(argc, argv);
	std::vector<std::string> stopwords;
	std::ifstream stopfile(result["stopfile"].as<std::string>());

	std::string tmp_word;
	while (stopfile >> tmp_word) {
		stopwords.push_back(tmp_word);
	}
	const int min_length = 3;
	const int max_length = 6;
	parser::Configuration config(min_length, max_length, stopwords);

    indexer::IndexBuilder builder(config);
	//std::vector<std::pair<int, std::string>> docs = {{file_id1, "The Matrix"}, {file_id2, "The Matrix Reloaded"}, {file_id3, "The Matrix Revolutions"}};
	std::ifstream jsonfile(result["docsfile"].as<std::string>());
	json data = json::parse(jsonfile);
	for (const auto& [id, text] : data["docs"].items()) {
		builder.add_document(std::stoi(id), text);
	}
    indexer::TextIndexWriter writer;
	std::string path_to_index = result["index"].as<std::string>();
	writer.write(std::filesystem::path{path_to_index}, builder.get_index());
	return 0;
}
