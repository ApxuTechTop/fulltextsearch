#include <fstream>
#include <libaccessor/accessor.hpp>


static const auto docs_dir = "docs";
static const auto entries_dir = "entries";
namespace fs = std::filesystem;


std::string accessor::TextIndexAccessor::load_document(const int document_id) const {
    const std::string doc_name {std::to_string(document_id)};
    std::ifstream doc(path_to_index / docs_dir / doc_name);
    return std::string{std::istreambuf_iterator<char>(doc), std::istreambuf_iterator<char>()};
}

std::size_t accessor::TextIndexAccessor::total_docs() const {
    return std::distance(fs::directory_iterator{path_to_index / docs_dir}, fs::directory_iterator());
}
std::unordered_map<int, std::vector<std::size_t> >
accessor::TextIndexAccessor::get_term_infos(std::string term) const {
    std::unordered_map<int, std::vector<std::size_t>> term_info;
    const int hashsize = 6;
	std::stringstream strstream;
    strstream << std::hex << std::hash<std::string>{}(term);
    std::ifstream f(path_to_index / entries_dir / (strstream.str().substr(0, hashsize)));
    std::string term_line;
    while (std::getline(f, term_line)) {
        std::string readed_term;
        std::stringstream strstream(term_line);
        strstream >> readed_term;
        if (readed_term != term) {
            term_line = "";
            continue;
        }
        std::size_t doc_count = 0;
        strstream >> doc_count;
        for (std::size_t i = 0; i < doc_count; ++i) {
            int doc_id = 0;
            strstream >> doc_id;
            std::size_t entries_count = 0;
            strstream >> entries_count;
            for (std::size_t j = 0; j < entries_count; ++j) {
                std::size_t pos = 0;
                strstream >> pos;
                term_info[doc_id].push_back(pos);
            }
        }
        break;
    }

    return term_info;
}