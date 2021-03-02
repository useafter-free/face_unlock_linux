#include "test.hpp"

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
    std::vector<Mat> camera_frames = getFramesFromCamera(timeout);
    processFrames(camera_frames);
    int num_faces = 0;
    int found_count = 0;        
    switch(model_choice) {
    case HAARCASCADE:
        for (auto& frame : camera_frames) {
            if (haarProcessing(frame)) {
                ++num_faces;
                if (testFace(frame))
                    ++found_count;
            }
        } // nun roti khayenge, naali me nhi bahayenge, sperm daan kro, accha lagta hai!
        break;
    case DNN:
        for (auto& frame : camera_frames) {
            if (dnnProcessing(frame)) {
                ++num_faces;
                if (testFace(frame))
                    ++found_count;
            }
        }
        break;
    };
    if (num_faces == 0)
        return false;
    return (static_cast<double>(found_count) / static_cast<double>(num_faces)) > 0.8 ; 
}

bool Test::testFace(Mat& frame_captured) 
{
    double confidence = 0.0;
    int label = -1;
    faceRec->read(model_dir_path + username + "-model.xml");
    faceRec->predict(frame_captured , label, confidence);
    std::cout << "confidence = " << confidence << " label: "<< label << "\n";
    if (confidence < lthreshold) {
        std::cout << "face matches\n";
        return true;
    } else {
        std::cout << "face doesn't matches\n";
        return false;
    }
}