#include "face_detection.hpp"
#include "haar_cascade.hpp"
#include "imgproc/xf_cvt_color.hpp"
#include "imgproc/xf_integral_image.hpp"
#include "common/xf_infra.hpp"

void face_detection_ip(
    hls::stream<ap_axiu<24,1,1,1>>& video_in,
    hls::stream<Rect_t>& faces_out,
    ap_uint<16> height,
    ap_uint<16> width)
{
#pragma HLS INTERFACE axis port=video_in
#pragma HLS INTERFACE axis port=faces_out
#pragma HLS INTERFACE ap_ctrl_none port=return

    // Tipicamente, altura e largura máximas são definidas por constantes em xf_config_params.hpp
    xf::cv::Mat<XF_8UC3, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1> img_rgb(height, width);
    xf::cv::Mat<XF_8UC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1> img_gray(height, width);
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1> integral(height, width);
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1> squared_integral(height, width);

    // Conversão de AXI para Mat
    xf::cv::AXIvideo2xfMat(video_in, img_rgb);

    // Conversão para grayscale
    xf::cv::rgb2gray(img_rgb, img_gray);

    // Integral image
    xf::cv::integral<XF_8UC1, XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>(img_gray, integral);

    // Detecção de janelas
    process_windows(integral, squared_integral, faces_out, height, width);
}

// Convert RGB AXI stream to grayscale image
/*void rgb2gray(
    hls::stream<ap_axiu<24,1,1,1>>& src_stream,
    xf::cv::Mat<XF_8UC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& img_gray,
    ap_uint<16> height,
    ap_uint<16> width)
{
    #pragma HLS INLINE OFF
    #pragma HLS DATAFLOW

    for (int y = 0; y < height; y++) {
        #pragma HLS LOOP_TRIPCOUNT min=1 max=MAX_HEIGHT
        for (int x = 0; x < width; x++) {
            #pragma HLS PIPELINE II=1
            ap_axiu<24,1,1,1> pix = src_stream.read();
            ap_uint<8> r = pix.data(7,0);
            ap_uint<8> g = pix.data(15,8);
            ap_uint<8> b = pix.data(23,16);
            //img_gray.at<uint8_t>(y,x) = (r * 76 + g * 150 + b * 29) >> 8;
            img_gray.write((y * img_gray.cols + x), ((r * 76 + g * 150 + b * 29) >> 8));
        }
    }
}*/