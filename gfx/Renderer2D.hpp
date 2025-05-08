#pragma once

#include "Coord.hpp"
#include "IRenderer.hpp"
#include "IVideoContext.hpp"

#include <string>

class Renderer2D final : public IRenderer {
private:
  IVideoContext *ctx = nullptr;

public:
  Renderer2D(IVideoContext *ctx_) noexcept : ctx{ctx_} {}
  void Delay(size_t ms) noexcept override { ctx->delay(ms); }

  void Render(const Coord &pos, float scale, double angle) noexcept override;

  void Load(const std::string &path) noexcept override { ctx->load(path); }
};
