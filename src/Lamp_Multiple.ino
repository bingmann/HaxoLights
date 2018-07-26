#ifndef LAMP
#define LAMP
class Lamp
{
public:
    Lamp(size_t addr)
    {
        addrs_.push_back(addr);
        init_lamp(addrs_[0]);
    }

    Lamp(std::vector<size_t> addrs)
        : addrs_(addrs)
    {
        for (int i = 0; i < addrs_.size(); ++i) {
            init_lamp(addrs_[i]);
        }
    }

    void dimm(size_t v) {
        for (int i = 0; i < addrs_.size(); ++i) {
            dmx.write(addrs_[i] + 1, v);
        }
    }

    void flash(size_t v) {
        for (int i = 0; i < addrs_.size(); ++i) {
            dmx.write(addrs_[i] + 2, v);
        }
    }

    // all colors
    void set(size_t v) {
        for (int i = 0; i < addrs_.size(); ++i) {
            for (size_t i = 0; i < 16; ++i) {
                dmx.write(addrs_[i] + 3 + i, v);
            }
        }
    }

    // RGBA colors
    void set_part(size_t p, size_t r, size_t g, size_t b, size_t a) {
        for (int i = 0; i < addrs_.size(); ++i) {
            dmx.write(addrs_[i] + 3 + 4 * p + 0, r);
            dmx.write(addrs_[i] + 3 + 4 * p + 1, g);
            dmx.write(addrs_[i] + 3 + 4 * p + 2, b);
            dmx.write(addrs_[i] + 3 + 4 * p + 3, a);
        }
    }

    // RGBA colors
    void set_part(size_t p, const Color& c) {
        set_part(p, c.r, c.g, c.b, c.w);
    }

    // RGBA colors
    void set(size_t r, size_t g, size_t b, size_t a) {
        for (size_t i = 0; i < 4; ++i) {
            set_part(i, r, g, b, a);
        }
    }

    // RGBA colors
    void set(const Color& c) {
        set(c.r, c.g, c.b, c.w);
    }

private:
    std::vector<size_t> addrs_;
    void init_lamp(size_t addr) {
        dmx.write(addr + 0, 11);
        dmx.write(addr + 1, 255);
        dmx.write(addr + 2, 0);
    }
};
#endif
