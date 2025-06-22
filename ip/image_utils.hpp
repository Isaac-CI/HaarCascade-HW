#pragma once
#include "common/xf_common.hpp"
#include "ap_int.h"

#define MAX_HEIGHT 1080
#define MAX_WIDTH  1920

// RGB to Grayscale conversion
void rgb2gray(
    hls::stream<ap_axiu<24,1,1,1>>& src_stream,
    xf::cv::Mat<XF_8UC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& img_gray);

void compute_integral(
    xf::cv::Mat<XF_8UC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& img_gray,
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& integral_img);

void compute_squared_integral(
    xf::cv::Mat<XF_8UC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& img_gray,
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& squared_integral);