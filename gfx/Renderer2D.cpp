#include "Renderer2D.hpp"
#include "Config.hpp"
#include <string>

void Renderer2D::Render(const Coord &pos, float scale, double angle) noexcept {
  ctx->clear();
  ctx->setScale(scale);
  ctx->setAngle(angle);
  ctx->setCamera(pos);
  ctx->draw();
  ctx->present();
}
