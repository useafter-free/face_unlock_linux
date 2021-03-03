#ifndef FACE_H
#define FACE_H
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

#define HAARCASCADE 1
#define DNN 2


using namespace cv;

class Face
{
public:
    Face();
    Face(std::string model_dir_path, std::string username, std::string caffeConfigFile, std::string caffeWeightFile); //dnn
    Face(std::string model_dir_path, std::string username, std::string cascadePath); //haar
    ~Face();
    bool dnnProcessing(Mat &frame);
    bool haarProcessing(Mat &frame);


protected:
    std::string username;
    int model_choice;
    const std::string model_dir_path;
    const std::string cascadePath;
    CascadeClassifier cascade;
    dnn::Net net;
    Ptr<face::FaceRecognizer> faceRec;
    const std::string caffeConfigFile;
    const std::string caffeWeightFile;
    std::vector<Mat> getFramesFromFile();
    std::vector<Mat> getFramesFromCamera(int timeout);
    int isModelAvailable(std::string username);
    void cropFace(Rect& faces, Mat& frame);    
    void processFrames(std::vector<Mat>& frames);
    void processFrame(Mat& frame);
};

#endif