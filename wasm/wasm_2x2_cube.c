//
// Created by Jacob Lin on 5/7/24.
//

// clang --target=wasm32 --no-standard-libraries -Wl,--export-all -Wl,--no-entry -o wasm_cube.wasm wasm_cube.c
// clang --target=wasm32 --no-standard-libraries -Wl,--export-all -Wl,--no-entry -o abcd.wasm wasm_2x2_cube.c

#include <graphi.h>
#include "wasmmath.h"

#define WIDTH 800
#define HEIGHT 600

#define NUM_PLANES 6
#define NUM_CORNERS 8

// 1.2 0.13 is nice
#define SCALE 1.2
#define GAP (0.13)

#define BG_COLOR BLACK

#define MODE_LENGTH_I 0
#define MODE_WIDTH_J 1
#define MODE_HEIGHT_K 2

#define FLT_MAX 3.402823466e+38F /* max value */
#define FLT_MIN (-FLT_MAX)       /* min positive value */

int DIMENSION = 4;

float A, B, C;

int hms = 0;
static uint32_t pixels[WIDTH * HEIGHT];

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

struct cube {
    struct Point_3D points[NUM_CORNERS];
    uint32_t currState;
};

typedef struct {
    int first;
    int second;
} Pair;

int get_num_active_planes(uint32_t state);

/*
 *  Function:   get_average_z
 *  -------------------------
 *  Given a plane as input, it finds the average of each corner's z values, so it can know
 *      if it is in front or behind in relation to the other planes
 *
 *  Input param:
 *      struct plane input
 *
 *  Return:
 *      Returns the average z value
 */
float get_average_z(struct plane input) {
    return (input.pointA.z + input.pointB.z + input.pointC.z + input.pointD.z) / (float) NUM_PLANES;
}

/*
 *  Function:   get_average_z_corners
 *  ---------------------------------
 *  Given a Point_3D as input, it finds the average of each corner's z values, so it can know
 *      if it is in front or behind in relation to the other cubes
 *
 *  Input param:
 *      struct Point_3D *input
 *
 *  Return:
 *      Returns the average z value
 */
float get_average_z_corners(struct Point_3D *input) {
    float sum = 0;
    for (int i = 0; i < NUM_CORNERS; i++) {
        sum += input[i].z;
    }
    return sum;
}

/*
 *  Function:   draw_planes
 *  -----------------------
 *  Given an array of planes, it will draw the planes on the canvas in the correct
 *      order of back to front by sorting the planes by its z value.
 *
 *  Input params:
 *      uint32_t *canvas:       array of pixels on screen
 *      int width:              width of canvas
 *      int height:             height of canvas
 *      struct plane *p:        array of planes to be drawn
 *      int count:              number of planes (size of struct plane *p)
 *
 *  Return:
 *      No return value. Places pixel into canvas.
 */
void draw_planes(uint32_t *canvas, int width, int height,
                 struct plane *p, int count) {
    // sort planes based on average z value
    int i, j, min_idx;

    // selection sort
    for (i = 0; i < count - 1; i++) {
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < count; j++)
            if (get_average_z(p[j]) > get_average_z(p[min_idx]))
                min_idx = j;

        // Swap the found minimum element with the first element
        if (min_idx != i) {
            struct plane temp = p[min_idx];
            p[min_idx] = p[i];
            p[i] = temp;
        }
    }

    // paint accordingly
    for (i = count/2; i < count; i++) {
        if(GAP == 0 && p[i].color == BG_COLOR) {
            continue;
        }
        else {
            fill_triangle(canvas, width, height,
                          (int) (p[i].s_pointA.x + 0.5), (int) (p[i].s_pointA.y + 0.5),
                          (int) (p[i].s_pointB.x + 0.5), (int) (p[i].s_pointB.y + 0.5),
                          (int) (p[i].s_pointC.x + 0.5), (int) (p[i].s_pointC.y + 0.5),
                          p[i].color);

            fill_triangle(canvas, width, height,
                          (int) (p[i].s_pointD.x + 0.5), (int) (p[i].s_pointD.y + 0.5),
                          (int) (p[i].s_pointB.x + 0.5), (int) (p[i].s_pointB.y + 0.5),
                          (int) (p[i].s_pointC.x + 0.5), (int) (p[i].s_pointC.y + 0.5),
                          p[i].color);
        }
    }
}

/*
 *  Function:   convert_3D_to_2D
 *  ----------------------------
 *  Given the dimensions of the canvas and the set of 3D points, relative to the camera,
 *      it converts the 3D coordinates and places its equivalent 2D points into the 2D
 *      array.
 *
 *  Input params:
 *      struct Point_2D* point2D:   pointer to store the converted 2D points
 *      int width:                  width of canvas
 *      int height:                 height of canvas
 *      struct Point_3D* points:    pointer to 3D points to be converted
 *      int num_corners:            number of corners
 *      struct Point_3D camera:
 *
 *  Return:
 *      No return value. Computed cubes are stored in cubes and the proper image is
 *          drawn into the canvas.
 */
void convert_3D_to_2D(struct Point_2D *point2D, int width, int height, struct Point_3D *points, struct Point_3D camera) {

    for (size_t i = 0; i < NUM_CORNERS; ++i) {
        struct Point_3D curr = points[i];
        float u, v;
        float x, y, z;
        x = curr.x - camera.x;
        y = curr.y - camera.y;
        z = curr.z - camera.z;


        u = (x / z) * 3000.0 + width / 2.0;
        v = (y / z) * 3000.0 + height / 2.0;

        struct Point_2D p = {u, v};
        point2D[i] = p;
    }
}

/*
 *  Function:   draw_cube
 *  ---------------------
 *  Given the canvas, the corners of the cube, the camera, face colors, and planes to
 *      consider, this function draws the cube to its required specifications.
 *
 *  Input params:
 *      struct cube* cubes:     array of cubes that the generated cubes will be stored in
 *      int num_cubes:          number of cubes in the Rubik's Cube
 *      float magnitude:        how long the dimensions of each cube is
 *      float camera_distance:  how far away the camera is from the cube
 *
 *  Return:
 *      No return value. Computed cubes are stored in cubes and the proper image is
 *          drawn into the canvas.
 */
