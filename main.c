#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "graphi.c"

#define WIDTH 800
#define HEIGHT 600


void draw_8_points(uint32_t *canvas, int width, int height, int cx, int cy, int x, int y, uint32_t color) {
    draw_point(canvas, width, height, cx + x, cy + y, color);
    draw_point(canvas, width, height, cx + x, cy - y, color);
    draw_point(canvas, width, height, cx - x, cy + y, color);
    draw_point(canvas, width, height, cx - x, cy - y, color);
    draw_point(canvas, width, height, cx + y, cy + x, color);
    draw_point(canvas, width, height, cx + y, cy - x, color);
    draw_point(canvas, width, height, cx - y, cy + x, color);
    draw_point(canvas, width, height, cx - y, cy - x, color);
}


void draw_circle(uint32_t *canvas, int width, int height, int cx, int cy, int r, uint32_t color) {
    int error = -r;
    int x = r;
    int y = 0;

    while (y <= x) {
        draw_8_points(canvas, width, height, cx, cy, x, y, color);
        error += (y << 1) + 1;
        ++y;
        if(error >= 0) {
            error -= (x << 1) - 1;
            x -= 1;
        }
    }

}



static uint32_t pixels[WIDTH*HEIGHT];

int main() {
    fill_screen(pixels, WIDTH, HEIGHT, WHITE);

    uint32_t curr_color = TEAL;
    for(size_t i = 0; i < HEIGHT; ++i) {
        draw_circle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, i, curr_color);

        if(i/20 % 3 == 2) {
            curr_color = LILAC;
        }
        else if(i/20 % 3 == 1) {
            curr_color = TEAL;
        }
        else {
            curr_color = WHITE;
        }

    }



    write_ppm(pixels, WIDTH, HEIGHT, "out.ppm");

    return 0;
}
