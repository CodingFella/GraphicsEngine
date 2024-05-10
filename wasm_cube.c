//
// Created by Jacob Lin on 5/7/24.
//

// clang --target=wasm32 --no-standard-libraries -Wl,--export-all -Wl,--no-entry -o wasm_cube.wasm wasm_cube.c

#include "graphi.c"

#define WIDTH 800
#define HEIGHT 600

#define TERMS 20

#define PI 3.14159265359

#define NUM_PLANES 6
#define PLANE_POINTS 4

void *memcpy(void *dest, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((char*)dest)[i] = ((char*)src)[i];
    }
}

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

struct Point_3D {
    float x, y, z;
};

struct Point_2D {
    float x, y;
};

struct plane {
    struct Point_3D pointA;
    struct Point_3D pointB;
    struct Point_3D pointC;
    struct Point_3D pointD;

    struct Point_2D s_pointA;
    struct Point_2D s_pointB;
    struct Point_2D s_pointC;
    struct Point_2D s_pointD;

    uint32_t color;
};

float get_average_z(struct plane input) {
    return (input.pointA.z + input.pointB.z + input.pointC.z + input.pointD.z) / (float)4.0;
}

void swap(struct plane *xp, struct plane *yp)
{
    struct plane temp = *xp;
    *xp = *yp;
    *yp = temp;
}


void draw_planes(uint32_t *canvas, int width, int height,
                 struct plane p0123, struct plane p5140, struct plane p4062,
                         struct plane p5476, struct plane p5173, struct plane p7362) {
    // sort planes based on average z value

    struct plane p[NUM_PLANES] = {p0123, p5140, p4062, p5476, p5173, p7362};
    int i, j, min_idx;

    // One by one move boundary of unsorted subarray
    for (i = 0; i < NUM_PLANES-1; i++)
    {
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i+1; j < NUM_PLANES; j++)
            if (get_average_z(p[j]) > get_average_z(p[min_idx]))
                min_idx = j;

        // Swap the found minimum element with the first element
        if(min_idx != i)
            swap(&p[min_idx], &p[i]);
    }
    // paint accordingly

    for(i = 3; i < NUM_PLANES; i++) {
        fill_triangle(canvas, WIDTH, HEIGHT, p[i].s_pointA.x, p[i].s_pointA.y, p[i].s_pointB.x, p[i].s_pointB.y, p[i].s_pointC.x, p[i].s_pointC.y, p[i].color);
        fill_triangle(canvas, WIDTH, HEIGHT, p[i].s_pointD.x, p[i].s_pointD.y, p[i].s_pointB.x, p[i].s_pointB.y, p[i].s_pointC.x, p[i].s_pointC.y, p[i].color);
    }
}


