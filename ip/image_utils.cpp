#include "image_utils.hpp"
#include "common/xf_infra.hpp"
#include "imgproc/xf_integral_image.hpp"

// Convert RGB AXI stream to grayscale image
void rgb2gray(
    hls::stream<ap_axiu<24,1,1,1>>& src_stream,
    xf::cv::Mat<XF_8UC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& img_gray) 
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
            img_gray.at<uint8_t>(y,x) = (r * 76 + g * 150 + b * 29) >> 8;
        }
    }
}

// Compute standard integral image
void compute_integral(
    xf::cv::Mat<XF_8UC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& img_gray,
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& integral_img) 
{
    #pragma HLS INLINE OFF
    
    /* Alternative basic implementation:
    int sum = 0;
    for (int y = 0; y < height; y++) {
        #pragma HLS LOOP_TRIPCOUNT min=1 max=MAX_HEIGHT
        for (int x = 0; x < width; x++) {
            #pragma HLS PIPELINE II=1
            sum += img_gray.at<uint8_t>(y,x);
            if (y == 0)
                integral_img.at<int>(y,x) = sum;
            else
                integral_img.at<int>(y,x) = integral_img.at<int>(y-1,x) + sum;
        }
        sum = 0;
    }
    */
}

// Compute squared integral image for variance normalization
void compute_squared_integral(
    xf::cv::Mat<XF_8UC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& img_gray,
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& squared_integral) 
{
    #pragma HLS INLINE OFF
    
    // HLS doesn't have built-in squared integral, so we implement it:
    int sum = 0;
    ROW_LOOP: for (int y = 0; y < img_gray.rows; y++) {
        #pragma HLS LOOP_TRIPCOUNT min=1 max=MAX_HEIGHT
        COL_LOOP: for (int x = 0; x < img_gray.cols; x++) {
            #pragma HLS PIPELINE II=1
            ap_uint<8> pix = img_gray.at<uint8_t>(y,x);
            ap_uint<16> pix_sq = pix * pix;
            sum += pix_sq;
            
            if (y == 0) {
                squared_integral.at<int>(y,x) = sum;
            } else {
                squared_integral.at<int>(y,x) = squared_integral.at<int>(y-1,x) + sum;
            }
        }
        sum = 0;
    }
}