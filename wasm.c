//
// Created by Jacob Lin on 5/4/24.
//

#include "graphi.c"

#define WIDTH 800
#define HEIGHT 600

#define PI 3.14159265359
#define TERMS 20


uint32_t pixels[WIDTH*HEIGHT];

double angle = 0;


double sin(double x) //sin calculation
{
    double sum;
    double fa;
    double pow;
    sum = 0.0;
    for(int i = 0; i <= TERMS; i++)
    {
        fa = 1.0;
        pow = 1.0;
        for(int j = 1; j <= 2*i+1; j++)
        {
            fa *= j;
            pow *= x;
        }
        sum += ((i%2?-1.0:1.0)/fa)*pow;
    }
    return sum;
}

double cos(double x) //cosine calculation
{
    double sum;
    double fa;
    double pow;
    sum = 0.0;
    for(int i = 0; i <= TERMS; i++)
    {
        fa = 1.0;
        pow = 1.0;
        for(int j = 1; j <= 2*i; j++)
        {
            fa *= j;
            pow *= x;
        }
        sum += ((i%2?-1.0:1.0)/fa)*pow;
    }
    return sum;
}


void rotate(int *x, int *y)
{
    int dx = *x - WIDTH/2;
    int dy = *y - HEIGHT/2;

    *x = (int)(dx * cos(angle) - dy * sin(angle)) + WIDTH/2;
    *y = (int)(dx * sin(angle) + dy * cos(angle)) + HEIGHT/2;
}

uint32_t* render(int dt)
{
    dt = dt % 360;
    angle = -PI * dt / 180.0;
//    angle += 2 * PI * dt;
//
//    int x1 = WIDTH/8, y1 = HEIGHT/8;
//    int x2 = WIDTH * 7/8, y2 = HEIGHT*7/8;
//    int x3 = WIDTH / 2, y3 = HEIGHT * 5/8;
//    rotate(&x1, &y1);
//    rotate(&x2, &y2);
//    rotate(&x3, &y3);


    int x1 = WIDTH/8, y1 = HEIGHT/4;
    int x2 = WIDTH*2/3, y2 = HEIGHT/2;
    int x3 = WIDTH/4, y3 = HEIGHT/8;

    rotate(&x1, &y1);
    rotate(&x2, &y2);
    rotate(&x3, &y3);


    fill_screen(pixels, WIDTH, HEIGHT, 0xFFFFFFFF);
    //draw_full_circle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, dt, 0xFFE6D7FF);
    //draw_line(pixels, WIDTH, HEIGHT, x1, y1, x2, y2, 0xFF000000);

    fill_triangle(pixels, WIDTH, HEIGHT, x1, y1, x2, y2, x3, y3, 0xFFE6D7FF);

    return pixels;
}


int main() {
    for(int i=270; i<360; i++)
        render(i);

    return 0;
}