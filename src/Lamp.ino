#ifndef LAMP
#define LAMP
class Lamp
{
public:
    Lamp(size_t addr)
        : addr_(addr)
    {
        dmx.write(addr_ + 0, 11);
        dmx.write(addr_ + 1, 255);
        dmx.write(addr_ + 2, 0);
    }

    void dimm(size_t v) {
        dmx.write(addr_ + 1, v);
    }

    void flash(size_t v) {
        dmx.write(addr_ + 2, v);
    }

    // all colors
    void set(size_t v) {
        for (size_t i = 0; i < 16; ++i) {
            dmx.write(addr_ + 3 + i, v);
        }
    }

    // RGBA colors
    void set_part(size_t p, size_t r, size_t g, size_t b, size_t a) {
        dmx.write(addr_ + 3 + 4 * p + 0, r);
        dmx.write(addr_ + 3 + 4 * p + 1, g);
        dmx.write(addr_ + 3 + 4 * p + 2, b);
        dmx.write(addr_ + 3 + 4 * p + 3, a);
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
    size_t addr_;
};
#endif
