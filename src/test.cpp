#include "test.hpp"
#include <chrono>
#include <iomanip>
#include <numeric>

Test::Test(double lthreshold, int timeout, std::string model_dir_path, std::string username, std::string caffeConfigFile, std::string caffeWeightFile)
: lthreshold(lthreshold), timeout(timeout), Face(model_dir_path, username, caffeConfigFile, caffeWeightFile)
{}

Test::Test(double lthreshold, int timeout, std::string model_dir_path, std::string username, std::string cascadePath)
: lthreshold(lthreshold), timeout(timeout), Face(model_dir_path, username, cascadePath)
{}

Test::~Test()
{}

//PAM
bool Test::testFaces() 
{
    faceRec->read(model_dir_path + username + "-model.xml");
    std::vector<Mat> camera_frames = getFramesFromCamera(timeout);
    std::chrono::steady_clock::time_point start_test = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_haar;
    int num_faces = 0;
    int found_count = 0;
    switch(model_choice) {
    case HAARCASCADE:
        processFrames(camera_frames);
        for (auto& frame : camera_frames) {
            if (haarProcessing(frame)) {
                ++num_faces;
                if (testFace(frame))
                    ++found_count;
            }
        }
        time_haar = (std::chrono::steady_clock::now() - start_test);
        //std::time_t ttp = std::chrono::system_clock::to_time_t(time_haar);
        std::cout << "time for haar: " << time_haar.count() << "\n";
        break;
    case DNN:
        for (auto& frame : camera_frames) {
            if (dnnProcessing(frame)) {
                ++num_faces;
                processFrame(frame);
                if (testFace(frame))
                    ++found_count;
            }
        }
        break;
    };
    if (num_faces == 0)
        return false;

    std::cout << found_count << "\n" << num_faces << "\n";
    std::cout << "ratio of success: " << (static_cast<double>(found_count) / static_cast<double>(num_faces)) << "\n";
    return (static_cast<double>(found_count) / static_cast<double>(num_faces)) > 0.65 ; 
}

bool Test::testFace(Mat& frame_captured) 
{
    double confidence = 0.0;
    int label = -1;
    faceRec->predict(frame_captured , label, confidence);
    //std::cout << "confidence = " << confidence << " label: "<< label << "\n";
    if (confidence < lthreshold) {
        //std::cout << "face matches\n";
        return true;
    } else {
        //std::cout << "face doesn't matches\n";
        return false;
    }
}
