#pragma once

#include <chrono>

#include "Config.hpp"
#include "Coord.hpp"

class Scroller final {
private:
  float m_scale{1.0f};
  Coord m_position{};
  float m_v_direction{}, m_h_direction{}, m_zoom_direction{};
  float m_time_per_frame_ms{};
  float m_wheel_y{};

public:
  Scroller(float ms) noexcept : m_time_per_frame_ms{ms} {}
  Coord GetCameraPos() const noexcept;
  float getScale() const noexcept;
  void execute() noexcept;

  std::chrono::time_point<std::chrono::steady_clock> ts{
      std::chrono::steady_clock::now()};
  bool escape_key_pressed{};
};
