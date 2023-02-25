#include "splash.hpp"
#include "bang.hpp"

#include "big_bang/datum/string.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
static const float tux_speed_walk_x = 2.4F;
static const float tux_speed_jump_x = tux_speed_walk_x;
static const float tux_speed_jump_y = -12.0F;
static const float tux_speed_dy = 1.0F;

static const std::vector<std::pair<int, int>> tux_spots = {
    /* { row, col } */
    { 6, 4 },
    { 6, 14 }, { 12, 24 },
    { 17, 34 }, { 22, 44 }
};

static const std::vector<std::vector<std::pair<int, int>>> task_info = {
    { /* { row, col } */ },
    { { 11, 15 }, { 11, 18 }, { 9, 21 } },
    { /* { 16, 27 } */ },
    { { 21, 37 }, { 22, 37 }, { 19, 40 } }
};

/*************************************************************************************************/
namespace {
    class BigBangPlane : public Plane {
    public:
        BigBangPlane(Cosmos* master) : Plane("宇宙大爆炸"), master(master) {}

    public:  // 覆盖游戏基本方法
        void load(float width, float height) override {
            this->splash = this->insert(new GridAtlas("splash.png"));
            this->title = this->insert(new Labellet(bang_font::title, GHOSTWHITE, title_fmt, this->name()));
            this->agent = this->insert(new Linkmon());
            
            this->load_tasks(width, height);
            this->tux = this->insert(new Tuxmon());

            this->tooltip = this->insert(make_label_for_tooltip(bang_font::tiny, GHOSTWHITE));
            this->set_tooltip_matter(this->tooltip);

            this->agent->scale(-1.0F, 1.0F);
            this->splash->create_logic_grid(28, 45);
            // this->splash->set_logic_grid_color(DIMGRAY);

            this->set_background(BLACK);
        }
        
        void reflow(float width, float height) override {
            this->move_to(this->title, this->agent, MatterAnchor::RB, MatterAnchor::LB);
            this->move_to(this->splash, width * 0.5F, height * 0.5F, MatterAnchor::CC);
            
            this->reflow_tasks(width, height);
            this->tux_home();
        }

        void update(uint32_t count, uint32_t interval, uint32_t uptime) override {
            this->tux_step(count, interval, uptime);

            if (this->target_plane > 0) {
                if (!this->agent->in_playing()) {
                    this->master->transfer_to_plane(this->target_plane);
                    this->target_plane = 0;
                }
            }
        }

        void on_mission_start() override {
            this->agent->play("Greeting", 1);
            this->tux->set_speed(tux_speed_walk_x, 0.0F);
            this->no_selected();
        }

    public:
        bool can_select(IMatter* m) override {
            return (dynamic_cast<Coinlet*>(m) != nullptr)
                    || (m == this->tux);
        }

        void after_select(IMatter* m, bool yes) override {
            if (yes) {
                if (m == this->tux) {
                    if (this->tux->is_wearing()) {
                        this->tux->take_off();
                    } else {
                        this->tux->wear("ice_hat");
                    }
                } else {
                    Coinlet* coin = dynamic_cast<Coinlet*>(m);

                    if (coin != nullptr) {
                        if (coin->name.compare(unknown_task_name) != 0) {
                            this->target_plane = coin->idx;
                            this->agent->play("Hide", 1);
                        }
                    }
                }
            }
        }

        bool update_tooltip(IMatter* m, float x, float y) override {
            bool updated = false;
            auto coin = dynamic_cast<Coinlet*>(m);

            if ((coin != nullptr) && !this->tooltip->visible()) {
                this->tooltip->set_text((coin->name.compare(unknown_task_name) == 0) ? BLACK : ROYALBLUE,
                    " %s ", coin->name.c_str());
                
                updated = true;
            }

            return updated;
        }

    private:
        void load_tasks(float width, float height) {
            int task_idx = 0;

            for (int seg = 0; seg < task_info.size(); seg ++) {
                std::vector<Coinlet*> subcoins;
                std::vector<Labellet*> subnames;

                for (int idx = 0; idx < task_info[seg].size(); idx ++) {
                    const char* tooltip = this->master->plane_name(++ task_idx);

                    if (tooltip == nullptr) {
                        this->load_task(subcoins, unknown_task_name, task_idx);
                    } else {
                        this->load_task(subcoins, tooltip, task_idx);
                    }
                }

                this->coins.push_back(subcoins);
            }

            for (int idx = task_idx + 1; idx < this->master->plane_count(); idx ++) {
                this->load_task(this->bonus_coins, this->master->plane_name(idx), idx);
            }
        }

