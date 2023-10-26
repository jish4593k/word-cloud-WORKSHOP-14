#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <iterator>
#include <sqlite3.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Function to preprocess and clean the text
std::string cleanText(std::string text) {
    // Convert to lowercase
    std::transform(text.begin(), text.end(), text.begin(), ::tolower);

    // Remove HTML tags
    text = std::regex_replace(text, std::regex("<[^>]+>"), "");

    // Tokenize the text
    std::istringstream iss(text);
    std::vector<std::string> words(std::istream_iterator<std::string>{iss},
                                   std::istream_iterator<std::string>());

    // Remove punctuation and stopwords
    std::vector<std::string> cleanedWords;
    for (const std::string &word : words) {
        if (std::find_if(word.begin(), word.end(), ::isdigit) == word.end() &&
            std::find_if(word.begin(), word.end(), ::ispunct) == word.end() &&
            std::find(stopwords.begin(), stopwords.end(), word) == stopwords.end()) {
            cleanedWords.push_back(word);
        }
    }

    return std::accumulate(cleanedWords.begin(), cleanedWords.end(), std::string(),
                           [](const std::string &a, const std::string &b) -> std::string {
                               return a + " " + b;
                           });
}

// Function to create a word cloud from text
void createWordCloud(const std::string &text) {
    std::cout << "Generating a word cloud from WeChat chat records..." << std::endl;

    // Remove HTML tags, stopwords, and punctuation
    std::string cleanedText = cleanText(text);

    // Create a WordCloud
    cv::Mat wordcloudImg(600, 800, CV_8UC3, cv::Scalar(255, 255, 255));
    cv::Ptr<cv::text::OCRHMMDecoder::ClassifierCallback> ocr = cv::text::loadOCRHMMClassifierCNN("OCRBeamSearch_CNN_model_data.xml.gz");

    // Set up other WordCloud parameters and rendering
    // ...

    // Save the word cloud image
    cv::imwrite("wordcloud.png", wordcloudImg);

    // Display the word cloud
    cv::imshow("Word Cloud", wordcloudImg);
    cv::waitKey(0);
}

// Function to extract chat records from the database
std::string getContentFromWeixin(const std::string &dbFilePath) {
    sqlite3 *db;
    int rc = sqlite3_open(dbFilePath.c_str(), &db);

    if (rc) {
        std::cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(1);
    }

    std::string content = "";
    const char *query = "SELECT Message FROM Chat_Table";  // Modify this query accordingly

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

    if (rc) {
        std::cerr << "Error preparing SQLite query: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(1);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        content += reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return content;
}

int main() {
    // Specify the path to the SQLite database
    std::string dbFilePath = "weixin.db";  // Modify this path accordingly

    // Get chat content from the database
    std::string content = getContentFromWeixin(dbFilePath);

    // Generate a word cloud from the chat records
    createWordCloud(content);

    return 0;
}
