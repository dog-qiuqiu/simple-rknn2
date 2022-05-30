#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rga.h"
#include "simple_rknn2.h"
//#include "opencv2/opencv.hpp"

static void dump_tensor_attr(rknn_tensor_attr *attr)
{
    printf("  index=%d, name=%s, n_dims=%d, dims=[%d, %d, %d, %d], n_elems=%d, size=%d, fmt=%s, type=%s, qnt_type=%s, "
           "zp=%d, scale=%f\n",
           attr->index, attr->name, attr->n_dims, attr->dims[0], attr->dims[1], attr->dims[2], attr->dims[3],
           attr->n_elems, attr->size, get_format_string(attr->fmt), get_type_string(attr->type),
           get_qnt_type_string(attr->qnt_type), attr->zp, attr->scale);
}

static unsigned char *load_data(FILE *fp, size_t ofst, size_t sz)
{
    unsigned char *data;
    int ret;

    data = NULL;

    if (NULL == fp)
    {
        return NULL;
    }

    ret = fseek(fp, ofst, SEEK_SET);
    if (ret != 0)
    {
        printf("blob seek failure.\n");
        return NULL;
    }

    data = (unsigned char *)malloc(sz);
    if (data == NULL)
    {
        printf("buffer malloc failure.\n");
        return NULL;
    }
    ret = fread(data, 1, sz, fp);
    return data;
}

static unsigned char *load_model(const char *filename, int *model_size)
{

    FILE *fp;
    unsigned char *data;

    fp = fopen(filename, "rb");
    if (NULL == fp)
    {
        printf("Open file %s failed.\n", filename);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);

    data = load_data(fp, 0, size);

    fclose(fp);

    *model_size = size;
    return data;
}

SimpleRKNN2::SimpleRKNN2(/* args */)
{
    width_ = 0;
    height_ = 0;
    channel_ = 3;
    model_data_size_ = 0;

    memset(&src_, 0, sizeof(src_));
    memset(&dst_, 0, sizeof(dst_));
}

SimpleRKNN2::~SimpleRKNN2()
{
    int ret = -1;
    ret = rknn_destroy(ctx_);

    for (int i = 0; i < io_num_.n_output; i++)
    {
        if (outputs_[i].buf)
        {
            free(outputs_[i].buf);
            outputs_[i].buf = nullptr;
        }
    }
    if (inputs_)
    {
        free(inputs_);
        inputs_ = nullptr;
    }
    if (outputs_)
    {
        free(outputs_);
        outputs_ = nullptr;
    }

    if (model_data_)
    {
        free(model_data_);
        model_data_ = nullptr;
    }
    if (resize_buf_)
    {
        free(resize_buf_);
        resize_buf_ = nullptr;
    }
    if (output_attrs_)
    {
        free(output_attrs_);
        output_attrs_ = nullptr;
    }
}

