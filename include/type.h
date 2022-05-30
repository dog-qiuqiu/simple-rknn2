#ifndef _TYPE_H
#define _TYPE_H

typedef struct _FeatureMap {
    int channel;
    int width;
    int height;
    const void *buf;
} FeatureMap;

//输入图像
typedef struct _InputImg {
    unsigned char *data;    //图像buffer的地址
    int width;              //输入图像的宽
    int height;             //输入图像的高
} InputImg;

#endif