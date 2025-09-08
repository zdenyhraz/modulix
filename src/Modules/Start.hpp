#pragma once
#include "Module/Module.hpp"

class Start : public Module {
  void Process() override {}

public:
  Start() { GenerateModuleName(); }
};
