#include "stdio.h"
#include "opencv2/opencv.hpp"

#include "type.h"
#include "simple_rknn2_pimpl.h"

void GetTop(const void *tensor, const int length, int &index, float &score)
{   
    float *data = (float*)tensor;

    int max_index = -1;
    float max_score = 0;    
    for (int i = 0; i < length; i++)
    {  
        if(data[i] > max_score)
        {
            max_index = i;
            max_score = data[i];
        }
    }

    index = max_index;
    score = max_score;
}   

int main()
{
    int ret = -1;
    SimpleRKNN2Pimpl rknn2;

    //加载rknn模型
    ret = rknn2.LoadModel("mobilenetv2.rknn");
    if (ret!=0) printf("gg\n");

    //读取图片
    cv::Mat cv_img = cv::imread("dog_224x224.jpg");
    InputImg input_img;
    input_img.data = cv_img.data;
    input_img.width = cv_img.cols;
    input_img.height = cv_img.rows;

    //模型推理
    std::vector<FeatureMap> dst_tensor;
    ret = rknn2.Forward(input_img, dst_tensor);

    //遍历模型输出节点
    for (int i = 0; i < dst_tensor.size(); i++)
    {
        //打印模型每个输出节点的张量结构
        printf("=========================\n");
        printf("index:%d c:%d h:%d w:%d\n", 
                i, dst_tensor[i].channel, dst_tensor[i].height, dst_tensor[i].width);

        //打印检测类别信息
        int index = -1;
        float score = -1;
        GetTop(dst_tensor[i].buf, dst_tensor[i].channel, index, score);
        printf("category:%d score:%f\n", index, score);
        printf("=========================\n");
    }

    return 0;
}