void draw_cube(uint32_t *canvas, int width, int height, struct Point_3D *points,
               struct Point_3D camera, const uint32_t *colors, uint32_t curr_color) {
    struct Point_2D c_2D[NUM_CORNERS];

    convert_3D_to_2D(c_2D, width, height, points, camera);

    struct plane p0123 = {points[0], points[1], points[2], points[3],
                          c_2D[0], c_2D[1], c_2D[2], c_2D[3], BG_COLOR};
    struct plane p5140 = {points[5], points[1], points[4], points[0],
                          c_2D[5], c_2D[1], c_2D[4], c_2D[0], BG_COLOR};
    struct plane p4062 = {points[4], points[0], points[6], points[2], c_2D[4], c_2D[0], c_2D[6], c_2D[2],
                          BG_COLOR};
    struct plane p5476 = {points[5], points[4], points[7], points[6], c_2D[5], c_2D[4], c_2D[7], c_2D[6],
                          BG_COLOR};
    struct plane p5173 = {points[5], points[1], points[7], points[3], c_2D[5], c_2D[1], c_2D[7], c_2D[3],
                          BG_COLOR};
    struct plane p7362 = {points[7], points[3], points[6], points[2], c_2D[7], c_2D[3], c_2D[6], c_2D[2],
                          BG_COLOR};

    struct plane planes[NUM_PLANES] = {p0123, p5140, p4062, p5476, p5173, p7362};

    for (int i = 0; i < 3; i++) {
        uint32_t color_id = ((curr_color << (32 - 6 * (i + 1) + 3)) >> 29);
        uint32_t plane_id = ((curr_color << (32 - 6 * (i + 1)) >> 29));
        if (plane_id != 7 && color_id != 7) {
            planes[plane_id].color = colors[color_id];
        }
    }

    if(DIMENSION == 1) {
        for (int i = 0; i < NUM_PLANES; i++) {
            planes[i].color = colors[i];
        }
    }

    draw_planes(canvas, WIDTH, HEIGHT, planes, NUM_PLANES);
}


/*
 *  Functions:      calculateX, calculateY, calculateZ
 *  -------------------
 *  Given the current coordinates, it will rotate it using linear algebra to its
 *      correct orientation.
 *
 *  Input params:
 *      struct cube* cubes:     array of cubes that the generated cubes will be stored in
 *      int num_cubes:          number of cubes in the Rubik's Cube
 *      float magnitude:        how long the dimensions of each cube is
 *      float camera_distance:  how far away the camera is from the cube
 *
 *  Return:
 *      No return value. Computed cubes are stored in cubes and the proper image is
 *          drawn into the canvas.
 */
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


/*
 *  Function:    f_modulo
 *  ---------------------
 *  Given the initial value and the mod value, this function will modify
 *      the float value that it is pointed to
 *
 *  Runtime Complexity: O(1)
 *
 *  Input params:
 *      float *x:   pointer to initial dividend value
 *      float y:    modulus divisor value
 *
 *  Return:
 *      No return value. The modulo value is placed directly into the input float.
 */
void f_modulo(float *x, float y) {
    *x = *x - y * (float) (int) (*x / y);
}


/*
 *  Function:    generateCorners
 *  ----------------------------
 *  Given the magnitude, camera distance, offsets in the x, y, and z axis,
 *      this function places the correct corner coordinates into the cube
 *      pointer that is passed in. By default, the cube will's center will
 *      be at (0,0,0).
 *
 *  Runtime Complexity: O(1)
 *
 *  Input params:
 *      struct cube *cube:      cube object where the corners will be set
 *      float magnitude:        length of cube size
 *      float camera_distance:  distance of camera from cube
 *      float x_offset:         amount of offset in the x-direction
 *      float y_offset:         amount of offset in the y-direction
 *      float z_offset:         amount of offset in the z-direction
 *
 *  Return:
 *      No return value. Computed corners are placed into the cube pointer
 *          that was passed in.
 */
void generateCorners(struct cube *cube, float magnitude, float camera_distance, float x_offset, float y_offset, float z_offset, int mode, float angle) {

    float x_factor = -1;
    float y_factor = -1;
    float z_factor = -1;

    for (int i = 0; i < NUM_CORNERS; i++) {
        struct Point_3D newPoint;
        // to get all combinations! like truth table thing to get all combos
        x_factor *= -1;
        if ((i % 2) == 0) y_factor *= -1;
        if ((i % 4) == 0) z_factor *= -1;

        newPoint.x = x_factor * magnitude + x_offset;
        newPoint.y = y_factor * magnitude + y_offset;
        newPoint.z = camera_distance + z_factor * magnitude + z_offset;

        if(angle == 0) {
            cube->points[i] = newPoint;
        }
        else {
            struct Point_3D rotatedNewPoint;
            float rad = angle * PI / 180.0;
            if(mode == MODE_LENGTH_I) {
                rotatedNewPoint.x = newPoint.x;
                rotatedNewPoint.y = newPoint.y * cos(rad) - (newPoint.z-camera_distance) * sin(rad);
                rotatedNewPoint.z = newPoint.y * sin(rad) + (newPoint.z-camera_distance) * cos(rad) + camera_distance;
                cube->points[i] = rotatedNewPoint;
            } else if(mode == MODE_WIDTH_J) {
                rotatedNewPoint.x = newPoint.x * cos(rad) + (newPoint.z-camera_distance) * sin(rad);
                rotatedNewPoint.y = newPoint.y;
                rotatedNewPoint.z = - newPoint.x * sin(rad) + (newPoint.z-camera_distance) * cos(rad) + camera_distance;
                cube->points[i] = rotatedNewPoint;
            } else if(mode == MODE_HEIGHT_K) {
                rotatedNewPoint.x = newPoint.x * cos(rad) - newPoint.y * sin(rad);
                rotatedNewPoint.y = newPoint.x * sin(rad) + newPoint.y * cos(rad);
                rotatedNewPoint.z = newPoint.z;
                cube->points[i] = rotatedNewPoint;
            }

        }

    }
}

/*
 *  Function:    copy_cube
 *  ----------------------
 *  Given destination and source, copies fields from source to destination.
 *
 *  Input params:
 *      struct cube *dest:      pointer to destination cube (paste)
 *      struct cube *src:       pointer to source cube (copy)
 *
 *  Return:
 *      No return value. Copied fields go directly into *dest.
 */
void copy_cube(struct cube *dest, struct cube *src) {
    for (int i = 0; i < NUM_CORNERS; i++) {
        dest->points[i] = src->points[i];
    }
    //dest->currState = src->currState;
}

/*
 *  Function:    copy_3D_point
 *  --------------------------
 *  Given destination and source, copies fields from source to destination.
 *
 *  Input params:
 *      struct Point_3D *dest:  pointer to destination cube (paste)
 *      struct Point_3D *src:   pointer to source cube (copy)
 *
 *  Return:
 *      No return value. Copied fields go directly into *dest.
 */
