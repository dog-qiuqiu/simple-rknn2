#ifndef _RKNN2_H
#define _RKNN2_H

#include<vector>

#include "type.h"
#include "im2d.h"
#include "rknn_api.h"

class SimpleRKNN2
{
private:
    int width_;
    int height_;
    int channel_;
    void *resize_buf_;
    int model_data_size_;
    unsigned char *model_data_;

    rga_buffer_t src_;
    rga_buffer_t dst_;

    rknn_context ctx_;
    rknn_input *inputs_;
    rknn_output *outputs_;
    rknn_input_output_num io_num_;
    rknn_tensor_attr *output_attrs_;
    
public:
    SimpleRKNN2();
    ~SimpleRKNN2();

    int LoadModel(const char *model_path);
    int Forward(const InputImg &src_img, std::vector<FeatureMap> &dst_feature_map);
};

#endif
