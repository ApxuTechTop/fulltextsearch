//#include <fcntl.h>
#include <fstream>
#include <libaccessor/binary_accessor.hpp>
#include <libbinary/binreader.hpp>
//#include <sys/mman.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <unistd.h>
#include <new>
#include <filesystem>
//#include <cerrno>
//#include <cstring>

accessor::DictionaryAccessor::DictionaryAccessor(char *ptr) : data(ptr) {}
std::uint32_t
accessor::DictionaryAccessor::retrieve(const std::string &word) const {
	binary::BinaryReader reader(data, section_offset);
	try {
		auto offset = reader.read<tech::Trie<std::uint32_t> >(word);
		return offset;
	} catch (const std::runtime_error &e) {
		std::cerr << "Error: " << e.what() << '\n';
		return 0;
	}
}

accessor::EntriesAccessor::EntriesAccessor(char *ptr) : data(ptr) {}
std::unordered_map<int, std::vector<std::size_t> >
accessor::EntriesAccessor::get_term_infos(const std::size_t offset) const {
	binary::BinaryReader reader(data, section_offset + offset);
	std::unordered_map<int, std::vector<std::size_t> > term_info;
	auto doc_count = reader.read<std::uint32_t>();
	for (std::uint32_t i = 0; i < doc_count; ++i) {
		auto doc_offset = reader.read<std::uint32_t>();
		auto doc_id = std::get<0>(reader.read<int>(doc_offset));
		auto pos_count = reader.read<std::uint32_t>();
		term_info[doc_id].reserve(pos_count);
		for (std::uint32_t j = 0; j < pos_count; ++j) {
			term_info[doc_id].push_back(reader.read<std::uint32_t>());
		}
	}
	return term_info;
}

accessor::DocumentsAccessor::DocumentsAccessor(char *ptr) : data(ptr) {}
std::uint32_t accessor::DocumentsAccessor::total_docs() const {
	binary::BinaryReader reader(data, section_offset);
	return reader.read<std::uint32_t>();
}
std::string
accessor::DocumentsAccessor::load_document(const int document_id) const {
	binary::BinaryReader reader(data, section_offset);
	std::uint32_t count = total_docs();
	for (std::uint32_t i = 0; i < count; ++i) {
		auto doc_id = reader.read<int>();
		auto text = reader.read<std::string>();
		if (doc_id == document_id) {
			return text;
		}
	}
	return {};
}

// static std::pair<char *, std::size_t>
// map_file(const std::filesystem::path &path) {
// 	// открыть файл
// 	int fd
// 		= open(path.c_str(), O_RDWR | O_CLOEXEC); // O_CLOEXEC узнать подробней
// 	if (fd == -1) {
// 		throw std::filesystem::filesystem_error("Cant open file", path, std::error_code());
// 	}
// 	// узнать размер
// 	struct stat statbuf {};
// 	fstat(fd, &statbuf);
// 	std::size_t size = statbuf.st_size;
// 	long sz = sysconf (_SC_PAGESIZE);
// 	size = ((size / sz) + 1) * sz;
// 	// mmap
// 	char *ptr = reinterpret_cast<char *>(
// 		mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0));
// 		std::cout << "mmap: " << size << " " << std::strerror(errno) << '\n';
// 		std::cout.flush();
// 	if (ptr == nullptr) {
// 		throw std::bad_alloc();
// 	}
// 	// закрыть файл
// 	close(fd);
// 	return { ptr, size };
// }

void accessor::BinaryIndexAccessor::set(char *ptr) {
	binary::BinaryReader reader(ptr);
	auto sections_count = reader.read<std::uint8_t>();
	if (sections_count != 4) {
		std::cout << "Bad header, got " << static_cast<int>(sections_count) << " sections\n";
	}
	auto header_name = reader.read<std::string>();
	if (header_name != "header") {
		std::cout << "Expected header, got " << header_name << '\n';
	}
	auto header_section_offset = reader.read<std::uint32_t>();
	if (header_section_offset != 0) {
		std::cout << "Expected 0, got " << header_section_offset << '\n';
	}
	auto dictionary_name = reader.read<std::string>();
	if (dictionary_name != "dictionary") {
		std::cout << "Expected dictionary, got " << dictionary_name << '\n';
	}
	auto dictionary_section_offset = reader.read<std::uint32_t>();
	std::cout << "dictionary_section_offset = " << dictionary_section_offset << '\n';
	auto entries_name = reader.read<std::string>();
	if (entries_name != "entries") {
		std::cout << "Expected entries, got " << entries_name << '\n';
	}
	auto entries_section_offset = reader.read<std::uint32_t>();
	std::cout << "entries_section_offset = " << entries_section_offset << '\n';
	auto documents_name = reader.read<std::string>();
	if (documents_name != "documents") {
		std::cout << "Expected documents, got " << documents_name << '\n';
	}
	auto documents_section_offset = reader.read<std::uint32_t>();
	std::cout << "documents_section_offset = " << documents_section_offset << '\n';
	dictionary_accessor.data = ptr;
	entries_accessor.data = ptr;
	documents_accessor.data = ptr;
	dictionary_accessor.section_offset = dictionary_section_offset;
	entries_accessor.section_offset = entries_section_offset;
	documents_accessor.section_offset = documents_section_offset;
}

accessor::BinaryIndexAccessor::BinaryIndexAccessor(
	const std::filesystem::path &path)
	: path_to_index(path) {
	std::ifstream f(path / "index", std::ios_base::in | std::ios_base::binary);
	f.seekg(0, std::ios_base::end);
	std::size_t size = f.tellg();
	data.reserve(size);
	f.read(data.data(), size);
	set(data.data());
}

std::string
accessor::BinaryIndexAccessor::load_document(const int document_id) const {
	return documents_accessor.load_document(document_id);
}
std::size_t accessor::BinaryIndexAccessor::total_docs() const {
	return documents_accessor.total_docs();
}
std::unordered_map<int, std::vector<std::size_t> >
accessor::BinaryIndexAccessor::get_term_infos(const std::string &term) const {
	return entries_accessor.get_term_infos(dictionary_accessor.retrieve(term));
}