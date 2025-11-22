#pragma once

#undef slots
#include <torch/script.h>
#define slots Q_SLOTS

#include <opencv2/opencv.hpp>
#define NOMINMAX

#include "BaseModel.h"

namespace Backend {
    cv::Mat tensor2Mat(at::Tensor& t);
    at::Tensor patchForward(at::Tensor input, BaseModel net);
}
