#pragma once
#include <ap_fixed.h>

#define NUM_STAGES 25

// 1. Fixed-point types matching OpenCV's internal calculations
typedef ap_fixed<16,8> feature_t;  // Q8.8 format for thresholds/weights
typedef ap_uint<5> coord_t;        // 0-31 range (sufficient for 24x24 window)

// 2. Haar feature rectangle (matches OpenCV's struct)
struct HaarFeature {
    coord_t x1, y1, x2, y2;  // Two opposite corners
    feature_t weight;         // Signed fixed-point
};

// 3. Weak classifier (identical to OpenCV's implementation)
struct WeakClassifier {
    feature_t threshold;
    feature_t left_value;
    feature_t right_value;
    ap_uint<16> feature_idx;     // Index into features[]
};

// 4. Stage classifier
struct Stage {
    feature_t stage_threshold;
    ap_uint<8> weak_count;
    WeakClassifier classifiers[215]; // Max weak classifiers per stage
};