#ifndef JOB_MANAGER
#define JOB_MANAGER
#include <future>
#include <thread>
#include <PDFParser.hpp>
#include <string>

struct PDFJob {
    std::future<bool> fut;
    std::unique_ptr<PDFParser> parser;
};

class JobManager {
private:
    std::vector<PDFJob> jobs;
    std::vector< std::unique_ptr<PDFParser> > parsers;
public:
    const uint8_t m_max_threads = 4;
    std::vector<PDFJob>& getJobs() {
        return jobs;
    }
    std::vector<std::unique_ptr<PDFParser>>& getParsers() {
        return parsers;
    }
    void completeJob(std::vector<PDFJob>::iterator& it);

    void endJobs();
    void extractPDF(const std::string& filePath);
    size_t getJobSize() const {
        return jobs.size();
    }

    size_t getParserSize() const {
        return parsers.size();
    }



};

#endif 

