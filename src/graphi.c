//
// Created by Jacob Lin on 4/17/24.
//

#include <stdint.h>
#include <stddef.h>

#include "graphi.h"
#include "graphi/fonts.h"

#define GRAPHIC_SWAP(T, a, b) do { T t = a; a = b; b = t; } while(0)

void fill_screen(uint32_t *canvas, size_t width, size_t height, uint32_t color) {
    for (size_t i = 0; i<width*height; ++i) {
        canvas[i] = color;
    }
}


void draw_point(uint32_t *canvas, size_t width, size_t height, int x, int y, uint32_t color) {
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
void draw_line(uint32_t *canvas, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color) {
//    if(x0< 0 || y0 < 0 || x0 >= width || y0 >= height) {
//        return;
//    }
//    if(x1< 0 || y1 < 0 || x1 >= width || y1 >= height) {
//        return;
//    }
    int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    for (;;){  /* loop */
        //canvas[x0 + y0*width] = color;
        draw_point(canvas, width, height, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }

}

// TODO: AA lines https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm

// todo
void draw_thick_line(uint32_t *canvas, size_t width, size_t height, int x0, int y0, int x1, int y1, int thickness, uint32_t color) {

    for(int i=-thickness/2; i<thickness/2; i++) {
        draw_line(canvas, width, height, x0, y0, x1, y1, color);
    }
}

// helper function for draw_circle
void draw_8_points(uint32_t *canvas, size_t width, size_t height, int cx, int cy, int x, int y, uint32_t color) {
    draw_point(canvas, width, height, cx + x, cy + y, color);
    draw_point(canvas, width, height, cx + x, cy - y, color);
    draw_point(canvas, width, height, cx - x, cy + y, color);
    draw_point(canvas, width, height, cx - x, cy - y, color);
    draw_point(canvas, width, height, cx + y, cy + x, color);
    draw_point(canvas, width, height, cx + y, cy - x, color);
    draw_point(canvas, width, height, cx - y, cy + x, color);
    draw_point(canvas, width, height, cx - y, cy - x, color);
}


void draw_circle(uint32_t *canvas, size_t width, size_t height, int cx, int cy, int r, uint32_t color) {
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

// helper function for draw_full_circle
void draw_4_lines(uint32_t *canvas, int width, int height, int cx, int cy, int x, int y, uint32_t color) {
    draw_line(canvas, width, height, cx + x, cy + y, cx + x, cy - y, color);
    draw_line(canvas, width, height, cx - x, cy + y, cx - x, cy - y, color);
    draw_line(canvas, width, height, cx + y, cy + x, cx + y, cy - x, color);
    draw_line(canvas, width, height, cx - y, cy + x, cx - y, cy - x, color);
}

void draw_full_circle(uint32_t *canvas, int width, int height, int cx, int cy, int r, uint32_t color) {
    int error = -r;
    int x = r;
    int y = 0;

    while (y <= x) {
        draw_4_lines(canvas, width, height, cx, cy, x, y, color);
        error += (y << 1) + 1;
        ++y;
        if(error >= 0) {
            error -= (x << 1) - 1;
            x -= 1;
        }
    }
}




void sort_points(int *x1, int *y1, int *x2, int *y2, int *x3, int *y3) {

    // p1 p2 p3
    if(*y2 < *y1) {
        GRAPHIC_SWAP(int, *y1, *y2);
        GRAPHIC_SWAP(int, *x1, *x2);
    }

    if(*y3 < *y2) {
        GRAPHIC_SWAP(int, *y2, *y3);
        GRAPHIC_SWAP(int, *x2, *x3);

    }

    if(*y2 < *y1) {
        GRAPHIC_SWAP(int, *y1, *y2);
        GRAPHIC_SWAP(int, *x1, *x2);

    }
}


void draw_triangle(uint32_t *canvas, size_t width, size_t height,
                   int x1, int y1,
                   int x2, int y2,
                   int x3, int y3, uint32_t color) {
    draw_line(canvas, width, height, x1, y1, x2, y2, color);
    draw_line(canvas, width, height, x1, y1, x3, y3, color);
    draw_line(canvas, width, height, x3, y3, x2, y2, color);

}


//             * (x1, y1)
//           *      *
//         *            *
//       * (x2, y2) ******** (x3, y3)

void fill_flat_bottom_triangle(uint32_t *canvas, size_t width, size_t height,
                               int x1, int y1,
                               int x2, int y2,
                               int x3, int y3, uint32_t color) {

    float slope1_2 = (float)(x2 - x1) / (float)(y2 - y1);
    float slope1_3 = (float)(x3 - x1) / (float)(y3 - y1);

    float curr_x_left = (float)x1;
    float curr_x_right = (float)x1;

    for(int curr_y = y1; curr_y <= y2; curr_y++) {
        draw_line(canvas, width, height, (int)(curr_x_left - 0.5), (int)(curr_y), (int)(curr_x_right+0.5), (int)(curr_y), color);

        curr_x_left += slope1_2;
        curr_x_right += slope1_3;
    }

}


//       * (x1, y1) ******** (x2, y3)
//         *            *
//           *      *
//             * (x3, y3)

void fill_flat_top_triangle(uint32_t *canvas, size_t width, size_t height,
                            int x1, int y1,
                            int x2, int y2,
                            int x3, int y3, uint32_t color) {

    float slope1_3 = (float)(x3 - x1) / (float)(y3 - y1);
    float slope2_3 = (float)(x3 - x2) / (float)(y3 - y2);

    float curr_x_left = (float)x3;
    float curr_x_right = (float)x3;

    for(int curr_y = y3; curr_y > y1; curr_y--) {
        draw_line(canvas, width, height, (int)(curr_x_left), curr_y, (int)(curr_x_right+0.5), curr_y, color);

        curr_x_left -= slope1_3;
        curr_x_right -= slope2_3;
    }

}

void fill_triangle(uint32_t *canvas, size_t width, size_t height,
                   int x1, int y1,
                   int x2, int y2,
                   int x3, int y3, uint32_t color) {

    sort_points(&x1, &y1, &x2, &y2, &x3, &y3);


    // p1 < p2 < p3



    int mid_x = (int)((float)x1 + ((float)(y2 - y1) / (float)(y3 - y1)) * (float)(x3 - x1));

    fill_flat_bottom_triangle(canvas, width, height, x1, y1, x2, y2, mid_x, y2, color);
    fill_flat_top_triangle(canvas, width, height, x2, y2, mid_x, y2, x3, y3, color);
}

void draw_rect(uint32_t *canvas, size_t width, size_t height, int x, int y, int w, int h, uint32_t color) {
    if(x + w >= width || y + h >= height) {
        return;
    }

    // TODO: it may be best to optimize for drawing triangles as they are the
    // most used but for right now any implementation will need to set each
    // pixel so its mute. but worth thinking about if I ever do simd things.
    for(int i=x; i<x+w; i++) {
        draw_line(canvas, width, height, i, y, i, y + h - 1, color);
    }
}


void draw_text(uint32_t *data, size_t width, size_t height, const char *text, int tx, int ty, int font_size, int spacing, uint32_t color) {
    graphi_canvas_t canvas = { data, width, height };
    for (size_t i = 0; *text; ++i, ++text) {
        int char_anchor_x = tx + font_size * i * (DEFAULT_FONT_WIDTH + spacing);
        int char_anchor_y = ty;

        graphi_draw_letter(canvas, *text, char_anchor_x, char_anchor_y, font_size, color);
    }
}

void graphi_draw_letter(graphi_canvas_t canvas, char letter, int tx, int ty, int font_size, uint32_t color) {
    if (letter < 0 || letter >= 128) return;

    const char *glyph = &graphic_glyphs[letter][0][0];

    for (int j = 0; j < DEFAULT_FONT_HEIGHT; j++) {
        for (int k = 0; k < DEFAULT_FONT_WIDTH; k++) {
            if (glyph[j * DEFAULT_FONT_WIDTH + k]) {
                draw_rect(canvas.buffer, canvas.width, canvas.height, tx + k * font_size, ty + j * font_size, font_size, font_size, color);
            }
        }
    }
}

#ifndef GRAPHI_NO_LIBC

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // For memcpy

size_t graphi_write_ppm(graphi_canvas_t canvas, char **outbuffer) {
    //    (width * height * 3 bytes for RGB) + // "P6\n4294967295 4294967295\n255\n" is less than 20 chars
    size_t allocation = (canvas.width * canvas.height * 3) + 20;

    char *buffer = malloc(allocation);
    if (buffer == NULL) return 0;

    size_t offset = snprintf(buffer, allocation, "P6\n%zu %zu\n255\n", canvas.width, canvas.height);

    // Write pixel data
    for (size_t i = 0; i < canvas.width * canvas.height; ++i) {
        uint32_t pixel = *(canvas.buffer + i);

        // Assuming ARGB or 0x00RRGGBB format
        // Extract Red, Green, Blue components
        buffer[offset++] = (pixel >> 16) & 0xFF; // Red
        buffer[offset++] = (pixel >> 8) & 0xFF;  // Green
        buffer[offset++] = (pixel) & 0xFF;       // Blue
    }

    *outbuffer = buffer;
    return offset;
}

#endif
