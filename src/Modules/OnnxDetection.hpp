#pragma once
#include "Engine/Module.hpp"
#include "Onnx/OnnxModel.hpp"

class OnnxDetection : public Module
{
  std::optional<OnnxModel> model;
  std::vector<std::string> classNames;
  const float scoreThreshold = 0.5;

  struct Detection
  {
    cv::Rect box;
    float score = 0;
    int64_t label = 0;
  };

  void Load() override
  {
    model = OnnxModel(GetProjectPath(GetParameter<std::string>("model path")), "onnx_detection", {"input"}, {"boxes", "labels", "scores"});
    classNames = LoadClassNames(GetProjectPath(GetParameter<std::string>("classes path")));
    LOG_DEBUG("Classes: ['{}']", fmt::join(classNames, "', '"));
  }

  void Unload() override { model->Unload(); }

  void Process() override
  {
    LOG_SCOPE("OnnxDetection");
    const auto outputTensor = model->Run(GetInputParameter<cv::Mat>("image"));
    const auto detections = ParseModelOutput(outputTensor);
    LOG_DEBUG("Detected {} objects", detections.size());
    std::vector<cv::Rect> boxes;
    boxes.reserve(detections.size());
    for (const auto& det : detections)
    {
      LOG_DEBUG("Detection: {}, score: {}, box: [{}, {}, {}, {}]", det.label, det.score, det.box.x, det.box.y, det.box.width, det.box.height);
      boxes.push_back(det.box);
    }
    SetOutputParameter("objects", boxes);
  }

  std::vector<Detection> ParseModelOutput(const std::vector<Ort::Value>& outputTensor) const
  {
    if (outputTensor.size() != 3)
      throw MODULE_EXCEPTION("Model output size mismatch (expected 3, got {})", outputTensor.size());
    size_t detectionCount = outputTensor[0].GetTensorTypeAndShapeInfo().GetShape()[0];
    const float* boxes = outputTensor[0].GetTensorData<float>();
    const int64_t* labels = outputTensor[1].GetTensorData<int64_t>();
    const float* scores = outputTensor[2].GetTensorData<float>();

    std::vector<Detection> detections;
    detections.reserve(detectionCount);
    for (size_t idx = 0; idx < detectionCount; ++idx)
      if (scores[idx] > scoreThreshold)
      {
        cv::Rect box(boxes[idx * 4 + 0], boxes[idx * 4 + 1], boxes[idx * 4 + 2] - boxes[idx * 4], boxes[idx * 4 + 3] - boxes[idx * 4 + 1]);
        detections.emplace_back(box, scores[idx], labels[idx]);
      }

    return detections;
  }

  std::vector<std::string> LoadClassNames(const std::filesystem::path& classesPath) const
  {
    LOG_DEBUG("Loading class names from {}", classesPath);
    if (not std::filesystem::is_regular_file(classesPath))
      throw MODULE_EXCEPTION("Could not find classes file '{}'", classesPath);

    std::vector<std::string> classNames;
    std::ifstream file(classesPath);
    std::string line;
    while (std::getline(file, line))
      if (not line.empty() and not std::all_of(line.begin(), line.end(), isspace))
        classNames.push_back(line);

    return classNames;
  }

public:
  OnnxDetection()
  {
    GenerateModuleName();
    DefineInputParameter<cv::Mat>("image");
    DefineOutputParameter<std::vector<cv::Rect>>("objects");
    DefineParameter<std::string>("model path", "model.onnx");
    DefineParameter<std::string>("classes path", "classes.txt");
  }
};
