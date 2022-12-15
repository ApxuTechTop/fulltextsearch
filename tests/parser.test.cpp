#include <gtest/gtest.h>
#include <libparser/parser.hpp>

TEST(ParserTests, ParseTestClassic) {
	std::string text = "Dr. Jekyll and Mr. Hyde";
	std::vector<std::string> stopwords = { "dr", "and", "mr" };
	const int min_ngram_len = 3;
	const int max_ngram_len = 6;
	parser::Configuration config{ min_ngram_len, max_ngram_len, stopwords };
	std::vector<parser::Term> test_terms = parser::find_ngrams(text, config);
	std::vector<parser::Term> ex_terms
		= { { "jek", 0 },	 { "jeky", 0 }, { "jekyl", 0 },
			{ "jekyll", 0 }, { "hyd", 1 },	{ "hyde", 1 } };
	ASSERT_EQ(test_terms.size(), ex_terms.size());
	for (size_t i = 0; i < ex_terms.size(); ++i) {
		ASSERT_EQ(test_terms[i].ngram, ex_terms[i].ngram);
		ASSERT_EQ(test_terms[i].index, ex_terms[i].index);
	}
}

TEST(ParserTests, ParseTest2) {
	std::string text = "Dr.   \\. Jekyll and Mr. Hyde got married";
	std::vector<std::string> stopwords = { "dr", "and", "mr" };
	const int min_ngram_len = 5;
	const int max_ngram_len = 6;
	parser::Configuration config{ min_ngram_len, max_ngram_len, stopwords };
	std::vector<parser::Term> test_terms = parser::find_ngrams(text, config);
	std::vector<parser::Term> ex_terms
		= { { "jekyl", 0 }, { "jekyll", 0 }, { "marri", 3 }, { "marrie", 3 } };
	ASSERT_EQ(test_terms.size(), ex_terms.size());
	for (size_t i = 0; i < ex_terms.size(); ++i) {
		ASSERT_EQ(test_terms[i].ngram, ex_terms[i].ngram);
		ASSERT_EQ(test_terms[i].index, ex_terms[i].index);
	}
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
