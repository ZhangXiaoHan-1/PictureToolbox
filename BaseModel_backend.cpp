// BaseModel_backend.cpp
#define NOMINMAX
#undef min
#undef max
#include "BaseModel_backend.h"

BaseModelBackend::BaseModelBackend(std::string dir) {
    this->MODEL_DIR = dir;
    this->load();
}

std::string BaseModelBackend::get_model_dir() {
    return this->MODEL_DIR;
}

void BaseModelBackend::load() {
    this->body = torch::jit::load(this->MODEL_DIR);
}

at::Tensor BaseModelBackend::forward(at::Tensor input) {
    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(input);
    at::Tensor output = this->body.forward(inputs).toTensor();
    return output;
}

at::Tensor BaseModelBackend::forward(std::vector<torch::jit::IValue> inputs) {
    at::Tensor output = this->body.forward(inputs).toTensor();
    return output;
}
