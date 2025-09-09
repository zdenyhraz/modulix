#include "WorkflowEditorWindow.hpp"
#include "Modules/LoadImage.hpp"
#include "Modules/SaveImage.hpp"
#include "Modules/DrawObjects.hpp"
#include "Modules/OnnxDetection.hpp"
#include "Utils/Async.hpp"

void WorkflowEditorWindow::Initialize()
{
  editor.OnStart();
  CreateWorkflow();
  LOG_DEBUG("OpenCV version:\n{}", cv::getBuildInformation());
}

void WorkflowEditorWindow::Render()
{
  if (ImGui::BeginTabItem("MicroserviceEditor"))
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

void WorkflowEditorWindow::ShowFlow()
{
  editor.ShowFlow();
}
