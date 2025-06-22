#ifndef FACE_DETECTION_HPP
#define FACE_DETECTION_HPP

#pragma once

#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "haar_params.hpp"
#include "common/xf_common.hpp"
#include "common/xf_utility.hpp"

#define MAX_WIDTH 1920
#define MAX_HEIGHT 1080
#define MAX_FACES 10

struct Rect_t {
    ap_uint<16> x, y, width, height;
};

void face_detection_ip(
    hls::stream<ap_axiu<24,1,1,1>>& video_in,
    hls::stream<Rect_t>& faces_out,
    ap_uint<16> height,
    ap_uint<16> width);

#endif