void draw_cube(uint32_t *canvas, int width, int height, struct Point_3D *corners, int num_corners, struct Point_3D camera, uint32_t color) {

    struct Point_2D c_2D[num_corners];
    for(size_t i = 0; i < num_corners; ++i) {
        struct Point_3D curr = corners[i];
        float u, v;
        float x, y, z;
        x = curr.x - camera.x;
        y = curr.y - camera.y;
        z = curr.z - camera.z;

        if(z == 0) {
            fill_screen(canvas, width, height, color);
            break;
        }

        u = (x / z) * 3000.0 + WIDTH/2.0;
        v = (y / z) * 3000.0 + HEIGHT/2.0;

        draw_point(canvas, width, height, u, v, color);

        struct Point_2D p = {u, v};
        c_2D[i] = p;
    }

    // 01 04 13 26 67 57 37 23 02 15 45 46
    // 01 23 45 67
    // 02 13 46 57
    // 04 26 15 37
//    draw_line(canvas, WIDTH, HEIGHT, c[0].x, c[0].y, c[1].x, c[1].y, TEAL);
//    draw_line(canvas, WIDTH, HEIGHT, c[2].x, c[2].y, c[3].x, c[3].y, TEAL);
//    draw_line(canvas, WIDTH, HEIGHT, c[0].x, c[0].y, c[2].x, c[2].y, TEAL);
//    draw_line(canvas, WIDTH, HEIGHT, c[1].x, c[1].y, c[3].x, c[3].y, TEAL);
//    draw_line(canvas, WIDTH, HEIGHT, c[4].x, c[4].y, c[5].x, c[5].y, LILAC);
//    draw_line(canvas, WIDTH, HEIGHT, c[6].x, c[6].y, c[7].x, c[7].y, LILAC);
//    draw_line(canvas, WIDTH, HEIGHT, c[4].x, c[4].y, c[6].x, c[6].y, LILAC);
//    draw_line(canvas, WIDTH, HEIGHT, c[5].x, c[5].y, c[7].x, c[7].y, LILAC);
//    draw_line(canvas, WIDTH, HEIGHT, c[0].x, c[0].y, c[4].x, c[4].y, color);
//    draw_line(canvas, WIDTH, HEIGHT, c[2].x, c[2].y, c[6].x, c[6].y, color);
//    draw_line(canvas, WIDTH, HEIGHT, c[1].x, c[1].y, c[5].x, c[5].y, color);
//    draw_line(canvas, WIDTH, HEIGHT, c[3].x, c[3].y, c[7].x, c[7].y, color);

    // faces
//    fill_triangle(canvas, WIDTH, HEIGHT, c[0].x, c[0].y, c[1].x, c[1].y, c[2].x, c[2].y, GREEN);
//    fill_triangle(canvas, WIDTH, HEIGHT, c[3].x, c[3].y, c[1].x, c[1].y, c[2].x, c[2].y, GREEN);
//
//    fill_triangle(canvas, WIDTH, HEIGHT, c[4].x, c[4].y, c[5].x, c[5].y, c[6].x, c[6].y, BLUE);
//    fill_triangle(canvas, WIDTH, HEIGHT, c[5].x, c[5].y, c[6].x, c[6].y, c[7].x, c[7].y, BLUE);
//
//    fill_triangle(canvas, WIDTH, HEIGHT, c[1].x, c[1].y, c[0].x, c[0].y, c[4].x, c[4].y, LILAC);
//    fill_triangle(canvas, WIDTH, HEIGHT, c[1].x, c[1].y, c[4].x, c[4].y, c[5].x, c[5].y, LILAC);


    struct plane p0123 = {corners[0], corners[1], corners[2], corners[3], c_2D[0], c_2D[1], c_2D[2], c_2D[3], BLUE};
    struct plane p5140 = {corners[5], corners[1], corners[4], corners[0], c_2D[5], c_2D[1], c_2D[4], c_2D[0], YELLOW};
    struct plane p4062 = {corners[4], corners[0], corners[6], corners[2], c_2D[4], c_2D[0], c_2D[6], c_2D[2], RED};
    struct plane p5476 = {corners[5], corners[4], corners[7], corners[6], c_2D[5], c_2D[4], c_2D[7], c_2D[6], GREEN};
    struct plane p5173 = {corners[5], corners[1], corners[7], corners[3], c_2D[5], c_2D[1], c_2D[7], c_2D[3], ORANGE};
    struct plane p7362 = {corners[7], corners[3], corners[6], corners[2], c_2D[7], c_2D[3], c_2D[6], c_2D[2], WHITE};

    draw_planes(canvas, WIDTH, HEIGHT, p0123, p5140, p4062, p5476, p5173, p7362);

}


struct Point_3D rotate(struct Point_3D point, double mat[][3]) {

}
float A, B, C;


float calculateX(float i, float j, float k) {
    return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
           j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
}

float calculateY(float i, float j, float k) {
    return j * cos(A) * cos(C) + k * sin(A) * cos(C) -
           j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) -
           i * cos(B) * sin(C);
}

float calculateZ(float i, float j, float k) {
    return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

static uint32_t pixels[WIDTH*HEIGHT];

void f_modulo(float* x, float y) {
    *x =  *x - y * (float)(int)(*x/y);
}
float angle = 0;

uint32_t* render(int dt, float a, float b, float c) {
    dt = dt % 360;
    angle = PI * dt / 180.0;
    fill_screen(pixels, WIDTH, HEIGHT, WHITE);

    A = a;
    B = b;
    C = c;

    f_modulo(&A, 2*PI);
    f_modulo(&B, 2*PI);
    f_modulo(&C, 2*PI);

    float magnitude = 11;
    float camera_distance = 200;
    struct Point_3D points[8] = {
            { magnitude, magnitude, camera_distance +magnitude},
            {-magnitude, magnitude, camera_distance +magnitude},
            {magnitude, -magnitude, camera_distance +magnitude},
            {-magnitude, -magnitude, camera_distance +magnitude},
            { magnitude, magnitude, camera_distance -magnitude},
            {-magnitude, magnitude, camera_distance -magnitude},
            { magnitude, -magnitude, camera_distance -magnitude},
            {-magnitude, -magnitude, camera_distance -magnitude},
    };

    for(int j=0; j<8; j++) {
        struct Point_3D new_point = {calculateX(points[j].x, points[j].y, points[j].z - camera_distance),calculateY(points[j].x, points[j].y, points[j].z - camera_distance),calculateZ(points[j].x, points[j].y, points[j].z - camera_distance) + camera_distance};
        points[j] = new_point;
    }

    fill_screen(pixels, WIDTH, HEIGHT, BLACK);

    struct Point_3D camera = {0, 0, 0};

    draw_cube(pixels, WIDTH, HEIGHT, points, 8, camera, RED);



    return pixels;
}
