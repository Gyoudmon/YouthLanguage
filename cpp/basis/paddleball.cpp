#include "paddleball.hpp"

using namespace WarGrey::STEM;

const int ball_radius = 8;
const int paddle_width = 128;
const int paddle_height = 8;

const int ball_speed = 4;
const int paddle_speed = ball_speed * 3;

/*************************************************************************************************/
WarGrey::STEM::PaddleBall::PaddleBall()
    : Universe("Paddle Ball", 60, 0x000000U, 0xFFFFFFU) { /* 什么都不做 */ }

WarGrey::STEM::PaddleBall::~PaddleBall() { /* 什么都不做 */ }

/*************************************************************************************************/
void WarGrey::STEM::PaddleBall::reflow(int width, int height) {
    this->screen_width = width;
    this->screen_height = height;

    // 确保球产生于屏幕上方的中间
    this->ball_x = this->screen_width / 2;
    this->ball_y = ball_radius;

    this->ball_dx = 1 * ball_speed;
    this->ball_dy = 1 * ball_speed;

    // 确保浆产生在靠近屏幕下方的中间
    this->paddle_x = this->ball_x - paddle_width / 2;
    this->paddle_y = this->screen_height - paddle_height * 3;
}

void WarGrey::STEM::PaddleBall::update(uint32_t interval, uint32_t count, uint32_t uptime) {
    int dead_y = this->screen_height - ball_radius;

    if (this->ball_y < dead_y) {
        this->ball_x = this->ball_x + this->ball_dx;
        this->ball_y = this->ball_y + this->ball_dy;

        if ((this->ball_x <= ball_radius) || (this->ball_x >= this->screen_width - ball_radius)) {
            this->ball_dx = -this->ball_dx;
        }
    
        if (this->ball_y <= ball_radius) {
            this->ball_dy = -this->ball_dy;
        }

        /* 检测小球是否被捕获 */ {
            int ball_bottom = this->ball_y + ball_radius;

            if ((ball_bottom >= this->paddle_y)
                    && (this->ball_x >= this->paddle_x)
                    && (this->ball_x <= this->paddle_x + paddle_width)) {
                this->ball_dy = -this->ball_dy;
            }
        }
    }
}

void WarGrey::STEM::PaddleBall::draw(SDL_Renderer* renderer, int x, int y, int width, int height) {
    game_fill_circle(renderer, this->ball_x, this->ball_y, ball_radius,
            ((this->ball_y >= this->paddle_y) ? RED : ORANGE));

    game_fill_rect(renderer, this->paddle_x, this->paddle_y,
            paddle_width, paddle_height, FORESTGREEN);
}

/*************************************************************************************************/
bool WarGrey::STEM::PaddleBall::on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) {
    bool handled = false;

    if (pressed) {
        switch(key) {
            case 'a': {
                if (this->paddle_x > 0) {
                    this->paddle_x -= paddle_speed;
                    handled = true;
                }
            }; break;
            case 'd': {
                if ((this->paddle_x + paddle_width) < this->screen_width) {
                    this->paddle_x += paddle_speed;
                    handled = true;
                }
            }; break;
        }
    }

    /* 思考，为什么这个始终返回 false 而不是 handled */
    return false;
}
