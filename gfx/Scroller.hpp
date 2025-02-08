#pragma once

#include <chrono>

#include "Config.hpp"
#include "Coord.hpp"

class Scroller final {
private:
  float m_scale{1.0f};
  Coord m_position{0.0f, 0.0f};
  float m_v_direction{0}, m_h_direction{0}, m_zoom_direction{0};
  float m_time_per_frame_ms{0};

public:
  Scroller(float ms) noexcept : m_time_per_frame_ms{ms} {}
  Coord GetCameraPos() const noexcept;
  float getScale() const noexcept;
  void execute() noexcept;

  std::chrono::time_point<std::chrono::steady_clock> ts{
      std::chrono::steady_clock::now()};
  bool escape_key_pressed{};
};
