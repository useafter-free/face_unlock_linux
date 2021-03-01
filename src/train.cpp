#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <opencv2/objdetect.hpp> 
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <string>
#include <chrono>

#define MAX_FRAMES_FOR_TRAINING 10

namespace fs = std::filesystem;
using namespace cv;
/*
to do:
1.class Test
i/p : username, face (via webcam), username-model.xml
o/p: face matches or not
*/

std::string username;
const std::string model_dir_path = "./data/";
CascadeClassifier cascade;
Ptr<face::FaceRecognizer> faceRec;
dnn::Net net;
double maxConf = 0.0;
Mat maxMat;
//
const std::string caffeConfigFile = "./data/deploy.prototxt";
const std::string caffeWeightFile = "./data/res10_300x300_ssd_iter_140000.caffemodel";


int isModelAvailable(std::string username) {
    
    std::fstream modelStream;
    modelStream.open(model_dir_path + username + "-model.xml");

    return !modelStream.fail();
 
}

bool containsFaceAndCrop(Mat &frame) {
    std::vector<Rect> faces; 
    
   //cascade.detectMultiScale( frame, faces, 1.1, 2);
    cascade.detectMultiScale( frame, faces, 1.05, 8, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size( 40, 40 ) );
    int face_count = faces.size();
    for (int i = 0; i < face_count; ++i) {
        cv::rectangle(frame, faces[i], cv::Scalar(255, 0, 0), 1, 8, 0);
        imwrite("face_detected.jpeg", frame);
    }

    if (faces.size() == 1) {
        frame = frame(faces[0]);
	    cv::resize(frame, frame, cv::Size( 96, 96 ));
        return true;
    }
    return false;
}

void cropFace(Rect& faces, Mat& frame) {
    frame = frame(faces);
	cv::resize(frame, frame, cv::Size( 96, 96 ));
    //imwrite("image2.jpg", frame);
}

bool dnnProcessing(Mat &frame) {
    std::vector<Rect> faces;
    bool max_frame_changed = false;
    int width = frame.size().width;
    int height = frame.size().height;
    std::cout << "DNN Processing...\n";
    cv::Mat inputBlob = cv::dnn::blobFromImage(frame, 1.1, cv::Size(width, height), false, true);
    net.setInput(inputBlob, "data");
    cv::Mat detection = net.forward("detection_out");
    cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
    for(int i = 0; i < detectionMat.rows; i++)
    {
        float confidence = detectionMat.at<float>(i, 2);
        if(confidence >= 0.5)
        {
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * width);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * height);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * width);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * height);
            cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 0, 0), 1, 8, 0);
            faces.push_back(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)));
            std::cout << "faces detected possibly\n";
            imshow("face detection", frame);
            if (faces.size() > 1) {
                return false;
            }        
            // if (confidence > maxConf) {
            //     maxConf = confidence;
            //     max_frame_changed = true;
                
            //     // bc return krade true if face is better than old face, aur agar true hoga to main me push_back kr
            //     // aur frames ham by reference bhej rhe hai, to koi issue nhi hoga
            // }
            // maxConf = confidence > maxConf ? confidence : maxConf;
            // max_frame_changed = confidence > maxConf;
            // maxFrame = max_frame_changed ? frame : maxFrame;
            // maxFace = max_frame_changed ? faces[0] : maxFace;
        }
    }// dnn for
    if (faces.size() == 1)
        cropFace(faces[0], frame);
      
     
    // if (max_frame_changed) {
    //     cropFace(faces[0], frame);
    //     return true;
    //     // nani nani nani
    // } //
    //cascade.detectMultiScale( frame, faces, 1.05, 8, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size( 40, 40 ));
    return true;
}


void saveModel(const std::string& path) {
	// Save actual model and classifier
	faceRec->save(path + username + "-model.xml");
}

void Train(std::vector<Mat>& frames_captured) {

    faceRec = face::LBPHFaceRecognizer::create();
    int n = frames_captured.size();
    std::vector<int> labels;
    for (int i = 0; i < n; ++i)
        labels.push_back(i);
    // Train
    std::cout << "no. of images collected: " << n << "\n";
    faceRec->train(frames_captured,  labels);
    std::cout << "Facial key saved.\n";
}

