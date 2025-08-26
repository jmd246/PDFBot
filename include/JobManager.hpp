#ifndef JOB_MANAGER
#define JOB_MANAGER
#include <future>
#include <thread>
#include <PDFParser.hpp>
#include <string>
#include <imgui.h>

struct PDFJob {
    std::future<bool> fut;
    std::unique_ptr<PDFParser> parser;
};

class JobManager {
private:
    std::vector<PDFJob> jobs;
    std::vector< std::unique_ptr<PDFParser> > parsers;
public:
    void displayProgress() ;
    void endJobs();
    void extractPDF(const std::string& filePath);
    void displayCompletedParsers();
    size_t getJobSize() const {
        return jobs.size();
    }
    size_t getParserSize() const {
        return parsers.size();
    }



};

#endif 

