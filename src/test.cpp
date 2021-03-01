#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <opencv2/objdetect.hpp> 
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <chrono>

#define MAX_FRAMES_FOR_TRAINING 10

using namespace cv;

std::string username;
const std::string model_dir_path = "./data/";
CascadeClassifier cascade;
dnn::Net net;
Ptr<face::FaceRecognizer> faceRec;
double lthreshold = 110;
double maxConf = 0.0;
Mat maxFrame;
Rect maxFace;

const std::string caffeConfigFile = "./data/deploy.prototxt";
const std::string caffeWeightFile = "./data/res10_300x300_ssd_iter_140000.caffemodel";


bool containsFaceAndCrop(Mat &frame) {
    std::vector<Rect> faces; 
    
   //cascade.detectMultiScale( frame, faces, 1.1, 2);
    cascade.detectMultiScale( frame, faces, 1.05, 8, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size( 40, 40 ) );
    int face_count = faces.size();
    for (int i = 0; i < face_count; ++i) {
        cv::rectangle(frame, faces[i], cv::Scalar(255, 0, 0), 1, 8, 0);
        imshow("face detected", frame);
    }

    if (faces.size() == 1) {
        frame = frame(faces[0]);
	    cv::resize(frame, frame, cv::Size( 96, 96 ));
        return true;
    }
    return false;
}




int isModelAvailable(std::string username) {
    std::fstream modelStream;
    modelStream.open(model_dir_path + username + "-model.xml");
    return !modelStream.fail(); 
}

bool dnnProcessing(Mat &frame) {
    std::vector<Rect> faces;
    bool max_frame_changed = false;
    int width = frame.size().width;
    int height = frame.size().height;
    cv::Mat inputBlob = cv::dnn::blobFromImage(frame, 1.05, cv::Size(width, height), false, true);
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
            imshow("Minor Detection", frame);
            if (faces.size() > 1) {
                return false;
            }        
            if (confidence > maxConf) {
                maxConf = confidence;
                maxFrame = frame.clone();
                maxFace = faces[0];
            }
            // maxConf = confidence > maxConf ? confidence : maxConf;
            // max_frame_changed = confidence > maxConf;
            // maxFrame = max_frame_changed ? frame : maxFrame;
            // maxFace = max_frame_changed ? faces[0] : maxFace;
        }
    }
    
     // dnn for
    // if (max_frame_changed) {
    //     cropFace(faces, maxFrame);
    // }
    //cascade.detectMultiScale( frame, faces, 1.05, 8, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size( 40, 40 ));
    return true;
}

void cropFace(Rect& faces, Mat& frame) {
    frame = frame(faces);
	cv::resize(frame, frame, cv::Size( 96, 96 ));
    imwrite("image2.jpg", frame);
}

bool Test(Mat& frame_captured) {
    double confidence = 0.0;
    int label = -1;

    faceRec = face::LBPHFaceRecognizer::create();
    faceRec->read(model_dir_path + username + "-model.xml");
    faceRec->predict(frame_captured , label, confidence);
    // conf_sum = conf_sum > confidence ? confidence : conf_sum ;
    std::cout << "confidence = " << confidence << " label: "<< label << "\n";
    if (confidence < lthreshold) {
        std::cout << "face matches\n";
        return true;
    } else {
        std::cout << "face doesn't matches\n";
        return false;
    }
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
    bool face_found = false;
    
    
    capture.open(0);
    if (capture.isOpened()) 
    { 
        // Capture frames from video and detect faces 
        std::cout << "Trying to Authenticate....\n"; 
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        while (!face_found)
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
                flip(frame1, frame1,1);
                //frames_captured.push_back(frame.clone());
            }

            if (model_choice == 1) {
                cvtColor(frame1, grayImage, COLOR_BGR2GRAY);     
                flip(grayImage, grayImage,1);
                face_found = containsFaceAndCrop(grayImage);
                if (face_found)
                    maxFrame = grayImage.clone();
            }

            char c = (char)waitKey(10); 
            // Press q to exit from window 
            if( c == 27 || c == 'q' || c == 'Q' )  
                break; 
            //imwrite("image.jpg", frames_captured[0]);
        }
        //
        if (model_choice == 2) {
            if (maxConf != 0.0) {
                cropFace(maxFace, maxFrame);
                cvtColor(maxFrame, maxFrame, COLOR_BGR2GRAY);
                Test(maxFrame);
            }
            else {
                std::cout << "No faces found...try again\n";
            }
        }
        if (model_choice == 1) {
            Test(maxFrame);
        }
         
    }    
    return 0;
}