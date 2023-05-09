#include <gtest/gtest.h>
#include <libaccessor/text_accessor.hpp>
#include <libindexer/indexer.hpp>

TEST(AccessorTests, ExampleTest) {
	namespace fs = std::filesystem;
	const int file_id1 = 199903;
	const int file_id2 = 200305;
	const int file_id3 = 200311;
	std::vector<std::pair<int, std::string> > docs
		= { { file_id1, "The Matrix" },
			{ file_id2, "The Matrix Reloaded" },
			{ file_id3, "The Matrix Revolutions" } };
	std::vector<std::string> stopwords = { "the" };
	const int min_ngram_len = 3;
	const int max_ngram_len = 6;
	parser::Configuration config{ min_ngram_len, max_ngram_len, stopwords };
	indexer::IndexBuilder builder(config);
	for (const auto &[id, text] : docs) {
		builder.add_document(id, text);
	}

	indexer::TextIndexWriter writer;
	fs::path path_to_index{ "build/index" };
	writer.write(path_to_index, builder.get_index());
	accessor::TextIndexAccessor reader(path_to_index);
	for (const auto &[key, value] : builder.get_index().entries) {
		ASSERT_EQ(reader.get_term_infos(key), value);
	}
	for (const auto &[key, value] : builder.get_index().docs) {
		ASSERT_EQ(reader.load_document(key), value);
	}
	fs::remove_all(path_to_index);
}

/*
TEST(IndexerTests, SomePositionsTest) {
	const int file_id1 = 199903;
	const int file_id2 = 111111;
	std::vector<std::pair<int, std::string> > docs
		= { { file_id1, "The Matrix matter" }, { file_id2, "MATR" } };
	std::vector<std::string> stopwords = { "the" };
	const int min_ngram_len = 3;
	const int max_ngram_len = 6;
	parser::Configuration config{ min_ngram_len, max_ngram_len, stopwords };
	indexer::IndexBuilder builder(config);
	for (const auto &[id, text] : docs) {
		builder.add_document(id, text);
	}
	indexer::Index index = {};

	index.entries = {
		{ "mat", { { file_id1, { 0, 1 } }, { file_id2, { 0 } } } },
		{ "matr", { { file_id1, { 0 } }, { file_id2, { 0 } } } },
		{ "matri", { { file_id1, { 0 } } } },
		{ "matrix", { { file_id1, { 0 } } } },
		{ "matt", { { file_id1, { 1 } } } },
		{ "matte", { { file_id1, { 1 } } } },
		{ "matter", { { file_id1, { 1 } } } },
	};
	ASSERT_EQ(builder.get_index().entries, index.entries);
}
*/

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
