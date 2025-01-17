#pragma once
#include <string>
#include <vector>

/*
std::vector<term> fun(configuration)

typedef struct {
	std::string ngram,
	int index
} Term;

std::string term.ngram
int term.index

int configuration.ngrammin
int configuration.ngrammax
std::string configuration.text
std::vector<std::string> configuration.stop_words



*/

namespace parser {

class Configuration {
  public:
	int ngrammin;
	int ngrammax;
	std::vector<std::string> stopwords;
	Configuration() : ngrammin(3), ngrammax(6){};
	Configuration(int nmin, int nmax, std::vector<std::string> swords)
		: ngrammin(nmin), ngrammax(nmax), stopwords(swords) {}
};
struct Term {
	std::string ngram;
	size_t index;
};

std::vector<parser::Term> find_ngrams(std::string text,
									  const parser::Configuration &config);
}