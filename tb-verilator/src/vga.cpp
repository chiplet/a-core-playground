// SPDX-License-Identifier: Apache-2.0

#include "vga.hpp"

VGA::VGA(uint32_t width, uint32_t height, uint8_t* hsync, uint8_t* vsync, uint8_t* r, uint8_t* g, uint8_t* b) :
    m_width(width), m_height(height), m_hsync(hsync), m_vsync(vsync), m_r(r), m_g(g), m_b(b)
{
    // init screenbuffer to pink
    m_screenbuffer = std::vector<Pixel>(m_width*m_height, {255,0,255,255});

    // Initialize window
    m_window = SDL_CreateWindow(
        "VGA Display Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_width,
        m_height,
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

void VGA::tick() {
    uint8_t hsync_falling = m_last_hsync && !(*m_hsync);
    uint8_t vsync_falling = m_last_vsync && !(*m_vsync);
    uint8_t pixel_stb = m_pixel_counter >= m_ticks_per_hline / m_width;

    if (pixel_stb) {
        // offset sync and back porch 
        int x_offset = m_x - 96 - 48;
        int y_offset = m_y - 2 - 33;
        x_offset -= 3; // (manual offset)
        if (x_offset >= 0 && y_offset >= 0 && x_offset < m_width && y_offset < m_height)
            m_screenbuffer[y_offset*m_width + x_offset] = {*m_r, *m_g, *m_b, 255};
        m_x += 1;
        m_pixel_counter = 0;
    }

    if (hsync_falling) {
        m_x = 0;
        m_y += 1;
        m_ticks_per_hline = m_ticks - m_last_hsync_ticks;
        m_last_hsync_ticks = m_ticks;
    }

    if (vsync_falling) {
        m_y += 1;
        if (m_y >= m_height)
            m_y = 0;
    }

    if (vsync_falling) {
        // printf("x=%d y=%d\n", m_x, m_y);
        SDL_UpdateTexture(m_texture, NULL, m_screenbuffer.data(), m_width*sizeof(Pixel));
        SDL_RenderClear(m_renderer);
        SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
        SDL_RenderPresent(m_renderer);
    }

    m_last_hsync = *m_hsync;
    m_last_vsync = *m_vsync;
    m_pixel_counter++;
    m_ticks++;
}