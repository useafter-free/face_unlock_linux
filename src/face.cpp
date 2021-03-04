#include "face.hpp"
#include <filesystem>
#include <fstream>
#include <chrono>

namespace fs = std::filesystem;

Face::Face(std::string model_dir_path, std::string username, std::string caffeConfigFile, std::string caffeWeightFile): model_dir_path(model_dir_path), username(username), caffeConfigFile(caffeConfigFile), caffeWeightFile(caffeWeightFile)
{
    net = cv::dnn::readNetFromCaffe(model_dir_path + caffeConfigFile, model_dir_path + caffeWeightFile);
    faceRec = face::LBPHFaceRecognizer::create();
    model_choice = 2;
}

Face::Face(std::string model_dir_path, std::string username, std::string cascadePath):model_dir_path(model_dir_path), username(username), cascadePath(cascadePath)
{
    cascade.load(model_dir_path + cascadePath);
    faceRec = face::LBPHFaceRecognizer::create();
    model_choice = 1;
}

Face::~Face() {}

Face::Face() {}


int Face::isModelAvailable(std::string username) {
    std::fstream modelStream;
    modelStream.open(model_dir_path + username + "-model.xml");
    return !modelStream.fail(); 
}

void Face::cropFace(Rect& faces, Mat& frame) {
    frame = frame(faces);
	resize(frame, frame, Size( 96, 96 ));
}

bool Face::dnnProcessing(Mat &frame) {
    std::vector<Rect> faces;
    bool max_frame_changed = false;
    int width = frame.cols;
    int height = frame.rows;
    Mat inputBlob = dnn::blobFromImage(frame, 1.0, Size(300, 300), Scalar(104.0, 177.0, 123.0), false, false);
    net.setInput(inputBlob, "data");
    Mat detection = net.forward("detection_out");
    Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
    for(int i = 0; i < detectionMat.rows; i++)
    {
        float confidence = detectionMat.at<float>(i, 2);
        if(confidence >= 0.7)
        {
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * width);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * height);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * width);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * height);
            rectangle(frame, Point(x1, y1), Point(x2, y2), Scalar(0, 255, 0), 2, 4);
            faces.push_back(Rect(Point(x1, y1), Point(x2, y2)));
            //imshow("face detection", frame);
        }
    }// dnn for

    //std::cout << "no. of faces: "<< faces.size() << "\n";

    if (faces.size() > 1 || faces.size() <= 0) 
        return false;
    else
        cropFace(faces[0], frame);
    return true;    
}

bool Face::haarProcessing(Mat &frame) {
    std::vector<Rect> faces; 

   //cascade.detectMultiScale( frame, faces, 1.1, 2);
    cascade.detectMultiScale( frame, faces, 1.05, 8, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size( 40, 40 ) );
    int face_count = faces.size();
    for (int i = 0; i < face_count; ++i) {
        cv::rectangle(frame, faces[i], cv::Scalar(255, 0, 0), 1, 8, 0);
        //imshow("haar face", frame);
    }

    if (faces.size() == 1) {
        cropFace(faces[0], frame);
        return true;
    }
    return false;
}

std::vector<Mat> Face::getFramesFromFile() 
{
    std::vector<Mat> dataset_frames;
    std::string dataset_path = model_dir_path + username + "/";
    for (const auto& file  : fs::directory_iterator(dataset_path)) {
        Mat frame1 = imread(file.path(), IMREAD_COLOR);    
        dataset_frames.push_back(frame1.clone());
    }
    return dataset_frames;
}

std::vector<Mat> Face::getFramesFromCamera(int timeout)
{
    Mat frame, temp;
    std::vector<Mat> camera_frames;
    float scale = 1.0;
    VideoCapture capture;
    bool face_found = false;
    
    
    capture.open(0);
    if (capture.isOpened()) 
    { 
        // Capture frames from video and detect faces 
        std::cout << "Trying to Authenticate....\n"; 
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        while (1)
        { 
            if(std::chrono::steady_clock::now() - start > std::chrono::seconds(timeout)) {
                std::cout << "Timeout reached\n";
                break;   
            } 
            capture >> frame; 
            if( frame.empty() ) 
                break;
            flip(frame, temp, 1);
            imshow("test frame", temp);
            waitKey(10);
            camera_frames.push_back(frame.clone());   
        }    
    }    
    return camera_frames;
}

void Face::processFrames(std::vector<Mat>& frames) 
{
    for (auto& frame : frames) {
        cvtColor(frame, frame, COLOR_BGR2GRAY);     
        flip(frame, frame, 1);
    }
}

void Face::processFrame(Mat& frame) 
{
    cvtColor(frame, frame, COLOR_BGR2GRAY);     
    flip(frame, frame, 1);
}
