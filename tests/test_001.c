//
// Created by Jacob Lin on 4/17/24.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <graphi.h>

#include "testing.h"

#define WIDTH 400
#define HEIGHT 300

static uint32_t buffer[WIDTH*HEIGHT];
#define BUFFER_SIZE 4096

int main() {
    graphi_canvas_t canvas = { buffer, WIDTH, HEIGHT };

    // test fill_screen. it should make the whole ppm one color
    fill_screen(canvas.buffer, canvas.width, canvas.height, TEAL);

    for (size_t i = 0; i < WIDTH; i += 5) {
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
        uint32_t color = (r << 16) | (g << 8) | b;
        draw_line(canvas.buffer, canvas.width, canvas.height, i, 0, WIDTH-i, HEIGHT-1, color);
    }

    const char *filename = "./tests/test_001.ppm";

    return compare_to_file(canvas, filename);
}
