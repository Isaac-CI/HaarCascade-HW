# Copyright (C) 2019-2022, Xilinx, Inc.
# Copyright (C) 2022-2023, Advanced Micro Devices, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# vitis hls makefile-generator v2.0.0

# source /tools/Xilinx/Vitis/2023.2/settings64.sh
# export LD_LIBRARY_PATH=/home/lsi2515/opencv_build/source/install/lib:$LD_LIBRARY_PATH

set CSIM 0
set CSYNTH 1
set COSIM 1
set VIVADO_SYN 1
set VIVADO_IMPL 1
set OPENCV_INCLUDE "/home/lsi2515/opencv_build/source/install/include/opencv4"
set OPENCV_LIB "/home/lsi2515/opencv_build/source/install/lib"
set XF_PROJ_ROOT "/home/lsi2515/Documents/Franklin/Vitis_Libraries-2023.2/vision"
set XPART xcu55c-fsvh2892-2L-e

set PROJ "haarcascadedetection.prj"
set SOLN "sol1"

if {![info exists CLKP]} {
  set CLKP 3.3
}

open_project -reset $PROJ

add_files "${XF_PROJ_ROOT}/L1/examples/haarcascadedetection/ip/face_detection.cpp" -cflags "-I${XF_PROJ_ROOT}/L1/include -I${XF_PROJ_ROOT}/L1/include/common -I./ -D__SDSVHLS__ -std=c++14" -csimflags "-I${XF_PROJ_ROOT}/L1/include -I${XF_PROJ_ROOT}/L1/include/common -I./ -D__SDSVHLS__ -std=c++14"
add_files "${XF_PROJ_ROOT}/L1/examples/haarcascadedetection/ip/haar_cascade.cpp" -cflags "-I${XF_PROJ_ROOT}/L1/include -I${XF_PROJ_ROOT}/L1/include/common -I./ -D__SDSVHLS__ -std=c++14" -csimflags "-I${XF_PROJ_ROOT}/L1/include -I${XF_PROJ_ROOT}/L1/include/common -I./ -D__SDSVHLS__ -std=c++14"
add_files -tb "${XF_PROJ_ROOT}/L1/examples/haarcascadedetection/ip/tb_face_detection.cpp" -cflags "  -I${OPENCV_INCLUDE} -I${XF_PROJ_ROOT}/L1/include -I ./ -D__SDSVHLS__ -std=c++14" -csimflags "-I${XF_PROJ_ROOT}/L1/include -I${XF_PROJ_ROOT}/L1/include/common -I./ -D__SDSVHLS__ -std=c++14"
set_top face_detection_ip

open_solution -reset $SOLN

set_part $XPART
create_clock -period $CLKP

if {$CSIM == 1} {
    csim_design -ldflags "-L ${OPENCV_LIB} \
                           -lopencv_imgcodecs \
                           -lopencv_imgproc \
                           -lopencv_calib3d \
                           -lopencv_core \
                           -lopencv_highgui \
                           -lopencv_flann \
                           -lopencv_features2d" \
                -argv "${XF_PROJ_ROOT}/data/512x512.png" \
}

# Síntese
if {$CSYNTH == 1} {
    csynth_design
}

# Co-simulação
if {$COSIM == 1} {
    cosim_design -ldflags "-L ${OPENCV_LIB} \
                            -lopencv_imgcodecs \
                            -lopencv_imgproc \
                            -lopencv_calib3d \
                            -lopencv_core \
                            -lopencv_highgui \
                            -lopencv_flann \
                            -lopencv_features2d" \
                 -argv "${XF_PROJ_ROOT}/data/512x512.png"
}

# Exportação da síntese e implementação
if {$VIVADO_SYN == 1} {
    export_design -flow syn -rtl verilog
}

if {$VIVADO_IMPL == 1} {
    export_design -flow impl -rtl verilog
}

exit