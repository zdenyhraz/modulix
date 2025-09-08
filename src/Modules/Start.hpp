#pragma once
#include "Engine/Module.hpp"

class Start : public Module
{
  void Process() override {}

public:
  Start() { GenerateModuleName(); }
};
