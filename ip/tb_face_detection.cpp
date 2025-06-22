#include "face_detection.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input image>\n", argv[0]);
        return -1;
    }

    cv::Mat frame = cv::imread(argv[1], 0);
    if (frame.empty()) {
        std::cerr << "Error loading image!" << std::endl;
        return -1;
    }

    hls::stream<ap_axiu<24,1,1,1>> src_stream;
    hls::stream<Rect_t> faces_stream;
    
    // Convert OpenCV image to AXI stream
    for (int y = 0; y < frame.rows; y++) {
        for (int x = 0; x < frame.cols; x++) {
            ap_axiu<24,1,1,1> pix;
            cv::Vec3b color = frame.at<cv::Vec3b>(y,x);
            pix.data = (color[2] << 16) | (color[1] << 8) | color[0];
            src_stream.write(pix);
        }
    }

    // Run IP
    face_detection_ip(src_stream, faces_stream, frame.rows, frame.cols);

    // Display results
    while (!faces_stream.empty()) {
        Rect_t rect = faces_stream.read();
        cv::rectangle(frame, 
                     cv::Point(rect.x, rect.y), 
                     cv::Point(rect.x + rect.width, rect.y + rect.height),
                     cv::Scalar(0,255,0), 2);
    }

    cv::imwrite("result.jpg", frame);
    return 0;
}