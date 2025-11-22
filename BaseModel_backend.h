// BaseModel_backend.h
#pragma once
#define NOMINMAX
#undef min
#undef max
#include <string>
#include <vector>
#include <torch/torch.h>
#include <torch/script.h>

class BaseModelBackend {
private:
    torch::jit::script::Module body;
    std::string MODEL_DIR;
    void load();

public:
    BaseModelBackend(std::string model_dir);
    std::string get_model_dir();
    at::Tensor forward(at::Tensor input);
    at::Tensor forward(std::vector<torch::jit::IValue> inputs);
};
