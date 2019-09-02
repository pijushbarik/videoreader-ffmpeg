#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "reader/videoreader.hpp"

int main(int argc, char **argv) {
    if(argc < 2) {
        std::cerr <<
            "Usage: " <<
            argv[0] <<
            " <video input>\n";
        exit(EXIT_FAILURE);
    }

    std::string input(argv[1]);
    VideoReader reader(input);
    
    if(!reader.isOpened()) {
        std::cerr << 
            "Unable to open video from \"" <<
            argv[1] <<
            "\"\n";
        exit(EXIT_FAILURE);
    }

    cv::Mat mat;

    int framecount = 0;

    while(true) {
        if(!reader.read(mat)) break;
        cv::imshow("Output", mat);
        if(cv::waitKey(10) > 0) break;
        std::cout << ++framecount << "\r" << std::flush;
    }

    std::cout << "Number of frames processed: " << framecount << "\n";
    
    return EXIT_SUCCESS;
}