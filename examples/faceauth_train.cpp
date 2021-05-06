#include <faceauth/train.hpp>
#include <unistd.h>
#include <readline/readline.h>

int main(int argc, char **argv)
{
    if (getuid() != 0) {
        std::cout << "You need to be root user to train model! \nexiting.....\n" ;
        exit(1);
    }

    std::string username, modelPath, datasetPath;
    std::cout << "Username: ";
    std::cin >> username;
    modelPath = "/etc/faceauth_data/";
    datasetPath = readline("Enter path to images directory:");
    Train *obj;
    if (std::stoi(argv[1]) == HAARCASCADE) {
        obj = new Train(modelPath, datasetPath, username, "haarcascade_frontalface_alt.xml");
    } else if (std::stoi(argv[1]) == DNN) {
        obj = new Train(modelPath, datasetPath, username, "deploy.prototxt", "res10_300x300_ssd_iter_140000_fp16.caffemodel");
    }

    obj->trainModel();

    return 0;
}