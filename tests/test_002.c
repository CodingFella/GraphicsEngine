//
// Created by Evan Stokdyk on 5/10/25.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <graphi.h>
#include "testing.h"

#define WIDTH 400
#define HEIGHT 300

static uint32_t buffer[WIDTH*HEIGHT];

int main(void) {
    graphi_canvas_t canvas = { buffer, WIDTH, HEIGHT };

    fill_screen(canvas.buffer, canvas.width, canvas.height, WHITE);

    uint32_t curr_color = TEAL;
    for(size_t i = 0; i < HEIGHT; ++i) {

        // void draw_circle(uint32_t *canvas, size_t width, size_t height, int cx, int cy, int r, uint32_t color);
        draw_circle(canvas.buffer, canvas.width, canvas.height, WIDTH/2, HEIGHT/2, i, curr_color);

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

    const char *filename = "./tests/test_002.ppm";

    return compare_to_file(canvas, filename);
}
