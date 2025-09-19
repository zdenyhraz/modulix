#include "WorkflowEditorWindow.hpp"
#include "Modules/LoadImage.hpp"
#include "Modules/SaveImage.hpp"
#include "Modules/DrawObjects.hpp"
#include "Modules/OnnxDetection.hpp"
#include "Modules/PerfTestCV.hpp"
#include "Modules/PerfTestCVIterate.hpp"
#include "Utils/Async.hpp"

void WorkflowEditorWindow::Initialize()
{
  editor.OnStart();
  CreateWorkflow();
  LOG_DEBUG("OpenCV version:\n{}", cv::getBuildInformation());
}

void WorkflowEditorWindow::Render()
{
  if (ImGui::BeginTabItem("Workflow Editor"))
  {
    ImGui::Separator();
    if (ImGui::Button("Run"))
      Launch([&]() { Run(); });
    ImGui::SameLine();
    if (ImGui::Button("Run async"))
      LaunchAsync([&]() { Run(); });
    ImGui::SameLine();
    if (ImGui::Button("Show flow"))
      editor.ShowFlow();

    editor.OnFrame();

    ImGui::EndTabItem();
  }
}

void WorkflowEditorWindow::Run()
{
  LOG_SCOPE("Workflow Load+Run");
  editor.workflow.Load();
  editor.workflow.Run();
}

void WorkflowEditorWindow::CreateWorkflow()
{
  if constexpr (false)
  {
    editor.workflow.SetName("Onnx object detection");
    auto& load = editor.workflow.AddModule<LoadImage>();
    auto& onnx = editor.workflow.AddModule<OnnxDetection>();
    auto& draw = editor.workflow.AddModule<DrawObjects>();
    auto& save = editor.workflow.AddModule<SaveImage>();

    load.SetParameter("image path", std::string("data/umbellula/umbellula.jpg"));
    onnx.SetParameter("model path", std::string("data/umbellula/umbellula.onnx"));
    onnx.SetParameter("classes path", std::string("data/umbellula/umbellula.txt"));
    save.SetParameter("image path", std::string("data/umbellula/result_cpp.jpg"));

    editor.workflow.Connect(editor.workflow.GetStart(), onnx);
    editor.workflow.Connect(load, onnx, "image");
    editor.workflow.Connect(load, draw, "image");
    editor.workflow.Connect(onnx, save);
    editor.workflow.Connect(onnx, draw, "objects");
    editor.workflow.Connect(draw, save, "image");
  }
  else if constexpr (true)
  {
    editor.workflow.SetName("Performance test OpenCV");
    auto& load = editor.workflow.AddModule<LoadImage>();
    auto& perf = editor.workflow.AddModule<PerfTestCV>();
    auto& save = editor.workflow.AddModule<SaveImage>();

    load.SetParameter("image path", std::string("data/perf/perf_image.jpg"));
    save.SetParameter("image path", std::string("data/perf/perf_result.jpg"));

    editor.workflow.Connect(editor.workflow.GetStart(), perf);
    editor.workflow.Connect(load, perf, "image");
    editor.workflow.Connect(perf, save);
    editor.workflow.Connect(perf, save, "image");
  }
  else
  {
    editor.workflow.SetName("Performance test OpenCV iterate");
    auto& load = editor.workflow.AddModule<LoadImage>();
    auto& perf = editor.workflow.AddModule<PerfTestCVIterate>();
    auto& save = editor.workflow.AddModule<SaveImage>();

    load.SetParameter("image path", std::string("data/perf/perf_image.jpg"));
    save.SetParameter("image path", std::string("data/perf/perf_result.jpg"));

    editor.workflow.Connect(editor.workflow.GetStart(), perf);
    editor.workflow.Connect(load, perf, "image");
    editor.workflow.Connect(perf, save);
    editor.workflow.Connect(perf, save, "image");
  }
}

void WorkflowEditorWindow::ShowFlow()
{
  editor.ShowFlow();
}
