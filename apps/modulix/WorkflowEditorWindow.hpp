#pragma once
#include "Gui/Window.hpp"
#include "WorkflowEditor.hpp"

class WorkflowEditorWindow : public Window
{
  void CreateWorkflow();
  void ShowFlow();
  void Run();

  WorkflowEditor editor;

public:
  void Initialize() override;
  void Render() override;
};
