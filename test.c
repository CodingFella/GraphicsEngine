//
// Created by Jacob Lin on 4/17/24.
//

#include <stdint.h>
#include <stdio.h>
#include "graphi.c"

#define WIDTH 400
#define HEIGHT 300

#define RED 0xFFFF0000

static uint32_t canvas[WIDTH][HEIGHT];

int main(void) {

    // test fill_screen. it should make the whole ppm one color
    fill_screen(canvas[0], WIDTH, HEIGHT, RED);
    int a = write_ppm(canvas[0], WIDTH, HEIGHT, "./tests_out/fill_screen_test.ppm");
    printf("%d", a);


    return 0;
}