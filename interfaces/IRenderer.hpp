#pragma once

#include <string>

class Scroller;

class IRenderer {

public:
  virtual void Delay(size_t ms) noexcept = 0;
  virtual void Load(const std::string &) noexcept = 0;
  virtual void Render(const Coord &pos, float scale, double angle) = 0;
  virtual ~IRenderer() noexcept = default;
};
