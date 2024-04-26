//
// Created by Jacob Lin on 4/17/24.
//

#include <stdint.h>
#include <stdio.h>
#include "graphi.c"

#define WIDTH 400
#define HEIGHT 300


static uint32_t canvas[WIDTH*HEIGHT];

void test_draw_line() {
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
        draw_line(canvas, WIDTH, HEIGHT, i, 0, WIDTH-i, HEIGHT, GREEN);
    }
    write_ppm(canvas, WIDTH, HEIGHT, "./tests_out/draw_line_test.ppm");

}


void test_fill_screen() {
    fill_screen(canvas, WIDTH, HEIGHT, RED);
    write_ppm(canvas, WIDTH, HEIGHT, "./tests_out/fill_screen_test.ppm");
}

void test_draw_circle() {
    uint32_t curr_color = TEAL;
    for(size_t i = 0; i < HEIGHT; ++i) {
        draw_circle(canvas, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, i, curr_color);

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
    write_ppm(canvas, WIDTH, HEIGHT, "./tests_out/draw_circle_test.ppm");

}

int main(void) {

    // test fill_screen. it should make the whole ppm one color
    fill_screen(canvas, WIDTH, HEIGHT, WHITE);



    test_draw_circle();


    return 0;
}