void copy_3D_point(struct Point_3D *dest, struct Point_3D *src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

/*
 *  Function:    load_default
 *  -------------------------
 *  Given the current cube, loads its current state based on the default, solved cube.
 *      Using bit-masking and binary operations, it will modify the bits to make it
 *      store the correct values.
 *
 *  Input params:
 *      uint32_t *curr_cube:    pointer to current cube whose current state is being set
 *      int planeOfInterest:    plane to set
 *      int plane_count:        used to find amount of offset for bitmask to work. each one can
 *                                  have up to 3 planes
 *
 *  Return:
 *      No return value. Default current state goes directly into *curr_cube
 */
void load_default(uint32_t *curr_cube, int planeOfInterest, int plane_count) {
    // clear out portion
    uint32_t mask = (1 << 6) - 1;
    mask = mask << plane_count * 6;
    mask = ~mask;

    *curr_cube = *curr_cube & mask;

    uint32_t plane_id = (planeOfInterest) << (6 * plane_count + 3);
    uint32_t color_id = (planeOfInterest) << (6 * plane_count);

    *curr_cube += plane_id;
    *curr_cube += color_id;
}


/*
 *  Function:    generateCubes
 *  --------------------------
 *  given the number of cubes, magnitude, and camera distance, this function generates
 *      the correct corner coordinates for each cube, finds the active planes (i.e.
 *      corner cube has 3 active ones, edges have 2, and faces have 1). Lastly, it
 *      sorts the cubes based on the average z coordinates of the corners so that it
 *      is painted in the right order.
 *
 *  Input params:
 *      struct cube* cubes:     array of cubes that the generated cubes will be stored in
 *      int num_cubes:          number of cubes in the Rubik's Cube
 *      float magnitude:        how long the dimensions of each cube is
 *      float camera_distance:  how far away the camera is from the cube
 *
 *  Return:
 *      No return value. Computed cubes are stored in cubes and the proper image is
 *          drawn into the canvas.
 */
struct cube *generateCubes(struct cube *cubes, struct cube *translatedCubes, int num_cubes, float magnitude, float camera_distance, int mode, int location, float angle) {
    int i, j, k;

    const int cubes_rows = DIMENSION;
    const int cubes_cols = DIMENSION;
    const int cubes_height = DIMENSION;


    for (i = 0; i < num_cubes; i++) {
        for (j = 0; j < NUM_CORNERS; j++) {
            translatedCubes[i].points[j] = cubes[i].points[j];
        }
        translatedCubes[i].currState = cubes[i].currState;
    }

    float spacing = (float) (SCALE * 2);

    float x_offset = -spacing * ((float)(DIMENSION - 1) / (float) 2);
    float y_offset = -spacing * ((float)(DIMENSION - 1) / (float) 2);
    float z_offset = -spacing * ((float)(DIMENSION - 1) / (float) 2);

    int count = 0;


    // stickers!
    for (i = 0; i < cubes_rows; i++) {
        for (j = 0; j < cubes_cols; j++) {
            for (k = 0; k < cubes_height; k++) {
                if (i == 0 || i == cubes_rows - 1 ||
                    j == 0 || j == cubes_cols - 1 ||
                    k == 0 || k == cubes_height - 1) {
                    if(mode == MODE_LENGTH_I && i == location || mode == MODE_WIDTH_J && j == location || mode == MODE_HEIGHT_K && k == location) {
                        generateCorners(&translatedCubes[count], magnitude, camera_distance,
                                        x_offset + (spacing * (float) i),
                                        y_offset + (spacing * (float) j), z_offset + (spacing * (float) k), mode, angle);
                    } else {
                        generateCorners(&translatedCubes[count], magnitude, camera_distance, x_offset + (spacing * (float) i),
                                            y_offset + (spacing * (float) j), z_offset + (spacing * (float) k), mode, 0);
                    }

                    count++;
                }
            }
        }
    }

    for (i = 0; i < num_cubes; i++) {
        copy_cube(&cubes[i], &translatedCubes[i]);
    }

    // rotate corners for each cube
    for (i = 0; i < num_cubes; i++) {
        for (j = 0; j < NUM_CORNERS; j++) {
            struct Point_3D new_point = {calculateX(translatedCubes[i].points[j].x, translatedCubes[i].points[j].y, translatedCubes[i].points[j].z - camera_distance),
                                         calculateY(translatedCubes[i].points[j].x, translatedCubes[i].points[j].y, translatedCubes[i].points[j].z - camera_distance),
                                         calculateZ(translatedCubes[i].points[j].x, translatedCubes[i].points[j].y, translatedCubes[i].points[j].z - camera_distance) + camera_distance};
            translatedCubes[i].points[j] = new_point;
        }
    }

    // draw the cubes!
    // selection sort
    int min_idx;
    for (i = 0; i < num_cubes - 1; i++) {
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < num_cubes; j++)
            if (get_average_z_corners(translatedCubes[j].points) > get_average_z_corners(translatedCubes[min_idx].points))
                min_idx = j;

        // Swap the found minimum element with the first element
        struct Point_3D temp;
        uint32_t temp_state;
        if (min_idx != i) {
            for (j = 0; j < NUM_CORNERS; j++) {
                temp = translatedCubes[min_idx].points[j];
                translatedCubes[min_idx].points[j] = translatedCubes[i].points[j];
                translatedCubes[i].points[j] = temp;
            }

            temp_state = translatedCubes[min_idx].currState;
            translatedCubes[min_idx].currState = translatedCubes[i].currState;
            translatedCubes[i].currState = temp_state;

            //            temp_state = currState[min_idx];
            //            currState[min_idx] = currState[i];
            //            currState[i] = temp_state;
        }
    }

    return translatedCubes;
}

void draw_guide_lines(struct Point_2D *translatedPoints) {
    // draw neighbor line
    int i;
    for (i = 0; i < NUM_CORNERS; i += 2) {
        draw_line(pixels, WIDTH, HEIGHT, (int) (translatedPoints[i].x + 0.5), (int) (translatedPoints[i].y + 0.5), (int) (translatedPoints[i + 1].x + 0.5),
                  (int) (translatedPoints[i + 1].y + 0.5), HOTPINK);
    }

    // draw long line
    for (i = 0; i < NUM_CORNERS; i += 2) {
        draw_line(pixels, WIDTH, HEIGHT, (int) (translatedPoints[i].x + 0.5), (int) (translatedPoints[i].y + 0.5), (int) (translatedPoints[(i + 2) % 8].x + 0.5), (int) (translatedPoints[(i + 2) % 8].y + 0.5), HOTPINK);
        draw_line(pixels, WIDTH, HEIGHT, (int) (translatedPoints[i + 1].x + 0.5), (int) (translatedPoints[i + 1].y + 0.5), (int) (translatedPoints[(i + 3) % 8].x + 0.5), (int) (translatedPoints[(i + 3) % 8].y + 0.5), HOTPINK);
    }
}

