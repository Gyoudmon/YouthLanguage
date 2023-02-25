#include "big_bang/game.hpp"

namespace WarGrey::STEM {
    class TheCosmos : public WarGrey::STEM::Cosmos {
    public:
        TheCosmos(int fps) : WarGrey::STEM::Cosmos(fps) {}
        virtual ~TheCosmos();

    public:  // 覆盖游戏基本方法
        void construct(int argc, char* argv[]) override;

    protected:
        void update(uint32_t count, uint32_t interval, uint32_t uptime) override;

    protected:
        virtual void parse_cmdline_options(int argc, char* argv[]) {}
    };
}