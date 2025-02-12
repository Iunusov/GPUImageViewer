#pragma once

#include "Coord.hpp"
#include "IRenderer.hpp"
#include "IVideoContext.hpp"

#include <vector>

class Renderer2D final : public IRenderer {
private:
  IVideoContext *ctx = nullptr;

public:
  Renderer2D(IVideoContext *ctx_) noexcept : ctx{ctx_} {}
  void Delay(size_t ms) noexcept override { ctx->delay(ms); }

  void Render(const Coord &pos, float scale, double angle,
              const std::vector<std::string> &Objects) noexcept override;
};