void process_mode_length_i(struct cube *cubes, struct cube *cubesOfInterest, int location, float camera_distance, struct Point_3D camera) {
    int i, j, k;
    int count = 0;
    for (i = 0; i < DIMENSION; i++) {
        for (j = 0; j < DIMENSION; j++) {
            for (k = 0; k < DIMENSION; k++) {
                if (i == 0 || i == DIMENSION - 1 ||
                    j == 0 || j == DIMENSION - 1 ||
                    k == 0 || k == DIMENSION - 1) {
                    if (i == location && j == 0 && k == 0) {
                        copy_cube(&cubesOfInterest[0], &cubes[count]);
                    }
                    if (i == location && j == DIMENSION - 1 && k == 0) {
                        copy_cube(&cubesOfInterest[1], &cubes[count]);
                    }
                    if (i == location && j == DIMENSION - 1 && k == DIMENSION - 1) {
                        copy_cube(&cubesOfInterest[2], &cubes[count]);
                    }
                    if (i == location && j == 0 && k == DIMENSION - 1) {
                        copy_cube(&cubesOfInterest[3], &cubes[count]);
                    }

                    count++;
                }
            }
        }
    }
    // assume mode == MODE_LENGTH_I

    float miny = FLT_MAX, minz = FLT_MAX;

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[0].points[i].y < miny) {
            miny = cubesOfInterest[0].points[i].y;
        }
        if (cubesOfInterest[0].points[i].z < minz) {
            minz = cubesOfInterest[0].points[i].z;
        }
    }

    float maxy = FLT_MIN, maxz = FLT_MIN;

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[2].points[i].y > maxy) {
            maxy = cubesOfInterest[2].points[i].y;
        }
        if (cubesOfInterest[2].points[i].z > maxz) {
            maxz = cubesOfInterest[2].points[i].z;
        }
    }

    struct Point_3D corners[NUM_CORNERS];
    int temp = 0;

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[0].points[i].y == miny && cubesOfInterest[0].points[i].z == minz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[0].points[i]);
            temp++;
        }
    }

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[1].points[i].y == maxy && cubesOfInterest[1].points[i].z == minz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[1].points[i]);
            temp++;
        }
    }

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[2].points[i].y == maxy && cubesOfInterest[2].points[i].z == maxz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[2].points[i]);
            temp++;
        }
    }

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[3].points[i].y == miny && cubesOfInterest[3].points[i].z == maxz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[3].points[i]);
            temp++;
        }
    }

    for (i = 0; i < NUM_CORNERS; i++) {
        struct Point_3D new_point = {calculateX(corners[i].x, corners[i].y, corners[i].z - camera_distance),
                                     calculateY(corners[i].x, corners[i].y, corners[i].z - camera_distance),
                                     calculateZ(corners[i].x, corners[i].y, corners[i].z - camera_distance) + camera_distance};
        corners[i] = new_point;
    }

    struct Point_2D translatedPoints[NUM_CORNERS];
    convert_3D_to_2D(translatedPoints, WIDTH, HEIGHT, corners, camera);

    draw_guide_lines(translatedPoints);
}

void process_mode_width_j(struct cube *cubes, struct cube *cubesOfInterest, int location, float camera_distance, struct Point_3D camera) {
    int i, j, k;
    int count = 0;
    for (i = 0; i < DIMENSION; i++) {
        for (j = 0; j < DIMENSION; j++) {
            for (k = 0; k < DIMENSION; k++) {
                if (i == 0 || i == DIMENSION - 1 ||
                    j == 0 || j == DIMENSION - 1 ||
                    k == 0 || k == DIMENSION - 1) {
                    if (i == 0 && j == location && k == 0) {
                        copy_cube(&cubesOfInterest[0], &cubes[count]);
                    }
                    if (i == DIMENSION - 1 && j == location && k == 0) {
                        copy_cube(&cubesOfInterest[1], &cubes[count]);
                    }
                    if (i == DIMENSION - 1 && j == location && k == DIMENSION - 1) {
                        copy_cube(&cubesOfInterest[2], &cubes[count]);
                    }
                    if (i == 0 && j == location && k == DIMENSION - 1) {
                        copy_cube(&cubesOfInterest[3], &cubes[count]);
                    }


                    count++;
                }
            }
        }
    }
    // assume mode == MODE_LENGTH_I

    float minx = FLT_MAX, minz = FLT_MAX;

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[0].points[i].x < minx) {
            minx = cubesOfInterest[0].points[i].x;
        }
        if (cubesOfInterest[0].points[i].z < minz) {
            minz = cubesOfInterest[0].points[i].z;
        }
    }

    float maxx = FLT_MIN, maxz = FLT_MIN;

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[2].points[i].x > maxx) {
            maxx = cubesOfInterest[2].points[i].x;
        }
        if (cubesOfInterest[2].points[i].z > maxz) {
            maxz = cubesOfInterest[2].points[i].z;
        }
    }

    struct Point_3D corners[NUM_CORNERS];
    int temp = 0;

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[0].points[i].x == minx && cubesOfInterest[0].points[i].z == minz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[0].points[i]);
            temp++;
        }
    }

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[1].points[i].x == maxx && cubesOfInterest[1].points[i].z == minz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[1].points[i]);
            temp++;
        }
    }

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[2].points[i].x == maxx && cubesOfInterest[2].points[i].z == maxz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[2].points[i]);
            temp++;
        }
    }

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[3].points[i].x == minx && cubesOfInterest[3].points[i].z == maxz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[3].points[i]);
            temp++;
        }
    }

    for (i = 0; i < NUM_CORNERS; i++) {
        struct Point_3D new_point = {calculateX(corners[i].x, corners[i].y, corners[i].z - camera_distance),
                                     calculateY(corners[i].x, corners[i].y, corners[i].z - camera_distance),
                                     calculateZ(corners[i].x, corners[i].y, corners[i].z - camera_distance) + camera_distance};
        corners[i] = new_point;
    }

    struct Point_2D translatedPoints[NUM_CORNERS];
    convert_3D_to_2D(translatedPoints, WIDTH, HEIGHT, corners, camera);

    draw_guide_lines(translatedPoints);
}

