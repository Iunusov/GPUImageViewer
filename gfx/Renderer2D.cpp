#include "Renderer2D.hpp"
#include "Config.hpp"
#include <string>

void Renderer2D::Render(const Coord &pos, float scale, double angle,
                        const std::vector<std::string> &Objects) noexcept {
  ctx->clear();

  ctx->setScale(scale);
  ctx->setAngle(angle);

  for (const auto &obj : Objects) {
    ctx->draw(obj);
  }

  ctx->setCamera(pos);

  ctx->present();
}
