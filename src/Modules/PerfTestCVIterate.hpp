#pragma once
#include "Engine/Module.hpp"

class PerfTestCVIterate : public Module
{
  void Process() override
  {
    LOG_SCOPE("PerfTestCVIterate");
    auto image = GetInputParameter<cv::Mat>("image").clone();
    for (int r = 0; r < image.rows; ++r)
      for (int c = 0; c < image.cols; ++c)
        image.at<cv::Vec3b>(r, c) = 0.5 * image.at<cv::Vec3b>(r, c);
    SetOutputParameter("image", image);
  }

public:
  PerfTestCVIterate()
  {
    GenerateModuleName();
    DefineInputParameter<cv::Mat>("image");
    DefineOutputParameter<cv::Mat>("image");
  }
};
