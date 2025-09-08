#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <chrono>
#include <numeric>
#include <filesystem>
#include <queue>
#include <functional>
#include <vector>
#include <complex>
#include <tuple>
#include <thread>
#include <memory>
#include <regex>
#include <ctime>
#include <limits>
#include <random>
#include <algorithm>
#include <mutex>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <cstdint>
#include <variant>
#include <initializer_list>
#include <span>
#include <type_traits>
#include <concepts>
#include <source_location>
#include <numbers>
#include <bit>
#include <ranges>
#include <future>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/chrono.h>
#include <fmt/std.h>
#include <fmt/ranges.h>
#include <fmt/color.h>
#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>

using namespace std::complex_literals;
using namespace std::chrono_literals;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define ImDrawIdx unsigned int
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_glfw.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <implot_internal.h>

namespace ImGui
{
inline bool SliderPercentage(const char* label, float* v, float v_min, float v_max, const char* format = "%.1f%%", ImGuiSliderFlags flags = 0)
{
  float valuePercent = *v * 100;
  ImGui::SliderFloat(label, &valuePercent, v_min * 100, v_max * 100, format, flags);
  *v = valuePercent / 100;
  return true;
}
};

#include "Log/Log.hpp"
