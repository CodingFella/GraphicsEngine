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
#define BLACK 0x00000000


int min(int x1, int x2) {
    if(x1 < x2) {
        return x1;
    }
    return x2;
}

int max(int x1, int x2) {
    if(x1 > x2) {
        return x1;
    }
    return x2;
}

int abs (int x1) {
    if (x1 < 0) return -x1;
    return x1;
}

void draw_line(uint32_t *canvas, int width, int height, int x0, int y0, int x1, int y1, uint32_t color) {
    int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    for (;;){  /* loop */
        canvas[x0 + y0*width] = color;
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }

}


static uint32_t pixels[WIDTH*HEIGHT];

int main() {
    fill_screen(pixels, WIDTH, HEIGHT, BLACK);

    uint32_t colour = 0x00000000;

    draw_point(pixels, WIDTH, HEIGHT, 0, 1, colour);

    //draw_line(pixels, WIDTH, HEIGHT, 0, 0, WIDTH, 5, RED);

    for(size_t i = 0; i < WIDTH; i += 5) {
        uint8_t hue = (uint8_t)(((double)(i) / (double)(WIDTH)) * 255); // Map i to the range [0, 255]

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
        draw_line(pixels, WIDTH, HEIGHT, i, 0, WIDTH-i, HEIGHT, current_color);
    }
    for(size_t i = 0; i < HEIGHT; i += 5) {
        uint8_t hue = (uint8_t)(((double)(i) / (double)(WIDTH)) * 255); // Map i to the range [0, 255]

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
        draw_line(pixels, WIDTH, HEIGHT, 0, i, WIDTH, HEIGHT-i, current_color);
    }
//    for(size_t i = 0; i < WIDTH; ++i) {
//        for(size_t j = 0; j < HEIGHT; ++j) {
//            uint8_t hue = (uint8_t)(((double)(i + j) / (double)(WIDTH+HEIGHT)) * 255); // Map i to the range [0, 255]
//
//            // Convert the hue to RGB using a rainbow color scheme
//            uint8_t r, g, b;
//            if (hue < 85) {
//                r = 255 - hue * 3;
//                g = 0;
//                b = hue * 3;
//            } else if (hue < 170) {
//                hue -= 85;
//                r = 0;
//                g = hue * 3;
//                b = 255 - hue * 3;
//            } else {
//                hue -= 170;
//                r = hue * 3;
//                g = 255 - hue * 3;
//                b = 0;
//            }
//
//            // Combine the RGB components into a single uint32_t color value
//            uint32_t current_color = (r << 16) | (g << 8) | b;
//            draw_point(pixels, WIDTH, HEIGHT, i, j, current_color);
//        }
//    }

    write_ppm(pixels, WIDTH, HEIGHT, "out.ppm");

    return 0;
}
