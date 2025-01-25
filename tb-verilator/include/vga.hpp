// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include "SDL2/SDL.h"

typedef struct Pixel {  // for SDL texture
    uint8_t r;  // red
    uint8_t g;  // green
    uint8_t b;  // blue
    uint8_t a;  // transparency
} Pixel;

class SimDisplay {
    public:
        SimDisplay(uint32_t width, uint32_t height, uint8_t* hsync, uint8_t* vsync, uint8_t* r, uint8_t* g, uint8_t* b);
        void tick();

    private:
        // display dimensions
        uint32_t m_width;
        uint32_t m_height;

        // display counters
        uint32_t m_ticks = 0;
        uint32_t m_pixel_counter = 0;
        uint32_t m_x = 0;
        uint32_t m_y = 0;
        uint8_t m_last_hsync = 1;
        uint8_t m_last_vsync = 1;
        uint32_t m_last_hsync_ticks = 0;
        uint32_t m_ticks_per_hline = 0;

        // SDL2
        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;
        SDL_Texture* m_texture = nullptr;
        std::vector<Pixel> m_screenbuffer;

        // Verilator
        uint8_t* m_hsync;
        uint8_t* m_vsync;
        uint8_t* m_r;
        uint8_t* m_g;
        uint8_t* m_b;
};