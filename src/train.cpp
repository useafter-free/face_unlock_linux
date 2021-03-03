#include "train.hpp"

Train::Train(std::string model_dir_path, std::string username, const std::string caffeConfigFile, const std::string caffeWeightFile)
: Face(model_dir_path, username, caffeConfigFile, caffeWeightFile)
{}

Train::Train(std::string model_dir_path, std::string username, const std::string cascadePath)
: Face(model_dir_path, username, cascadePath)
{}

Train::~Train()
{}


void Train::trainModel()
{
    if(isModelAvailable(username)){
        std::cout << "Model already exists!\n";
        return;
    }
    std::vector<Mat> dataset_frames = getFramesFromFile();
    
    std::vector<Mat> frames_with_faces;
    switch(model_choice) {
    case HAARCASCADE:
        processFrames(dataset_frames);
        for (auto& frame : dataset_frames) {
            if (haarProcessing(frame)) {
                frames_with_faces.push_back(frame.clone());
            }
        }
        break;
    case DNN:
        for (auto& frame : dataset_frames) {
            if (dnnProcessing(frame)) {
                frames_with_faces.push_back(frame.clone());
            }
        }
        processFrames(frames_with_faces);
        break;    
    };
    int n = frames_with_faces.size();
    std::vector<int> labels;
    for (int i = 0; i < n; ++i)
        labels.push_back(i);
    // Train
    std::cout << "no. of images collected: " << n << "\n";
    faceRec->train(frames_with_faces,  labels);
    saveModel(model_dir_path);
    std::cout << "Facial key saved.\n";
}

//should be called after trainModel has already been called
void Train::saveModel(const std::string& path)
{
	// Save model and classifier
	faceRec->save(path + username + "-model.xml");
}