int SimpleRKNN2::LoadModel(const char *model_path)
{
    int ret = -1;

    /* Create the neural network */
    model_data_ = load_model(model_path, &model_data_size_);
    ret = rknn_init(&ctx_, model_data_, model_data_size_, 0, NULL);
    if (ret < 0)
    {
        printf("rknn_init error ret=%d\n", ret);
        return -1;
    }

    /*Query rknn sdk version*/
    rknn_sdk_version version;
    ret = rknn_query(ctx_, RKNN_QUERY_SDK_VERSION, &version,
                     sizeof(rknn_sdk_version));
    if (ret < 0)
    {
        printf("rknn_init error ret=%d\n", ret);
        return -1;
    }
    printf("sdk version: %s driver version: %s\n", version.api_version,
           version.drv_version);
    
    /*Query the number of model input and output nodes*/
    ret = rknn_query(ctx_, RKNN_QUERY_IN_OUT_NUM, &io_num_, sizeof(io_num_));
    if (ret < 0)
    {
        printf("rknn_init error ret=%d\n", ret);
        return -1;
    }
    printf("model input num: %d, output num: %d\n", io_num_.n_input,
           io_num_.n_output);

    /*Query the information of input and output tensors of the model*/
    rknn_tensor_attr input_attrs[io_num_.n_input];
    memset(input_attrs, 0, sizeof(input_attrs));
    for (int i = 0; i < io_num_.n_input; i++)
    {
        input_attrs[i].index = i;
        ret = rknn_query(ctx_, RKNN_QUERY_INPUT_ATTR, &(input_attrs[i]),
                         sizeof(rknn_tensor_attr));
        if (ret < 0)
        {
            printf("rknn_init error ret=%d\n", ret);
            return -1;
        }
        dump_tensor_attr(&(input_attrs[i]));
    }
    output_attrs_ = (rknn_tensor_attr*)malloc(sizeof(rknn_tensor_attr)*io_num_.n_output);
    //rknn_tensor_attr output_attrs[io_num_.n_output];
    memset(output_attrs_, 0, sizeof(rknn_tensor_attr)*io_num_.n_output);
    for (int i = 0; i < io_num_.n_output; i++)
    {
        output_attrs_[i].index = i;
        ret = rknn_query(ctx_, RKNN_QUERY_OUTPUT_ATTR, &(output_attrs_[i]),
                         sizeof(rknn_tensor_attr));
        dump_tensor_attr(&(output_attrs_[i]));
    }

    if (input_attrs[0].fmt == RKNN_TENSOR_NCHW)
    {
        printf("model is NCHW input fmt\n");
        channel_ = input_attrs[0].dims[1];
        height_ = input_attrs[0].dims[2];
        width_ = input_attrs[0].dims[3];
    }
    else
    {
        printf("model is NHWC input fmt\n");
        height_ = input_attrs[0].dims[1];
        width_ = input_attrs[0].dims[2];
        channel_ = input_attrs[0].dims[3];
    }

    printf("model input height=%d, width=%d, channel=%d\n", height_, width_,
           channel_);    


    /*Init input tensor buffer struct*/
    inputs_ = (rknn_input*)malloc(sizeof(rknn_input)*1);
    memset(inputs_, 0, sizeof(rknn_input)*1);

    inputs_[0].index = 0;
    inputs_[0].type = RKNN_TENSOR_UINT8;
    inputs_[0].size = width_ * height_ * channel_;
    inputs_[0].fmt = RKNN_TENSOR_NHWC;
    inputs_[0].pass_through = 0;

    resize_buf_ = malloc(height_ * width_ * channel_);

    /*Init output tensor buffer struct*/
    outputs_ = (rknn_output*)malloc(sizeof(rknn_output)*io_num_.n_output);
    memset(outputs_, 0, sizeof(rknn_output)*io_num_.n_output);

    /*Convert model tensor to float*/
    for (int i = 0; i < io_num_.n_output; i++)
    {
        outputs_[i].want_float = 1;
        //用户自行申请和释放
        outputs_[i].is_prealloc = 1;
        //申请output buffer
        int out_channel = output_attrs_[i].dims[1];
        int out_height = output_attrs_[i].dims[2];
        int out_width = output_attrs_[i].dims[3];

        outputs_[i].buf = malloc(sizeof(float) * out_channel * out_height * out_width);
    }

    return ret;
}

int SimpleRKNN2::Forward(const InputImg &src_img, std::vector<FeatureMap> &dst_feature_map)
{   
    int ret = -1;
   
    int img_width = src_img.width;
    int img_height = src_img.height;
    /*rga resize*/
    src_ = wrapbuffer_virtualaddr((void *)src_img.data, img_width, img_height, RK_FORMAT_RGB_888);
    dst_ = wrapbuffer_virtualaddr((void *)resize_buf_, width_, height_, RK_FORMAT_RGB_888);

    im_rect src_rect;
    im_rect dst_rect;
    memset(&src_rect, 0, sizeof(src_rect));
    memset(&dst_rect, 0, sizeof(dst_rect));
    ret = imcheck(src_, dst_, src_rect, dst_rect);
    if (IM_STATUS_NOERROR != ret)
    {
        printf("%d, check error! %s", __LINE__, imStrError((IM_STATUS)ret));
        return -1;
    }
    IM_STATUS STATUS = imresize(src_, dst_);

    /*rga resize debug*/
    // cv::Mat resize_img(cv::Size(width_, height_), CV_8UC3, resize_buf_);
    // cv::imwrite("resize_input.jpg", resize_img);

    /*Set input tensor*/
    inputs_[0].buf = resize_buf_;
    rknn_inputs_set(ctx_, io_num_.n_input, inputs_);

    /*run forward*/
    ret = rknn_run(ctx_, NULL);
    ret = rknn_outputs_get(ctx_, io_num_.n_output, outputs_, NULL);

    dst_feature_map.clear();
    dst_feature_map.resize(io_num_.n_output);

    for (int i = 0; i < io_num_.n_output; i++)
    {
        // NCHW
        dst_feature_map[i].channel = output_attrs_[i].dims[1];
        dst_feature_map[i].height = output_attrs_[i].dims[2];
        dst_feature_map[i].width = output_attrs_[i].dims[3];

        dst_feature_map[i].buf = (int8_t *)outputs_[i].buf;
    }

    return ret;
}