#ifndef PDF_PARSER_HPP
#define PDF_PARSER_HPP
#include <iostream>

#include <podofo/podofo.h>
using namespace PoDoFo;
struct Chunk {
    std::string text;
    unsigned int pageNumber;
    size_t startChar;
    size_t endChar;
};

class PDFParser{
public:
    std::atomic<float> progress{ 0.0f };
    std::vector<Chunk> allChunks;
    bool extractionDone = false;

    void extractPDFThread(const char* filename);
    std::vector<Chunk> chunkTextWithMeta(
        const std::string& text,
        unsigned int pageNumber,
        size_t chunkWordSize = 600,
        size_t overlapWords = 100
    );
    std::vector<Chunk> extractTextChunks(const char* filename);
};
#endif // !