int main(int argc, char** argv) {
    Mat frame, grayImage;
    // (frame, rect)
    std::vector<Mat> frames_captured;
    std::cout << "Enter Username: ";
    std::cin >> username;
     
    int model_choice = 1;
    std::cout << "Choose Classifier for face detection\n";
    std::cout << "Enter 1 for Haar Cascade and 2 for DNN Classifier\n";
    std::cin >> model_choice;
    switch (model_choice) {
        case 1:             // Haar
        cascade.load(model_dir_path + "haarcascade_frontalface_alt.xml" );
        break;
        case 2:
        net = cv::dnn::readNetFromCaffe(caffeConfigFile, caffeWeightFile);
        break;
        case 0: // default
        std::cout << "Wrong choice of classfier\n";
    };
    float scale = 1.0;
    VideoCapture capture;
    //cascade.load( model_dir_path + "haarcascade_frontalface_alt.xml");
    
    
    if (isModelAvailable(username)){
        // exit, or improve the model?
        std::cout << "Model already trained for user = " << username << "\n";
        return 0;
    }
    else {
        std::cout << "model doesn't exists for " << username << "\n";
        std::cout << "Capturing facial data for " << username << ".....\n";
        
        bool read_from_file = true;
        
        if (read_from_file) {
            std::string dataset_path = "./data/ayushd/";
            for (const auto& file  : fs::directory_iterator(dataset_path)) {
                Mat frame1 = imread(file.path(), IMREAD_COLOR);
                if (model_choice == 2 && dnnProcessing(frame1)) {
                    cvtColor(frame1, grayImage, COLOR_BGR2GRAY);     
                    flip(grayImage, grayImage,1);
                    std::cout << "frame captured\n";
                    frames_captured.push_back(grayImage.clone());
                }
                if (model_choice == 1) {
                    cvtColor(frame1, grayImage, COLOR_BGR2GRAY);     
                    flip(grayImage, grayImage,1);
                    if (containsFaceAndCrop(grayImage)) {
                    std::cout << "frame captured\n";
                    frames_captured.push_back(grayImage.clone());    
                    }
                }   
            }
        }
        else {
            capture.open(0);
        if (capture.isOpened()) 
        { 
            // Capture frames from video and detect faces 
            std::cout << "Face Detection Started....\n"; 
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            while (1)
            { 
                if(std::chrono::steady_clock::now() - start > std::chrono::seconds(10)) {
                    std::cout << "Timeout reached\n";
                    break;   
                } 
                capture >> frame; 
                if( frame.empty() ) 
                    break;
                    
                
                Mat frame1 = frame.clone();
                if (model_choice == 2 && dnnProcessing(frame1)) {
                    cvtColor(frame1, grayImage, COLOR_BGR2GRAY);     
                    flip(grayImage, grayImage,1);
                    std::cout << "frame captured\n";
                    frames_captured.push_back(grayImage.clone());
                    
                    // std::stringstream imgname;
                    // imgname << "train/" << username << "/image_" << i << ".jpg";  
                    // ++i;
                    // imwrite(imgname.str(), grayImage);
                
                }
                if (model_choice == 1) {
                    cvtColor(frame1, grayImage, COLOR_BGR2GRAY);     
                    flip(grayImage, grayImage,1);
                    if (containsFaceAndCrop(grayImage)) {
                    std::cout << "frame captured\n";
                    frames_captured.push_back(grayImage.clone());    
                    }
                }
                //detectAndDraw( frame1, cascade, scale );  
                //startWindowThread();                
                

                char c = (char)waitKey(10); 
                // Press q to exit from window 
                if( c == 27 || c == 'q' || c == 'Q' )  
                    break; 

            }
        }
        }
        if (frames_captured.size()) {
            Train(frames_captured);
            saveModel(model_dir_path);
        }
    }
    return 0;
}