#ifndef TEST_H
#define TEST_H

#include "face.hpp"

#define MAX_FRAMES_FOR_TRAINING 10

class Test : public Face { 
private:
    double lthreshold;
    int timeout;
public:
    Test();
    Test(double lthreshold, int timeout, std::string model_dir_path, std::string username, std::string caffeConfigFile, std::string caffeWeightFile);
    Test(double lthreshold, int timeout, std::string model_dir_path, std::string username, std::string cascadePath);
    ~Test();
    bool testFace(Mat& frame);
    bool testFaces();
};

#endif