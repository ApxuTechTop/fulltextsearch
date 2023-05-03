#include <libaccessor-c/accessor.h>
#include <libaccessor/accessor.hpp>
#include <cstring>

extern "C" {
    void index_load_document(index_accessor accessor, int doc_id, char** doc_text) {
        if (accessor.type == index_accessor_type_text) {
            accessor::TextIndexAccessor text_accessor(std::string(accessor.path));
            std::string text = text_accessor.load_document(doc_id);
            char* buf = static_cast<char*>(malloc(sizeof(char) * (text.size() + 1)));
            std::memcpy(buf, text.data(), text.size() + 1);
            *doc_text = buf;
        } else if (accessor.type == index_accessor_type_binary) {

        }
    }
}