void process_mode_height_k(struct cube *cubes, struct cube *cubesOfInterest, int location, float camera_distance, struct Point_3D camera) {
    int i, j, k;
    int count = 0;
    for (i = 0; i < DIMENSION; i++) {
        for (j = 0; j < DIMENSION; j++) {
            for (k = 0; k < DIMENSION; k++) {
                if (i == 0 || i == DIMENSION - 1 ||
                    j == 0 || j == DIMENSION - 1 ||
                    k == 0 || k == DIMENSION - 1) {
                    if (i == 0 && j == 0 && k == location) {
                        copy_cube(&cubesOfInterest[0], &cubes[count]);
                    }
                    if (i == DIMENSION - 1 && j == 0 && k == location) {
                        copy_cube(&cubesOfInterest[1], &cubes[count]);
                    }
                    if (i == DIMENSION - 1 && j == DIMENSION - 1 && k == location) {
                        copy_cube(&cubesOfInterest[2], &cubes[count]);
                    }
                    if (i == 0 && j == DIMENSION - 1 && k == location) {
                        copy_cube(&cubesOfInterest[3], &cubes[count]);
                    }


                    count++;
                }
            }
        }
    }
    // assume mode == MODE_LENGTH_I

    float minx = FLT_MAX, miny = FLT_MAX;

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[0].points[i].x < minx) {
            minx = cubesOfInterest[0].points[i].x;
        }
        if (cubesOfInterest[0].points[i].y < miny) {
            miny = cubesOfInterest[0].points[i].y;
        }
    }

    float maxx = FLT_MIN, maxy = FLT_MIN;

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[2].points[i].x > maxx) {
            maxx = cubesOfInterest[2].points[i].x;
        }
        if (cubesOfInterest[2].points[i].y > maxy) {
            maxy = cubesOfInterest[2].points[i].y;
        }
    }

    struct Point_3D corners[NUM_CORNERS];
    int temp = 0;

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[0].points[i].x == minx && cubesOfInterest[0].points[i].y == miny) {
            copy_3D_point(&corners[temp], &cubesOfInterest[0].points[i]);
            temp++;
        }
    }

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[1].points[i].x == maxx && cubesOfInterest[1].points[i].y == miny) {
            copy_3D_point(&corners[temp], &cubesOfInterest[1].points[i]);
            temp++;
        }
    }

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[2].points[i].x == maxx && cubesOfInterest[2].points[i].y == maxy) {
            copy_3D_point(&corners[temp], &cubesOfInterest[2].points[i]);
            temp++;
        }
    }

    for (i = 0; i < NUM_CORNERS; i++) {
        if (cubesOfInterest[3].points[i].x == minx && cubesOfInterest[3].points[i].y == maxy) {
            copy_3D_point(&corners[temp], &cubesOfInterest[3].points[i]);
            temp++;
        }
    }

    for (i = 0; i < NUM_CORNERS; i++) {
        struct Point_3D new_point = {calculateX(corners[i].x, corners[i].y, corners[i].z - camera_distance),
                                     calculateY(corners[i].x, corners[i].y, corners[i].z - camera_distance),
                                     calculateZ(corners[i].x, corners[i].y, corners[i].z - camera_distance) + camera_distance};
        corners[i] = new_point;
    }

    struct Point_2D translatedPoints[NUM_CORNERS];
    convert_3D_to_2D(translatedPoints, WIDTH, HEIGHT, corners, camera);

    // draw neighbor line
    draw_guide_lines(translatedPoints);
}

void generateIndicator(struct cube *cubes, int mode, int location, float camera_distance, struct Point_3D camera) {
    if (location >= DIMENSION) {
        return;
    }

    struct cube cubesOfInterest[4];

    if (mode == MODE_LENGTH_I) {
        process_mode_length_i(cubes, cubesOfInterest, location, camera_distance, camera);
    } else if (mode == MODE_WIDTH_J) {
        process_mode_width_j(cubes, cubesOfInterest, location, camera_distance, camera);
    } else if (mode == MODE_HEIGHT_K) {
        process_mode_height_k(cubes, cubesOfInterest, location, camera_distance, camera);
    } else {
        return;
    }
}


/*
 *  Function:    swap_plane
 *  -----------------------
 *  Given the current state, it will change any initial plane with the new update value
 *
 *  Input params:
 *      uint32_t *state:    pointer to current state to modify
 *      int initial:        value of initial state
 *      int updated:        value to replace initial state
 *
 *  Return:
 *      No return value. Update value is change directly to *state.
 */
void swap_plane(uint32_t *state, int initial, int updated) {
    for (int i = 0; i < 4; i++) {
        uint32_t plane_id = ((*state << (32 - 6 * (i + 1)) >> 29));
        if (plane_id == initial) {
            uint32_t mask = (1 << 3) - 1;
            mask = mask << (i * 6 + 3);
            mask = ~mask;

            *state = *state & mask;

            *state += updated << (i * 6 + 3);
        }
    }
}

/*
 *  Function:    load_cube_in_plane
 *  -------------------------------
 *  Loads cubes in a specified plane into a separate pointer of pointers to cubes
 *
 *  Input params:
 *      struct cube *cubes:         array of cubes
 *      struct cube **select_cubes: array of pointers to cubes in specified plane
 *      int mode:                   axis (in i, j, or k direction)
 *      int location:
 *
 *  Return:
 *      No return value. Update value is change directly to *state.
 */
void load_cube_in_plane(struct cube *cubes, struct cube **select_cubes, int mode, int location) {

    int i, j, k;

    int count = 0;
    int select_count = 0;

    for (i = 0; i < DIMENSION; i++) {
        for (j = 0; j < DIMENSION; j++) {
            for (k = 0; k < DIMENSION; k++) {
                if (i == 0 || i == DIMENSION - 1 ||
                    j == 0 || j == DIMENSION - 1 ||
                    k == 0 || k == DIMENSION - 1) {

                    if (mode == MODE_LENGTH_I && i == location) {
                        select_cubes[select_count] = &cubes[count];
                        select_count++;
                    }

                    if (mode == MODE_WIDTH_J && j == location) {
                        select_cubes[select_count] = &cubes[count];
                        select_count++;
                    }

                    if (mode == MODE_HEIGHT_K && k == location) {
                        select_cubes[select_count] = &cubes[count];
                        select_count++;
                    }

                    count++;
                }
            }
        }
    }
}

int get_num_active_planes(uint32_t state) {
    int count = 0;
    for (int i = 0; i < 4; i++) {
        uint32_t plane_id = ((state << (32 - 6 * (i + 1)) >> 29));
        if (plane_id != 7) {
            count++;
        }
    }
    return count;
}


int is_active_plane(uint32_t state, int plane_to_check) {
    for (int i = 0; i < 3; i++) {
        uint32_t plane_id = ((state << (32 - 6 * (i + 1)) >> 29));
        if (plane_id == plane_to_check) {
            return 1;
        }
    }
    return 0;
}

uint32_t get_color_of_plane(uint32_t state, int plane_to_check) {
    for (int i = 0; i < 3; i++) {
        uint32_t plane_id = ((state << (32 - 6 * (i + 1)) >> 29));
        if (plane_id == plane_to_check) {
            return ((state << (32 - 6 * (i + 1) + 3)) >> 29);
        }
    }
    return 0;
}

