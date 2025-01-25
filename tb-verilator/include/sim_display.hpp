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
        SimDisplay(uint32_t width, uint32_t height, uint8_t* p_frame_buf);
        void render();

    private:
        // display dimensions
        uint32_t m_width;
        uint32_t m_height;

        // pointer to frame buffer (video memory array of simulated dut)
        uint8_t* m_frame_buf;

        // SDL2
        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;
        SDL_Texture* m_texture = nullptr;
};