#include "GstVideoCap/GstVideoCap.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cout << "Usage: ./gstreamer_cxx <path to videofile> <width> <height>\n";
        return -1;
    } 

    GstVideoCap cap = GstVideoCap(std::string(argv[1]), atoi(argv[2]), atoi(argv[3]));
    cap.start();

    return 0;
}