//
// Created by Jacob Lin on 4/17/24.
//

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#define RED 0xFFFF0000
#define GREEN 0xFF00FF00
#define BLUE 0xFF0000FF
#define WHITE 0xFFFFFFFF
#define BLACK 0xFF000000
#define TEAL 0xFF008080
#define LILAC 0xFFE6D7FF

void fill_screen(uint32_t *canvas, size_t width, size_t height, u_int32_t color) {
    for (size_t i = 0; i<width*height; ++i) {
        canvas[i] = color;
    }
}

int write_ppm(const uint32_t *canvas, size_t width, size_t height, const char* file_name) {
    FILE *f = fopen(file_name, "wb");
    // failed
    if (f == NULL) {
        return -1;
    }

    fprintf(f, "P6\n%d %d\n255\n", (int) width, (int) height);

    for (size_t i = 0; i<width*height; ++i) {
        uint32_t current = *(canvas + i);

        uint8_t bytes[3] = {
                (current>>16)&0xFF,
                (current>>8)&0xFF,
                (current)&0xFF
        };

        // printf("Red: %d Green: %d Blue: %d\n", bytes[0], bytes[1], bytes[2]);

        fwrite(bytes, sizeof(bytes), 1, f);
    }

    fclose(f);

    return 0;
}

void draw_point(uint32_t *canvas, int width, int height, int x, int y, uint32_t color) {
    if(x< 0 || y < 0 || x >= width || y >= height) {
        return;
    }

    canvas[y*width + x] = color;
}

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

// draws line
// Input: pointer to canvas array, width and height of canvas, initial point (x0, y0), final point (x1, y1), color
// Output: void
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

// TODO: AA lines https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
