#include "stdio.h"
#include <sys/time.h>
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

int main(int argc, char * argv[])
{
	if (argc != 2)
	{
		printf("You should use specify rknn model file!\n");
		return -1;
	}
    printf("Load model:%s\n", argv[1]);
    
    int ret = -1;
    SimpleRKNN2Pimpl rknn2;

    //加载rknn模型
    ret = rknn2.LoadModel(argv[1]);
    if (ret!=0) printf("gg\n");

    //读取图片
    cv::Mat cv_img = cv::imread("dog_224x224.jpg");
    InputImg input_img;
    input_img.data = cv_img.data;
    input_img.width = cv_img.cols;
    input_img.height = cv_img.rows;

    //模型推理
    std::vector<FeatureMap> dst_tensor;
    struct timeval t1,t2;
    gettimeofday(&t1, NULL);
    ret = rknn2.Forward(input_img, dst_tensor);
    gettimeofday(&t2, NULL);
    float time_use=(t2.tv_sec-t1.tv_sec)*1000000+(t2.tv_usec-t1.tv_usec);
    printf("Forward time:%.10f ms\n", time_use / 1000);

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

