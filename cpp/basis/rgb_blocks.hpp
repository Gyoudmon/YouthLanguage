#ifndef _RGB_BLOCKS_H // 确保只被 include 一次
#define _RGB_BLOCKS_H

#include "game.hpp"

namespace WarGrey::STEM {
    class RGBBlock : public WarGrey::STEM::DrawingBoard {
        public:
            RGBBlock();
        
        public:
            void draw(SDL_Renderer* renderer, int x, int y, int width, int height);
    };
    
    class ContrastColorBlock : public WarGrey::STEM::DrawingBoard {
        public:
            ContrastColorBlock();
        
        public:
            void construct(int argc, char* argv[]);
            void draw(SDL_Renderer* renderer, int x, int y, int width, int height);

        private:
            float hue0;
    };
}

#endif
