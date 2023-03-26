#include <fstream>
#include <iostream>
#include <libindexer/indexer.hpp>

indexer::IndexBuilder::IndexBuilder(
	parser::Configuration config) // изначально сделал const & но компилятор
								  // сказал передавай по значению
	: configuration(std::move(config)) {}

void indexer::IndexBuilder::add_document(const int document_id,
										 const std::string &text) {
	index.docs[document_id] = text;
	std::vector<parser::Term> terms
		= parser::find_ngrams(text, configuration); // config?
	for (auto &term : terms) {
		index.entries[term.ngram][document_id].push_back(term.index);
	}
}

indexer::Index indexer::IndexBuilder::get_index() const { return index; }

void indexer::TextIndexWriter::write(const std::filesystem::path &path,
									 const indexer::Index &index) const {
	namespace fs = std::filesystem;
	fs::create_directories(path);
	fs::path docs_dir{ "docs" };
	fs::path docs_path = path / docs_dir;
	fs::create_directories(docs_path);
	for (const auto &[id, text] : index.docs) {
		std::fstream ofs(docs_path / fs::path{ std::to_string(id) },
						 std::ios_base::out | std::ios_base::trunc);
		ofs << text;
	}
	fs::path entries_dir{ "entries" };
	fs::path entries_path = path / entries_dir;
	fs::create_directories(entries_path);
	const int hashsize = 6;
	std::stringstream strstream;
	for (const auto &[term, entries] : index.entries) {
		strstream << std::hex << std::hash<std::string>{}(term);
		std::fstream ofs(
			entries_path / fs::path{ (strstream.str().substr(0, hashsize)) },
			std::ios_base::out | std::ios_base::ate);
		strstream.str("");
		ofs << term << " " << entries.size() << " ";
		for (const auto &[id, positions] : entries) {
			ofs << id << " " << positions.size() << " ";
			for (const auto &pos : positions) {
				ofs << pos << " ";
			}
		}
		ofs << std::endl;
	}
}