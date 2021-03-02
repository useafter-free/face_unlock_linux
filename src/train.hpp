#ifndef TRAIN_H
#define TRAIN_H

#include "face.hpp"

class Train : public Face
{
public:
    Train(std::string model_dir_path, int timeout, std::string username, const std::string caffeConfigFile, const std::string caffeWeightFile);
    Train(std::string model_dir_path, int timeout, std::string username, const std::string cascadePath);
    ~Train();
    void trainModel();
    void saveModel(const std::string& path);
};
#endif