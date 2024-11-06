#pragma once

#include <string>
#include <vector>
class Scroller;

class IRenderer {

public:
  virtual void Delay(size_t ms) noexcept = 0;
  virtual void Render(const Coord &pos, double scale,
                      const std::vector<std::string> &Objects) = 0;
  virtual ~IRenderer() noexcept = default;
};
