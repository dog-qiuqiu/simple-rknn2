#include "simple_rknn2.h"
#include "simple_rknn2_pimpl.h"

SimpleRKNN2Pimpl::SimpleRKNN2Pimpl():pimpl(new SimpleRKNN2())
{
}

SimpleRKNN2Pimpl::~SimpleRKNN2Pimpl()
{
}

int SimpleRKNN2Pimpl::LoadModel(const char *model_path)
{
    return pimpl->LoadModel(model_path);
}

int SimpleRKNN2Pimpl::Forward(const InputImg &src_img, std::vector<FeatureMap> &dst_feature_map)
{
    return pimpl->Forward(src_img, dst_feature_map);
}