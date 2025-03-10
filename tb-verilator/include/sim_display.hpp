// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iostream>
#include <fstream>
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
        SimDisplay(const char* title, uint32_t width, uint32_t height, uint32_t scale, uint8_t** banks, uint32_t offset);
        void render();
        void dump(const char* dump_file);

    private:
        void copy_banks_to_framebuf();

        // display dimensions
        uint32_t m_width;
        uint32_t m_height;
        std::vector<uint8_t> m_framebuf;

        // pointer to video memory banks
        uint8_t** m_banks;
        uint32_t m_offset;

        // SDL2
        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;
        SDL_Texture* m_texture = nullptr;
};