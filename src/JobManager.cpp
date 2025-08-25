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

void UIProgressBar(bool completed, float progress) {
    if (!completed) {
        ImGui::SetNextWindowSize(ImVec2(800, 600), 0);
        ImGui::Begin("PDF Processing");
        ImGui::Text("Extracting PDF ...");
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        ImGui::End();
    }
}
void JobManager::endJobs() {
    for (auto& job : jobs) {
        job.parser.get()->stopFlag = true;
    }
}
void JobManager::displayProgress() {
    //display progress of active jobs
    for (auto iterator = jobs.begin(); iterator != jobs.end(); ) {
        if (iterator->fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            iterator->fut.get(); // retrieve dummy result
            iterator->parser->extractionDone = true;

            // Optionally store parser somewhere else before erasing
            parsers.push_back(std::move(iterator->parser));


            iterator = jobs.erase(iterator); // remove job from active queue
        }
        else {
            UIProgressBar(iterator->parser->extractionDone, iterator->parser->progress);
            ++iterator;
        }
    }
}
void JobManager::displayCompletedParsers() {
    ImGui::Begin("PDF Processed");
    for (auto& parser : parsers ) {
        ImGui::Text("Extracting PDF Complete!");
    }
    ImGui::End();

}

