#pragma once
#include "Module/Module.hpp"

class SaveImage : public Module {
  void Process() override {
    LOG_SCOPE("SaveImage");
    const auto path =
        GetProjectPath(GetParameter<std::string>("image path")).string();
    const auto &image = GetInputParameter<cv::Mat>("image");
    cv::imwrite(path, image);
    LOG_DEBUG("Saved image size: {}x{}x{}", image.cols, image.rows,
              image.channels());
    LOG_DEBUG("Saved image to '{}'", path);
  }

public:
  SaveImage() {
    GenerateModuleName();
    DefineInputParameter<cv::Mat>("image");
    DefineParameter<std::string>("image path", "");
  }
};
