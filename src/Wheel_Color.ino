#ifndef WHEEL_COLOR
#define WHEEL_COLOR
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
static inline Color WheelColor(uint32_t i, uint8_t intensity) {
    if (intensity == 0)
        return Color(0);
    i &= 255;
    if (i < 85) {
        return Color(
            (256 - i * 3) * 255u / intensity, 0, (i * 3) * 255 / intensity);
    }
    if (i < 170) {
        i -= 85;
        return Color(
            0, (i * 3) * 255u / intensity, (256 - i * 3) * 255u / intensity);
    }
    i -= 170;
    return Color(
        (i * 3) * 255u / intensity, (256 - i * 3) * 255u / intensity, 0);
}
#endif
