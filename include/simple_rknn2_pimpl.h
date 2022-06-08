#ifndef _SIMPLE_RKNN2_PIMPL_
#define _SIMPLE_RKNN2_PIMPL_

#include "type.h"
#include <vector>
#include <memory>

/***************函数接口*******************/
class SimpleRKNN2;
class SimpleRKNN2Pimpl
{
private:
    std::unique_ptr<SimpleRKNN2> pimpl;
public:
    SimpleRKNN2Pimpl();
    ~SimpleRKNN2Pimpl();

    int LoadModel(const char *model_path);
    int Forward(const InputImg &src_img, std::vector<FeatureMap> &dst_feature_map);
};

#endif