#ifndef COLOR
#define COLOR

// Color union for WBGR strips
struct Color {
    union {
        struct {
            uint8_t w;
            uint8_t b;
            uint8_t g;
            uint8_t r;
        } __attribute__((packed));
        struct {
            uint8_t white;
            uint8_t blue;
            uint8_t green;
            uint8_t red;
        } __attribute__((packed));
        uint32_t v;
    } __attribute__((packed));

    Color(uint8_t w) : w(w), b(0), g(0), r(0) {}

    Color(uint8_t r, uint8_t g, uint8_t b) : w(0), b(b), g(g), r(r) {}

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
        : w(w), b(b), g(g), r(r) {}
};

Color software_dim(Color& c1, size_t intensity) {
    return Color(c1.r * intensity / 255.0, c1.g * intensity / 255.0, c1.b * intensity / 255.0);
}

#endif
