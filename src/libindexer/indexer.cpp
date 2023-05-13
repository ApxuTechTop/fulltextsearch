#include <fstream>
#include <iostream>
#include <libbinary/binbuffer.hpp>
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
			std::ios_base::app);
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

template <class Os> Os &operator<<(Os &ostream, binary::BinaryBuffer bbuf) {
	ostream.write(bbuf.get().data(), bbuf.get().size());
	return ostream;
}

void indexer::BinaryIndexWriter::write(const std::filesystem::path &path,
									   const indexer::Index &index) const {
	namespace fs = std::filesystem;
	fs::create_directories(path);
	std::fstream ofs(path / "index",
					 std::ios_base::out | std::ios_base::binary);

	// docs section
	binary::BinaryBuffer docs_buffer;
	std::unordered_map<int, std::uint32_t> docs_offsets;
	for (const auto& [doc_id, text] : index.docs) {
		docs_offsets[doc_id] = std::get<0>(docs_buffer.write(doc_id));
		docs_buffer.write(text);
	}
	// entries section
	binary::BinaryBuffer entries_buffer;
	std::unordered_map<std::string, std::uint32_t> entries_offsets;
	for (const auto &[key, docs] : index.entries) {
		entries_offsets[key] = std::get<0>(entries_buffer.write(static_cast<std::uint32_t>(docs.size())));
		for (const auto& [doc_id, positions] : docs) {
			entries_buffer.write(docs_offsets[doc_id]);
			entries_buffer.write(static_cast<std::uint32_t>(positions.size()));
			for (const auto& pos : positions) {
				entries_buffer.write(static_cast<std::uint32_t>(pos));
			}
		}
	}
	// auto [entries_section_start, entries_offsets, entries_section_end]
	// 	= entries_buffer.write(index.entries, docs_offsets);
	// dictionary section
	binary::BinaryBuffer dictionary_buffer;
	tech::Trie<std::uint32_t> trie;
	for (const auto &[term, offset] : entries_offsets) {
		// std::cout << term << " -> " << std::hex << offset << '\n';
		trie.add(term, offset);
	}
	dictionary_buffer.write(trie);
	// header section
	binary::BinaryBuffer bbuf;
	bbuf.write(static_cast<std::uint8_t>(4));
	bbuf.write(std::string("header"));
	bbuf.write(static_cast<std::uint32_t>(0));
	auto [dictionary_name_start, dictionary_name_end]
		= bbuf.write(std::string("dictionary"));
	bbuf.write(static_cast<std::uint32_t>(0));

	auto [entries_name_start, entries_name_end]
		= bbuf.write(std::string("entries"));
	bbuf.write(static_cast<std::uint32_t>(0));

	auto [docs_name_start, docs_name_end] = bbuf.write(std::string("docs"));
	bbuf.write(static_cast<std::uint32_t>(0));

	auto dictionary_section_offset
		= std::get<0>(bbuf.write(dictionary_buffer));
	auto entries_section_offset = std::get<0>(bbuf.write(entries_buffer));
	auto docs_section_offset = std::get<0>(bbuf.write(docs_buffer));

	bbuf.write_to(&dictionary_section_offset, 4, dictionary_name_end);
	bbuf.write_to(&entries_section_offset, 4, entries_name_end);
	bbuf.write_to(&docs_section_offset, 4, docs_name_end);

	ofs << bbuf;
}