void reverseArray(struct cube** arr, int start, int end) {
    struct cube *temp;
    while (start < end) {
        temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}

void rotate_sides(struct cube **select_cubes, Pair *plane_order, int cubes_per_plane) {
    int i, j;
    struct cube *grouped_cubes[4 * (DIMENSION - 1)];

    for (i = 0; i < 4; i++) {
        int temp = 0;
        for (j = 0; j < cubes_per_plane; j++) {
            uint32_t currState = select_cubes[j]->currState;
            int num_active_planes = get_num_active_planes(currState);
            if (num_active_planes == 1 && is_active_plane(currState, plane_order[i].first)) {
                grouped_cubes[i * (DIMENSION - 1) + temp] = select_cubes[j];
                temp++;
            }

            if (num_active_planes == 2 && is_active_plane(currState, plane_order[i].first)
                && is_active_plane(currState, plane_order[i].second)) {
                grouped_cubes[i * (DIMENSION - 1) + temp] = select_cubes[j];
                temp++;
            }
        }
    }

    for(i=0; i<4; i++) {
        if(get_num_active_planes(grouped_cubes[i * (DIMENSION-1)]->currState) > 1) {
            reverseArray(grouped_cubes, i * (DIMENSION-1), i * (DIMENSION-1) + DIMENSION-1-1);
        }
    }

    // MIDDLE PIECES
    for (j = 0; j < DIMENSION - 1 - 1; j++) {

        uint32_t temp = grouped_cubes[(0* (DIMENSION - 1) + j)]->currState;


        grouped_cubes[(0* (DIMENSION - 1) + j)]->currState = grouped_cubes[1 * (DIMENSION - 1) + j]->currState;
        swap_plane(&(grouped_cubes[0 * (DIMENSION - 1) + j]->currState), plane_order[0].second,
                   plane_order[0].first);

        grouped_cubes[(1* (DIMENSION - 1) + j)]->currState = grouped_cubes[2 * (DIMENSION - 1) + j]->currState;
        swap_plane(&(grouped_cubes[(1) * (DIMENSION - 1) + j]->currState), plane_order[1].second,
                   plane_order[1].first);

        grouped_cubes[(2* (DIMENSION - 1) + j)]->currState = grouped_cubes[3 * (DIMENSION - 1) + j]->currState;
        swap_plane(&(grouped_cubes[(2) * (DIMENSION - 1) + j]->currState), plane_order[2].second,
                   plane_order[2].first);

        grouped_cubes[3 * (DIMENSION - 1) + j]->currState = temp;
        swap_plane(&(grouped_cubes[(3) * (DIMENSION - 1) + j]->currState), plane_order[3].second,
                   plane_order[3].first);
    }

    // EDGE PIECES
    j = DIMENSION-1-1;

    uint32_t temp = grouped_cubes[(0* (DIMENSION - 1) + j)]->currState;


    grouped_cubes[(0* (DIMENSION - 1) + j)]->currState = grouped_cubes[1 * (DIMENSION - 1) + j]->currState;
    swap_plane(&(grouped_cubes[0 * (DIMENSION - 1) + j]->currState), plane_order[0].second,
               plane_order[0].first);
    swap_plane(&(grouped_cubes[0 * (DIMENSION - 1) + j]->currState), plane_order[1].second,
               plane_order[1].first);

    grouped_cubes[(1* (DIMENSION - 1) + j)]->currState = grouped_cubes[2 * (DIMENSION - 1) + j]->currState;
    swap_plane(&(grouped_cubes[(1) * (DIMENSION - 1) + j]->currState), plane_order[1].second,
               plane_order[1].first);
    swap_plane(&(grouped_cubes[(1) * (DIMENSION - 1) + j]->currState), plane_order[2].second,
               plane_order[2].first);

    grouped_cubes[(2* (DIMENSION - 1) + j)]->currState = grouped_cubes[3 * (DIMENSION - 1) + j]->currState;
    swap_plane(&(grouped_cubes[(2) * (DIMENSION - 1) + j]->currState), plane_order[2].second,
               plane_order[2].first);
    swap_plane(&(grouped_cubes[(2) * (DIMENSION - 1) + j]->currState), plane_order[3].second,
               plane_order[3].first);

    grouped_cubes[3 * (DIMENSION - 1) + j]->currState = temp;
    swap_plane(&(grouped_cubes[(3) * (DIMENSION - 1) + j]->currState), plane_order[3].second,
               plane_order[3].first);
    swap_plane(&(grouped_cubes[(3) * (DIMENSION - 1) + j]->currState), plane_order[0].second,
               plane_order[0].first);
}

void rotate_edges(struct cube **select_cubes, Pair *plane_order, int cubes_per_plane) {
    int i, j;
    struct cube *grouped_cubes[4 * (DIMENSION - 1)];
    int count = 0;
    for (i = 0; i < 4; i++) {
        int temp = 0;
        for (j = 0; j < cubes_per_plane; j++) {
            uint32_t currState = select_cubes[j]->currState;
            int num_active_planes = get_num_active_planes(currState);

            if (num_active_planes == 2 && is_active_plane(currState, plane_order[i].first)) {
                grouped_cubes[i * (DIMENSION - 1) + temp] = select_cubes[j];
                temp++;
                count++;
            }

            if (num_active_planes == 3 && is_active_plane(currState, plane_order[i].first)
                && is_active_plane(currState, plane_order[i].second)) {
                grouped_cubes[i * (DIMENSION - 1) + temp] = select_cubes[j];
                temp++;
                count++;
            }
        }
    }

    for(i=0; i<4; i++) {
        if(get_num_active_planes(grouped_cubes[i * (DIMENSION-1)]->currState) > 2) {
            reverseArray(grouped_cubes, i * (DIMENSION-1), i * (DIMENSION-1) + DIMENSION-1-1);
        }
    }

    // MIDDLE PIECES
    for (j = 0; j < DIMENSION - 1-1; j++) {

        uint32_t temp = grouped_cubes[(0 * (DIMENSION - 1) + j)]->currState;

        grouped_cubes[(0 * (DIMENSION - 1) + j)]->currState = grouped_cubes[1 * (DIMENSION - 1) + j]->currState;
        swap_plane(&(grouped_cubes[0 * (DIMENSION - 1) + j]->currState), plane_order[0].second,
                   plane_order[0].first);

        grouped_cubes[(1 * (DIMENSION - 1) + j)]->currState = grouped_cubes[2 * (DIMENSION - 1) + j]->currState;
        swap_plane(&(grouped_cubes[(1) * (DIMENSION - 1) + j]->currState), plane_order[1].second,
                   plane_order[1].first);

        grouped_cubes[(2 * (DIMENSION - 1) + j)]->currState = grouped_cubes[3 * (DIMENSION - 1) + j]->currState;
        swap_plane(&(grouped_cubes[(2) * (DIMENSION - 1) + j]->currState), plane_order[2].second,
                   plane_order[2].first);

        grouped_cubes[(3 * (DIMENSION - 1) + j)]->currState = temp;
        swap_plane(&(grouped_cubes[(3) * (DIMENSION - 1) + j]->currState), plane_order[3].second,
                   plane_order[3].first);

    }

    // EDGE PIECES
    j = DIMENSION-1-1;

    uint32_t temp = grouped_cubes[(0* (DIMENSION - 1) + j)]->currState;


    grouped_cubes[(0* (DIMENSION - 1) + j)]->currState = grouped_cubes[1 * (DIMENSION - 1) + j]->currState;
    swap_plane(&(grouped_cubes[0 * (DIMENSION - 1) + j]->currState), plane_order[0].second,
               plane_order[0].first);
    swap_plane(&(grouped_cubes[0 * (DIMENSION - 1) + j]->currState), plane_order[1].second,
               plane_order[1].first);

    grouped_cubes[(1* (DIMENSION - 1) + j)]->currState = grouped_cubes[2 * (DIMENSION - 1) + j]->currState;
    swap_plane(&(grouped_cubes[(1) * (DIMENSION - 1) + j]->currState), plane_order[1].second,
               plane_order[1].first);
    swap_plane(&(grouped_cubes[(1) * (DIMENSION - 1) + j]->currState), plane_order[2].second,
               plane_order[2].first);

    grouped_cubes[(2* (DIMENSION - 1) + j)]->currState = grouped_cubes[3 * (DIMENSION - 1) + j]->currState;
    swap_plane(&(grouped_cubes[(2) * (DIMENSION - 1) + j]->currState), plane_order[2].second,
               plane_order[2].first);
    swap_plane(&(grouped_cubes[(2) * (DIMENSION - 1) + j]->currState), plane_order[3].second,
               plane_order[3].first);

    grouped_cubes[3 * (DIMENSION - 1) + j]->currState = temp;
    swap_plane(&(grouped_cubes[(3) * (DIMENSION - 1) + j]->currState), plane_order[3].second,
               plane_order[3].first);
    swap_plane(&(grouped_cubes[(3) * (DIMENSION - 1) + j]->currState), plane_order[0].second,
               plane_order[0].first);

}

void rotate_middle(struct cube **select_cubes, int cubes_per_plane, int mode, int direction) {
    int i, j;
    int count = 0;
    int num_cubes = cubes_per_plane - 4 * (DIMENSION - 1);
    struct cube *grouped_cubes[num_cubes];

    int n = DIMENSION - 2;

    if (n < 1) {
        return;
    }

    if(mode == MODE_HEIGHT_K) {
        direction = !direction;
    }

    for (j = 0; j < cubes_per_plane; j++) {
        uint32_t currState = select_cubes[j]->currState;
        int num_active_planes = get_num_active_planes(currState);
        if (num_active_planes == 1) {
            grouped_cubes[count] = select_cubes[j];
            count++;
        }
    }
    if (direction) {
        for (i = 0; i < (n + 1) / 2; i++) {
            for (j = 0; j < n / 2; j++) {
                uint32_t temp = grouped_cubes[(n - 1 - j) * n + i]->currState;
                grouped_cubes[(n - 1 - j) * n + i]->currState = grouped_cubes[(n - 1 - i) * n + n - j - 1]->currState;
                grouped_cubes[(n - 1 - i) * n + n - j - 1]->currState = grouped_cubes[j * n + n - 1 - i]->currState;
                grouped_cubes[j * n + n - 1 - i]->currState = grouped_cubes[i * n + j]->currState;
                grouped_cubes[i * n + j]->currState = temp;
            }
        }
    }
    else {
        for (i = 0; i < (n + 1) / 2; i++) {
            for (j = 0; j < n / 2; j++) {
                uint32_t temp = grouped_cubes[i * n + j]->currState;
                grouped_cubes[i * n + j]->currState = grouped_cubes[j * n + n - 1 - i]->currState;
                grouped_cubes[j * n + n - 1 - i]->currState = grouped_cubes[(n - 1 - i) * n + n - j - 1]->currState;
                grouped_cubes[(n - 1 - i) * n + n - j - 1]->currState = grouped_cubes[(n - 1 - j) * n + i]->currState;
                grouped_cubes[(n - 1 - j) * n + i]->currState = temp;
            }
        }
    }
}

void rotate(struct cube *cubes, int mode, int location, Pair *plane_order, int direction) {
    int cubes_per_plane;

    if (location == 0) {
        cubes_per_plane = DIMENSION * DIMENSION;
        struct cube *select_cubes[cubes_per_plane];
        load_cube_in_plane(cubes, select_cubes, mode, location);
        rotate_edges(select_cubes, plane_order, cubes_per_plane);
        rotate_middle(select_cubes, cubes_per_plane, mode, direction);
        return;
    }

    else if (location == DIMENSION - 1) {
        cubes_per_plane = DIMENSION * DIMENSION;
        struct cube *select_cubes[cubes_per_plane];
        load_cube_in_plane(cubes, select_cubes, mode, location);
        rotate_edges(select_cubes, plane_order, cubes_per_plane);
        rotate_middle(select_cubes, cubes_per_plane, mode, direction);
        return;
    }
    else {
        cubes_per_plane = DIMENSION * 4 - 4;
        struct cube *select_cubes[cubes_per_plane];
        load_cube_in_plane(cubes, select_cubes, mode, location);
        rotate_sides(select_cubes, plane_order, cubes_per_plane);
    }
}


/*
 *  Function:    resetFaces
 *  -----------------------
 *  Sets all cubes to default, solved state
 *
 *  Input params:
 *      struct cube *cubes:     pointer to every cube that makes the
 *      int num_cubes:
 *
 *  Return:
 *      uint32_t *pixels: a 2D array containing the pixels to be drawn
 *          onto the screen.
 */
void resetFaces(struct cube *cubes, int num_cubes) {
    int i, j, k;
    for (i = 0; i < num_cubes; i++) {
        cubes[i].currState = 0xFFFFFFFF;
    }

    int planeOfInterest;
    int count = 0;
    for (i = 0; i < DIMENSION; i++) {
        for (j = 0; j < DIMENSION; j++) {
            for (k = 0; k < DIMENSION; k++) {
                if (i == 0 || i == DIMENSION - 1 ||
                    j == 0 || j == DIMENSION - 1 ||
                    k == 0 || k == DIMENSION - 1) {
                    int planeCount = 0;
                    if (i == 0) {
                        planeOfInterest = 4;

                        load_default(&cubes[count].currState, planeOfInterest, planeCount);
                        planeCount++;
                    }

                    if (i == DIMENSION - 1) {
                        planeOfInterest = 2;

                        load_default(&cubes[count].currState, planeOfInterest, planeCount);
                        planeCount++;
                    }


                    if (j == 0) {
                        planeOfInterest = 5;

                        load_default(&cubes[count].currState, planeOfInterest, planeCount);
                        planeCount++;
                    }
                    if (j == DIMENSION - 1) {
                        planeOfInterest = 1;

                        load_default(&cubes[count].currState, planeOfInterest, planeCount);
                        planeCount++;
                    }

                    if (k == 0) {
                        planeOfInterest = 3;

                        load_default(&cubes[count].currState, planeOfInterest, planeCount);
                        planeCount++;
                    }

                    if (k == DIMENSION - 1) {
                        planeOfInterest = 0;
                        load_default(&cubes[count].currState, planeOfInterest, planeCount);
                    }
                    count++;
                }
            }
        }
    }
}


void draw_timer(int hms, int width, int height, int tx, int ty, int font_size, int spacing, uint32_t color) {
    char time[9];
    time[8] = '\0';

    int h1 = (hms / 100000) % 10;
    int h2 = (hms / 10000) % 10;
    int m1 = (hms / 1000) % 10;
    int m2 = (hms / 100) % 10;
    int s1 = (hms / 10) % 10;
    int s2 = hms % 10;

    // Format the time string as hh:mm:ss
    time[0] = '0' + h1;
    time[1] = '0' + h2;
    time[2] = ':';
    time[3] = '0' + m1;
    time[4] = '0' + m2;
    time[5] = ':';
    time[6] = '0' + s1;
    time[7] = '0' + s2;

    draw_text(pixels, width, height, time, tx, ty, font_size, DIGITAL_FONT_WIDTH, DIGITAL_FONT_HEIGHT, spacing, color);
}

int checkSolved(struct cube *cubes, int num_cubes) {
    int i, j;
    for(i = 0; i < NUM_PLANES; i++) {
        uint32_t curr_color = BG_COLOR;
        uint32_t temp;
        for(j = 0; j < num_cubes; j++) {
            if(is_active_plane(cubes[j].currState, i)) {
                temp = get_color_of_plane(cubes[j].currState, i);
                if(curr_color == BG_COLOR) {
                    curr_color = temp;
                }
                if(curr_color != temp) {
                    return 0;
                }
            }
        }
    }
    return 1;
}
/*
 *  Function:    render
 *  -------------------
 *  calls the methods to draw/compute the cube and store it in an
 *      uint32_t array describing the pixels on the screen.
 *
 *  Input params (called by js file):
 *      int dt:     the current 'tick', computed on the js file
 *      float a:    controls angle of an axis
 *      float b:    controls angle of another axis
 *      float c:    controls angle of yet another axis
 *
 *  Return:
 *      uint32_t *pixels: a 2D array containing the pixels to be drawn
 *          onto the screen.
 */
uint32_t *render(int dt, float a, float b, float c, int dimension, int mode, int location, int toRotate, int direction, int angle, int add_sec, int reset_time) {
    int refresh = 0;
    if (dimension != DIMENSION) {
        refresh = 1;
    }
    DIMENSION = dimension;
    int num_cubes = (DIMENSION == 1) ? 1 : DIMENSION * DIMENSION * DIMENSION - (DIMENSION - 2) * (DIMENSION - 2) * (DIMENSION - 2);
    struct cube cubes[num_cubes];
    struct cube translatedCubes[num_cubes];

    if(mode == MODE_WIDTH_J) {
        angle = -angle;
    }
    if(mode == MODE_HEIGHT_K) {
        angle = -angle;
//        direction = !direction;
    }
    if (refresh) {
        resetFaces(cubes, num_cubes);
    }
    if (toRotate && direction) {
        if (mode == MODE_LENGTH_I) {
            Pair plane_order[4] = {
                    {5, 3},
                    {3, 1},
                    {1, 0},
                    {0, 5}};
            rotate(cubes, mode, location, plane_order, direction);
        } if (mode == MODE_WIDTH_J) {
            Pair plane_order[4] = {
                    {4, 3},
                    {3, 2},
                    {2, 0},
                    {0, 4}};
            rotate(cubes, mode, location, plane_order, direction);
        } if (mode == MODE_HEIGHT_K) {
            Pair plane_order[4] = {
                    {5, 4},
                    {4, 1},
                    {1, 2},
                    {2, 5}};
            rotate(cubes, mode, location, plane_order, direction);
        }
    }

    if (toRotate && !direction) {
        if (mode == MODE_LENGTH_I) {
            Pair plane_order[4] = {
                    {5, 0},
                    {0, 1},
                    {1, 3},
                    {3, 5}
                    };
            rotate(cubes, mode, location, plane_order, direction);
        } if (mode == MODE_WIDTH_J) {
            Pair plane_order[4] = {
                    {4, 0},
                    {0, 2},
                    {2, 3},
                    {3, 4}
                    };
            rotate(cubes, mode, location, plane_order, direction);
        } if (mode == MODE_HEIGHT_K) {
            Pair plane_order[4] = {
                    {5, 2},
                    {2, 1},
                    {1, 4},
                    {4, 5}
            };
            rotate(cubes, mode, location, plane_order, direction);
        }
    }

    fill_screen(pixels, WIDTH, HEIGHT, BG_COLOR);

    A = a;
    B = b;
    C = c;

    f_modulo(&A, 2 * (float) PI);
    f_modulo(&B, 2 * (float) PI);
    f_modulo(&C, 2 * (float) PI);

    float magnitude = SCALE - GAP;
    float camera_distance = 200;

    struct Point_3D camera = {0, 0, 0};

    uint32_t colors[NUM_PLANES] = {BLUE, YELLOW, RED, GREEN, ORANGE, WHITE};

    generateCubes(cubes, translatedCubes, num_cubes, magnitude, camera_distance, mode, location, angle);

    for (int i = 0; i < num_cubes; i++) {
        draw_cube(pixels, WIDTH, HEIGHT, translatedCubes[i].points, camera, colors, translatedCubes[i].currState);
    }

    if(angle == 0) {
        generateIndicator(cubes, mode, location, camera_distance, camera);
    }

    if(reset_time) {
        hms = 0;
    }

    uint32_t hms_color = RED;
    if(checkSolved(cubes, num_cubes)) {
        hms = hms;
        hms_color = GREEN;
    }
    else if(add_sec) {
        hms += 1;
        if(hms % 100 % 60 == 0) {
            hms += 100;
            hms -= 60;
        }
        if(hms % 10000 % 6000 == 0) {
            hms += 10000;
            hms -= 6000;
        }
    }



    int font_size = 5;
    int spacing = 1;
    int margin = 10;
    int tx = WIDTH-8*(DIGITAL_FONT_WIDTH+spacing)*font_size + spacing*font_size - margin;
    draw_timer(hms, WIDTH, HEIGHT, tx, margin, font_size, spacing, hms_color);


    return pixels;
}


void *memcpy(void *dest, const void *src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        ((char *) dest)[i] = ((char *) src)[i];
    }
    return NULL;
}

// for debugging
int main(void) {
//    render(1, 0, 0, 0, 4, MODE_LENGTH_I, 0, 1, 0);
//    render(1, 0, 0, 0, 4, MODE_LENGTH_I, 0, 1, 0);
//    render(1, 0, 0, 0, 4, MODE_LENGTH_I, 0, 1);

    int num_cubes = (DIMENSION == 1) ? 1 : DIMENSION * DIMENSION * DIMENSION - (DIMENSION - 2) * (DIMENSION - 2) * (DIMENSION - 2);
    struct cube cubes[num_cubes];
    struct cube translatedCubes[num_cubes];


    float magnitude = SCALE - GAP;
    float camera_distance = 200;

    struct Point_3D camera = {0, 0, 0};

    uint32_t colors[NUM_PLANES] = {BLUE, YELLOW, RED, GREEN, ORANGE, WHITE};


    generateCubes(cubes, translatedCubes, num_cubes, magnitude, camera_distance, 0, 0, 0);


    int o = checkSolved(cubes, num_cubes);

    return 0;
}
