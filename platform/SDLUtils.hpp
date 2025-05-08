#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>

namespace SDLUtils {
struct Tile {
  SDL_Texture *texture = nullptr;
  SDL_FRect dest{};
};
bool loadImageTilesToGPU(SDL_Renderer *renderer, const std::string &filePath,
                         std::vector<Tile> &tiles, int &imgW,
                         int &imgH) noexcept;

void getPrimaryDisplayResolution(size_t &w, size_t &h) noexcept;

} // namespace SDLUtils