#include <gtest/gtest.h>
#include <libindexer/indexer.hpp>

TEST(IndexerTests, ExampleTest) {
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
	indexer::Index index = {};

	index.entries = {
		{ "mat",
		  { { file_id1, { 0 } }, { file_id2, { 0 } }, { file_id3, { 0 } } } },
		{ "matr",
		  { { file_id1, { 0 } }, { file_id2, { 0 } }, { file_id3, { 0 } } } },
		{ "matri",
		  { { file_id1, { 0 } }, { file_id2, { 0 } }, { file_id3, { 0 } } } },
		{ "matrix",
		  { { file_id1, { 0 } }, { file_id2, { 0 } }, { file_id3, { 0 } } } },
		{ "rel", { { file_id2, { 1 } } } },
		{ "relo", { { file_id2, { 1 } } } },
		{ "reloa", { { file_id2, { 1 } } } },
		{ "reload", { { file_id2, { 1 } } } },
		{ "rev", { { file_id3, { 1 } } } },
		{ "revo", { { file_id3, { 1 } } } },
		{ "revol", { { file_id3, { 1 } } } },
		{ "revolu", { { file_id3, { 1 } } } },
	};
	ASSERT_EQ(builder.get_index().entries, index.entries);
}

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

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
