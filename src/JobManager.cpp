#include <JobManager.hpp>


void JobManager::extractPDF(const std::string& filePath) {
    PDFJob job;
    job.parser = std::make_unique<PDFParser>();
    job.fut = std::async(std::launch::async, [filePath, parser = job.parser.get()]() -> bool {
        parser->extractTextChunks(filePath);
        return true; // dummy return
    });

    jobs.push_back(std::move(job));
    std::cout << "Selected file: " << filePath << std::endl;
}

void JobManager::completeJob(std::vector<PDFJob>::iterator& it) {
    it->fut.get();
    it->parser->extractionDone = true;
    parsers.push_back(std::move(it->parser));
    it = jobs.erase(it);
}
void JobManager::endJobs() {
    for (auto& job : jobs) {
        job.parser->stopFlag = true;
    }
}

