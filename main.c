#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#include <time.h>

#include "graphi.c"

#define WIDTH 800
#define HEIGHT 600


void draw_full_circle(uint32_t *canvas, int width, int height, size_t cx, size_t cy, size_t r, uint32_t color) {
    // draw rectangle but check if f(0)
    for(size_t i = cx-r; i <= cx+r; ++i) {
        for(size_t j = cy-r; j <= cy+r; ++j) {
            double value = (double)((i-cx)*(i-cx)) + (double)((j-cy)*(j-cy)) - (double)(r*r);
            if(value <= r) {
                draw_point(canvas, width, height, i, j, color);
            }
        }
    }
}

void draw_4_lines(uint32_t *canvas, int width, int height, int cx, int cy, int x, int y, uint32_t color) {
    draw_line(canvas, width, height, cx + x, cy + y, cx + x, cy - y, color);
    draw_line(canvas, width, height, cx - x, cy + y, cx - x, cy - y, color);
    draw_line(canvas, width, height, cx + y, cy + x, cx + y, cy - x, color);
    draw_line(canvas, width, height, cx - y, cy + x, cx - y, cy - x, color);
}

void draw_full_circle_v2(uint32_t *canvas, int width, int height, int cx, int cy, int r, uint32_t color) {
    int error = -r;
    int x = r;
    int y = 0;

    while (y <= x) {
        draw_2_lines(canvas, width, height, cx, cy, x, y, color);
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


    clock_t start, end;
    double cpu_time_used;

    start = clock();
    draw_full_circle_v2(pixels, WIDTH, HEIGHT, 250, 250, 250, RED);
    //draw_circle(pixels, WIDTH, HEIGHT, 5, 5, 5, LILAC);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken: %f\n seconds", cpu_time_used);

    write_ppm(pixels, WIDTH, HEIGHT, "out2.ppm");



    return 0;
}
