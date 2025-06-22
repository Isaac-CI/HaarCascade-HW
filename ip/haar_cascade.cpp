#include "haar_cascade.hpp"
#include "haar_data.hpp" // Generated from XML
#include "hls_math.h"    // For fixed-point math
#include "common/xf_common.hpp"

// Process all sliding windows
void process_windows(
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& integral,
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& squared_integral,
    hls::stream<Rect_t>& faces_out,
    ap_uint<16> height,
    ap_uint<16> width)
{
    #pragma HLS DATAFLOW
    const ap_uint<16> min_size = 24; // OpenCV default minimum size

    WINDOW_ROW: for (ap_uint<16> y = 0; y <= height - min_size; y += 5) {
        #pragma HLS LOOP_TRIPCOUNT min=1 max=1024
        WINDOW_COL: for (ap_uint<16> x = 0; x <= width - min_size; x += 5) {
            #pragma HLS PIPELINE II=1
            #pragma HLS LOOP_FLATTEN OFF

            if (evaluate_window(integral, squared_integral, x, y, min_size)) {
                Rect_t rect = {x, y, min_size, min_size};
                faces_out.write(rect);
            }
        }
    }
}

// Evaluate a single window through all stages
bool evaluate_window(
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& integral,
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& squared_integral,
    ap_uint<16> x, ap_uint<16> y, ap_uint<16> size)
{
    #pragma HLS INLINE

    // 1. Compute window mean and variance (OpenCV's normalization)
    int32_t sum = integral.read((y + size) * integral.cols + (x + size))
            - integral.read(y * integral.cols + (x + size))
            - integral.read((y + size) * integral.cols + x)
            + integral.read(y * integral.cols + x);


    // Similarly for squared_integral
    int64_t sq_sum = squared_integral.read((y + size) * integral.cols + (x + size))
        - squared_integral.read(y * integral.cols + (x + size))
        - squared_integral.read((y + size) * integral.cols + x)
        + squared_integral.read(y * integral.cols + x);
    
    feature_t mean = sum / (feature_t)(size * size);
    feature_t variance = (sq_sum - sum * mean) / (feature_t)(size * size);
    variance = (variance >= 1.0) ? variance : (feature_t)1.0f; // OpenCV's behavior

    // 2. Process each stage
    STAGE_LOOP: for (int s = 0; s < NUM_STAGES; s++) {
        #pragma HLS LOOP_TRIPCOUNT min=20 max=25
        feature_t stage_sum = 0;

        WEAK_CLASSIFIER_LOOP: for (int w = 0; w < stages[s].weak_count; w++) {
            #pragma HLS PIPELINE II=1
            feature_t feature_val = compute_feature(integral, x, y, size, stages[s].classifiers[w].feature_idx);
            
            // Normalize as in OpenCV
            feature_val = (feature_val - mean) / hls::sqrt(variance);
            
            // Decision tree evaluation
            if (feature_val < stages[s].classifiers[w].threshold) {
                stage_sum += stages[s].classifiers[w].left_value;
            } else {
                stage_sum += stages[s].classifiers[w].right_value;
            }
        }

        // Early rejection
        if (stage_sum < stages[s].stage_threshold) {
            return false;
        }
    }
    return true;
}

// Compute a single Haar feature value
feature_t compute_feature(
    xf::cv::Mat<XF_32SC1, MAX_HEIGHT, MAX_WIDTH, XF_NPPC1>& integral,
    ap_uint<16> x, ap_uint<16> y, ap_uint<16> size,
    ap_uint<16> feature_idx)
{
    #pragma HLS INLINE
    
    // Get feature parameters (2 rectangles per feature)
    HaarFeature rect1 = features[feature_idx * 2];
    HaarFeature rect2 = features[feature_idx * 2 + 1];

    // Scale coordinates
    float scale = size / 24.0f; // OpenCV's base window size
    auto scale_coord = [&](ap_uint<8> coord) -> ap_uint<16> {
        return coord * scale;
    };

    // Compute rectangle sums
    auto rect_sum = [&](HaarFeature r) -> int32_t {
        ap_uint<16> x1 = x + scale_coord(r.x1);
        ap_uint<16> y1 = y + scale_coord(r.y1);
        ap_uint<16> x2 = x + scale_coord(r.x2);
        ap_uint<16> y2 = y + scale_coord(r.y2);
        
        return integral.read(y2 * integral.cols + x2)
            - integral.read(y1 * integral.cols + x2)
            - integral.read(y2 * integral.cols + x1)
            + integral.read(y1 * integral.cols + x1);
    };

    // Weighted sum as in OpenCV
    return (rect_sum(rect1) * rect1.weight) + (rect_sum(rect2) * rect2.weight);
}