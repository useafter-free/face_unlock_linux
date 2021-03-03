#include <faceauth/train.hpp>


int main(int argc, char **argv)
{
    std::string username, modelPath;
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Enter model path: ";
    std::cin >> modelPath;
     Train *obj;
    if (std::stoi(argv[1]) == HAARCASCADE) {
        obj = new Train(modelPath, username, "haarcascade_frontalface_alt.xml");
    } else if (std::stoi(argv[1]) == DNN) {
        obj = new Train(modelPath, username, "deploy.prototxt", "res10_300x300_ssd_iter_140000_fp16.caffemodel");
    }

    obj->trainModel();

    return 0;
}