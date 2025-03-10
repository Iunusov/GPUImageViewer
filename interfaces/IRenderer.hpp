#pragma once

#include <string>
#include <vector>
class Scroller;

class IRenderer {

public:
  virtual void Delay(size_t ms) noexcept = 0;
  virtual void Render(const Coord &pos, float scale, double angle,
                      const std::vector<std::string> &Objects) = 0;
  virtual ~IRenderer() noexcept = default;
};
