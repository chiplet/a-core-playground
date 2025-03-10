// SPDX-License-Identifier: Apache-2.0

#include "sim_display.hpp"

SimDisplay::SimDisplay(const char* title, uint32_t width, uint32_t height, uint32_t scale, uint8_t** banks, uint32_t offset)
    : m_width(width), m_height(height), m_banks(banks), m_offset(offset)
{

    // allocate local flat screenbuffer
    m_framebuf = std::vector<uint8_t>(m_width * m_height, 0);

    // Initialize window
    m_window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_width*scale,
        m_height*scale,
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

    // AI slop toom color palette, no reason to believe this would be correct, but it's something
    SDL_Color doom_palette[256] = {
        {0, 0, 0, 255}, {31, 23, 11, 255}, {23, 15, 7, 255}, {47, 39, 27, 255},
        {59, 51, 31, 255}, {71, 63, 39, 255}, {87, 75, 47, 255}, {103, 91, 55, 255},
        {119, 107, 63, 255}, {135, 123, 75, 255}, {151, 139, 83, 255}, {167, 155, 95, 255},
        {187, 171, 107, 255}, {203, 187, 123, 255}, {219, 203, 135, 255}, {235, 219, 151, 255},
        {11, 11, 11, 255}, {15, 15, 15, 255}, {23, 23, 23, 255}, {27, 27, 27, 255},
        {35, 35, 35, 255}, {43, 43, 43, 255}, {47, 47, 47, 255}, {55, 55, 55, 255},
        {63, 63, 63, 255}, {75, 75, 75, 255}, {83, 83, 83, 255}, {95, 95, 95, 255},
        {103, 103, 103, 255}, {115, 115, 115, 255}, {123, 123, 123, 255}, {135, 135, 135, 255},
        {0, 0, 0, 255}, {7, 7, 7, 255}, {11, 11, 11, 255}, {19, 19, 19, 255},
        {27, 27, 27, 255}, {35, 35, 35, 255}, {43, 43, 43, 255}, {51, 51, 51, 255},
        {59, 59, 59, 255}, {67, 67, 67, 255}, {75, 75, 75, 255}, {83, 83, 83, 255},
        {91, 91, 91, 255}, {99, 99, 99, 255}, {107, 107, 107, 255}, {115, 115, 115, 255},
        {0, 0, 0, 255}, {7, 7, 7, 255}, {11, 11, 11, 255}, {19, 19, 19, 255},
        {27, 27, 27, 255}, {35, 35, 35, 255}, {43, 43, 43, 255}, {51, 51, 51, 255},
        {59, 59, 59, 255}, {67, 67, 67, 255}, {75, 75, 75, 255}, {83, 83, 83, 255},
        {91, 91, 91, 255}, {99, 99, 99, 255}, {107, 107, 107, 255}, {115, 115, 115, 255},
        {0, 0, 0, 255}, {7, 7, 7, 255}, {15, 15, 15, 255}, {23, 23, 23, 255},
        {31, 31, 31, 255}, {39, 39, 39, 255}, {47, 47, 47, 255}, {55, 55, 55, 255},
        {63, 63, 63, 255}, {71, 71, 71, 255}, {79, 79, 79, 255}, {87, 87, 87, 255},
        {95, 95, 95, 255}, {103, 103, 103, 255}, {111, 111, 111, 255}, {119, 119, 119, 255},
        {0, 0, 0, 255}, {7, 7, 7, 255}, {15, 15, 15, 255}, {23, 23, 23, 255},
        {31, 31, 31, 255}, {39, 39, 39, 255}, {47, 47, 47, 255}, {55, 55, 55, 255},
        {63, 63, 63, 255}, {71, 71, 71, 255}, {79, 79, 79, 255}, {87, 87, 87, 255},
        {95, 95, 95, 255}, {103, 103, 103, 255}, {111, 111, 111, 255}, {119, 119, 119, 255},
        {0, 0, 0, 255}, {7, 7, 7, 255}, {15, 15, 15, 255}, {23, 23, 23, 255},
        {31, 31, 31, 255}, {39, 39, 39, 255}, {47, 47, 47, 255}, {55, 55, 55, 255},
        {63, 63, 63, 255}, {71, 71, 71, 255}, {79, 79, 79, 255}, {87, 87, 87, 255},
        {95, 95, 95, 255}, {103, 103, 103, 255}, {111, 111, 111, 255}, {119, 119, 119, 255},
        {128, 128, 128, 255}, {135, 135, 135, 255}, {143, 143, 143, 255}, {151, 151, 151, 255},
        {159, 159, 159, 255}, {167, 167, 167, 255}, {175, 175, 175, 255}, {183, 183, 183, 255},
        {191, 191, 191, 255}, {199, 199, 199, 255}, {207, 207, 207, 255}, {215, 215, 215, 255},
        {223, 223, 223, 255}, {231, 231, 231, 255}, {239, 239, 239, 255}, {247, 247, 247, 255},
        {255, 255, 255, 255}, {128, 0, 0, 255}, {139, 69, 19, 255}, {160, 82, 45, 255},
        {169, 169, 169, 255}, {0, 128, 128, 255}, {0, 0, 255, 255}, {0, 255, 0, 255},
        {255, 255, 255, 255}
    };

    // Initialize texture
    m_texture = SDL_CreateTexture(
        m_renderer,
        // SDL_PIXELFORMAT_ABGR8888,
        SDL_PIXELFORMAT_RGB332,
        // SDL_PIXELFORMAT_INDEX8,
        SDL_TEXTUREACCESS_TARGET,
        m_width,
        m_height
    );

    if (m_renderer == NULL) {
        fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
        return;
    }
}

void SimDisplay::copy_banks_to_framebuf() {
    for (int i = 0; i < m_width*m_height; i++) {
        uint32_t addr = m_offset + i;
        uint32_t bank_idx = addr & 0b11;
        uint32_t bank_addr = addr >> 2;

        m_framebuf[i] = m_banks[bank_idx][bank_addr];
        // m_framebuf[i] = *(*(m_banks + bank_idx) + bank_addr);
    }
}

void SimDisplay::render() {
    // SDL_UpdateTexture(m_texture, NULL, m_frame_buf, m_width*sizeof(Pixel));
    copy_banks_to_framebuf();
    SDL_UpdateTexture(m_texture, NULL, m_framebuf.data(), m_width);
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
    SDL_RenderPresent(m_renderer);
}

void SimDisplay::dump(const char* dump_file) {
    std::ofstream outFile(dump_file, std::ios::binary);
    if (outFile) {
        outFile.write(reinterpret_cast<const char*>(m_framebuf.data()), m_framebuf.size());
        outFile.close();
    }
}