        void load_task(std::vector<Coinlet*>& subcoins, const char* tooltip, int task_idx) {
            subcoins.push_back(this->insert(new Coinlet(tooltip, task_idx)));
            
            if (strcmp(tooltip, unknown_task_name) == 0) {
                subcoins.back()->stop();
            }
        }

        void reflow_tasks(float width, float height) {
            float dx, dy;

            for (int seg = 0; seg < task_info.size(); seg ++) {
                auto subinfos = task_info[seg];
                auto subcoins = this->coins[seg];
                
                for (int idx = 0; idx < subinfos.size(); idx ++) {
                    auto pos = subinfos[idx];

                    this->splash->feed_logic_tile_location(pos.first, pos.second, &dx, &dy, MatterAnchor::CC, false);
                    this->move_to(subcoins[idx], dx, dy, MatterAnchor::CC);
                }
            }

            for (size_t idx = this->bonus_coins.size(); idx > 0; idx --) {
                if (idx == this->bonus_coins.size()) {
                    this->move_to(this->bonus_coins[idx - 1], this->splash, MatterAnchor::LB, MatterAnchor::LB);
                } else {
                    this->move_to(this->bonus_coins[idx - 1], this->bonus_coins[idx], MatterAnchor::RC, MatterAnchor::LC);
                }
            }
        }
    
    private:
        void tux_home() {
            int row = tux_spots[0].first;
            int col = tux_spots[0].second;
            float x0, y0;
            
            this->splash->feed_logic_tile_location(row, col, &x0, &y0, MatterAnchor::LB, false);
            this->move_to(this->tux, x0, y0, MatterAnchor::CB);
            this->tux_walk_segment = 1;
            this->tux_start_walk();
        }

        void tux_start_walk() {
            this->tux->play("walk");
            this->tux->set_speed_xy(tux_speed_walk_x, 0.0F);
            this->tux_target_y = 0.0F;
        }

        void tux_step(uint32_t count, uint32_t interval, uint32_t uptime) {
            float x0, y0, tx, ty, gx;
            
            this->feed_matter_location(this->tux, &tx, &ty, MatterAnchor::RB);
            this->feed_matter_location(this->splash, &x0, &y0, MatterAnchor::LT);
            
            tx -= x0;
            ty -= y0;

            if (this->tux_target_y == 0.0F) {
                this->feed_splash_location(this->tux_walk_segment, &gx, nullptr);

                if (tx >= gx) {
                    this->tux_walk_segment += 1;
                    
                    if (this->tux_walk_segment < tux_spots.size()) {
                        this->feed_splash_location(this->tux_walk_segment, nullptr, &this->tux_target_y);
                        this->tux->play("buttjump");
                        this->tux->set_speed_xy(tux_speed_jump_x, tux_speed_jump_y);
                    } else {
                        this->tux_home();
                    }
                }
            } else if (ty >= this->tux_target_y) {
                this->tux_start_walk();
            } else {
                this->tux->set_speed_xy(tux_speed_jump_x,
                    this->tux->y_speed() + tux_speed_dy);
            }
        }

        void feed_splash_location(size_t idx, float* x, float* y) {
            this->splash->feed_logic_tile_location(
                tux_spots[idx].first,
                tux_spots[idx].second,
                x, y,
                MatterAnchor::LB);
        }

    private:
        Linkmon* agent;
        Labellet* title;
        std::vector<std::vector<Coinlet*>> coins;
        std::vector<Coinlet*> bonus_coins;
        Labellet* tooltip;
        Sprite* tux;
        GridAtlas* splash;

    private:
        int tux_walk_segment = 1;
        float tux_target_y = 0.0F;
        
    private:
        Cosmos* master;
        int target_plane = 0;
    };
}

/*************************************************************************************************/
WarGrey::STEM::TheCosmos::~TheCosmos() {
    bang_fonts_destroy();
    imgdb_teardown();
}

void WarGrey::STEM::TheCosmos::construct(int argc, char* argv[]) {
    enter_digimon_zone(argv[0]);
    bang_fonts_initialize();
    imgdb_setup(digimon_zonedir().append("stone"));
    this->parse_cmdline_options(argc, argv);

    this->push_plane(new BigBangPlane(this));
}

void WarGrey::STEM::TheCosmos::update(uint32_t count, uint32_t interval, uint32_t uptime) {
    if (this->has_current_mission_completed()) {
        this->transfer_to_plane(0);
    }
}