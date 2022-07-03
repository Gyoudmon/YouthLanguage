#ifndef _GAME_OF_LIFE_H // 确保只被 include 一次
#define _GAME_OF_LIFE_H

#include "game.hpp"

namespace WarGrey::STEM {
    enum class GameState { Run, Stop, Edit };

    /** 声明游戏宇宙 **/
    class GameOfLife : public WarGrey::STEM::Universe {
        public:
            GameOfLife(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, const char* title);
            virtual ~GameOfLife();

        public:    // 覆盖游戏基本方法
            void construct(int argc, char* argv[], int width, int height);
            void update(SDL_Renderer* renderer, uint32_t interval, uint32_t count, uint32_t uptime);

        protected: // 覆盖输入事件处理方法
            void on_char(char key, uint16_t modifiers, uint8_t repeats);        // 处理键盘事件
            void on_click(int x, int y);                                        // 处理单击事件

        protected: // 演化策略, 默认留给子类实现
            virtual void evolve(int** world, int* shadow, int stage_width, int stage_height) = 0;

        private:
            void timeline_forward(uint32_t interval, uint32_t count, uint32_t uptime);
            void switch_game_state(WarGrey::STEM::GameState new_state);
            void display_game_state(SDL_Renderer* renderer, const std::string &desc_state, uint32_t color);
            void display_instruction(SDL_Renderer* renderer, const std::string &desc_state, char key, int index);
            void display_user_message(SDL_Renderer* renderer, const std::string &message);

        private:
            void construct_random_game_world();
            void reset_game_world();
            bool forward_game_world(int repeats, bool force);

        private: // 舞台参数
            int** world;
            int* shadow;
            int generation;
            int stage_x;
            int stage_y;
            int stage_width;
            int stage_height;
            int screen_width;
            int screen_height;

        private: // 游戏状态
            WarGrey::STEM::GameState state;           
            bool hide_grid;

        private:
            int chwidth;
            int lineheight;
            char last_key_typed;
            std::string user_message;
    };

    class ConwayLife : public WarGrey::STEM::GameOfLife {
        public:
            ConwayLife(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture);

        protected:
            void evolve(int** world, int* shadow, int stage_width, int stage_height);
    };

    class HighLife : public WarGrey::STEM::GameOfLife {
        public:
            HighLife(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture);

        protected:
            void evolve(int** world, int* shadow, int stage_width, int stage_height);
    };
}

#endif
