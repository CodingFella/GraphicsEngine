#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "graphi.c"

#define WIDTH 800
#define HEIGHT 600

#define RED 0xFFFF0000
#define GREEN 0xFF00FF00
#define BLUE 0xFF0000FF
#define WHITE 0xFFFFFFFF




void draw_line(uint32_t *canvas, int width, int height, int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    // just a point!
    if(dx == 0 && dy == 0) {
        canvas[y1 * width + x1] = color;
    }

}


static uint32_t pixels[WIDTH*HEIGHT];

int main() {
    fill_screen(pixels, WIDTH, HEIGHT, WHITE);

    uint32_t colour = 0x00000000;

    draw_point(pixels, WIDTH, HEIGHT, 0, 1, colour);

    for(size_t i = 0; i < WIDTH; ++i) {
        for(size_t j = 0; j < HEIGHT; ++j) {
            uint8_t hue = (uint8_t)(((double)(i + j) / (double)(WIDTH+HEIGHT)) * 255); // Map i to the range [0, 255]

            // Convert the hue to RGB using a rainbow color scheme
            uint8_t r, g, b;
            if (hue < 85) {
                r = 255 - hue * 3;
                g = 0;
                b = hue * 3;
            } else if (hue < 170) {
                hue -= 85;
                r = 0;
                g = hue * 3;
                b = 255 - hue * 3;
            } else {
                hue -= 170;
                r = hue * 3;
                g = 255 - hue * 3;
                b = 0;
            }

            // Combine the RGB components into a single uint32_t color value
            uint32_t current_color = (r << 16) | (g << 8) | b;
            draw_point(pixels, WIDTH, HEIGHT, i, j, current_color);
        }
    }

    write_ppm(pixels, WIDTH, HEIGHT, "out.ppm");

    return 0;
}
