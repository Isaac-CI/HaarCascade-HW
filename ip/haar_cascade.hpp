#pragma once
#include "haar_params.hpp"
#include "common/xf_common.hpp"  // Adicionar
#include "common/xf_utility.hpp" // Adicionar
#include "hls_stream.h"          // Adicionar
#include "face_detection.hpp"    // Adicionar para Rect_t

// Usar namespace consistente (xf::cv ou hls)
void process_windows(
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& integral,
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& squared_integral,
    hls::stream<Rect_t>& faces_out,
    ap_uint<16> height,
    ap_uint<16> width);

bool evaluate_window(
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& integral,
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& squared_integral,
    ap_uint<16> x, ap_uint<16> y, ap_uint<16> size);

feature_t compute_feature(
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& integral,
    ap_uint<16> x, ap_uint<16> y, ap_uint<16> size,
    ap_uint<16> feature_idx);