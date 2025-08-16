#include <PDFParser.hpp>

void PDFParser::extractPDFThread(const char* filename) {
    allChunks.clear();
    auto chunks = extractTextChunks(filename);
    allChunks.insert(allChunks.end(), chunks.begin(), chunks.end());
    extractionDone = true;
}
std::vector<Chunk> PDFParser::chunkTextWithMeta(const std::string& text, unsigned int pageNumber, size_t chunkWordSize, size_t overlapWords)
{
    std::vector<Chunk> chunks;
    // Split text into words
    std::istringstream iss(text);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word) words.push_back(word);
    if (words.empty()) return chunks;

    size_t start = 0;
    size_t charOffset = 0;

    while (start < words.size()) {
        size_t end = start + chunkWordSize;
        if (end > words.size()) end = words.size();

        // Join words into a single string chunk
        std::ostringstream oss;
        size_t chunkStartChar = charOffset;
        for (size_t i = start; i < end; ++i) {
            if (i != start) oss << " ";
            oss << words[i];
        }
        std::string chunkText = oss.str();
        size_t chunkEndChar = chunkStartChar + chunkText.size();

        chunks.push_back({ chunkText, pageNumber, chunkStartChar, chunkEndChar });

        // Update char offset
        charOffset += chunkText.size() + 1; // +1 for space/newline

        if (end == words.size()) break;
        start = end - overlapWords;
    }

    return chunks;


}

std::vector<Chunk> PDFParser::extractTextChunks(const char* filename)
{
    std::vector<Chunk> allChunks;

    try {
        PdfMemDocument pdf;
        pdf.Load(filename);

        auto& pages = pdf.GetPages();
        unsigned int pageCount = pages.GetCount();
        std::cout << "Number of pages: " << pageCount << std::endl;

        for (unsigned int i = 0; i < pageCount; ++i) {
            PdfPage* page = &pages.GetPageAt(i);
            std::vector<PdfTextEntry> entries;
            page->ExtractTextTo(entries);

            // Combine entries into one page string
            std::ostringstream oss;
            for (auto& entry : entries) {
                oss << entry.Text << " ";
            }
            std::string pageText = oss.str();

            // Chunk the page text
            auto pageChunks = chunkTextWithMeta(pageText, i);
            allChunks.insert(allChunks.end(), pageChunks.begin(), pageChunks.end());

            // update progress bar
            progress = static_cast<float>(i + 1) / pageCount;
        }
    }
    catch (const PdfError& err) {
        std::cerr << "PoDoFo error: " << err.what() << std::endl;
    }

    return allChunks;
}
