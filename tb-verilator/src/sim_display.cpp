// SPDX-License-Identifier: Apache-2.0

#include "sim_display.hpp"

SimDisplay::SimDisplay(uint32_t width, uint32_t height, uint8_t* p_frame_buf)
    : m_width(width), m_height(height), m_frame_buf(p_frame_buf)
{
    // Initialize window
    m_window = SDL_CreateWindow(
        "Simulation Display",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_width*4,
        m_height*4,
        SDL_WINDOW_SHOWN
    );
    if (m_window == NULL) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        return;
    }

    // Initialize renderer
    m_renderer = SDL_CreateRenderer(
        m_window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (m_renderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        return;
    }

    // Initialize texture
    m_texture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_TARGET,
        m_width,
        m_height
    );
    if (m_renderer == NULL) {
        fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
        return;
    }
}

void SimDisplay::render() {
    SDL_UpdateTexture(m_texture, NULL, m_frame_buf, m_width*sizeof(Pixel));
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
    SDL_RenderPresent(m_renderer);
}