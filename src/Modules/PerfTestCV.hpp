#pragma once
#include "Engine/Module.hpp"

class PerfTestCV : public Module
{
  void Process() override
  {
    LOG_SCOPE("PerfTestCV");
    auto image = GetInputParameter<cv::Mat>("image").clone();
    const auto blurAmount = GetParameter<float>("blur amount");
    const auto blurIterations = GetParameter<int>("blur iterations");
    for (int i = 0; i < blurIterations; i++)
      cv::GaussianBlur(image, image, cv::Size(image.rows * blurAmount, image.rows * blurAmount), 0);
    SetOutputParameter("image", image);
  }

public:
  PerfTestCV()
  {
    GenerateModuleName();
    DefineInputParameter<cv::Mat>("image");
    DefineOutputParameter<cv::Mat>("image");
    DefineParameter<float>("blur amount", 0.01);
    DefineParameter<int>("blur iterations", 10);
  }
};
