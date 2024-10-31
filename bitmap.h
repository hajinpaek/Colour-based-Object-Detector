#ifndef _BITMAP_H
#define _BITMAP_H

// NOTE: you do not need to edit this file

#define RED 0
#define GREEN 1
#define BLUE 2

// Here we define our own type "Bmp"
// it is a struct containing all the data about an image
typedef struct {

    // The height of the image in pixels
    unsigned int height; 

    // The width of the image in pixels
    unsigned int width; 

    // 2D array of pointers to pixels
    // pixels are a 3 byte arrays of [RED, GREEN, BLUE]
    // each is a colour (from 0-255) is that component of colour in the pixel
    unsigned char ***pixels;

    // Don't worry about this, we just use it to store some extra information about the image
    void *header;
} Bmp;

// structure used to store Hue-Saturation-Value (HSV)
// of a single pixel colour: used by "rgb2hsv()"
typedef struct {

    // Hue (raw colour): measured from 0 to 360
    int hue;

    // Saturation (vibrancy of colour): between 0 and 100
    int saturation;

    // Value (lightness): between 0 and 100
    int value;

} HSV;

// Open an image
Bmp read_bmp(char *filename); 

// Write an image to a file
void write_bmp(Bmp, char *filename);

// Copy an image
Bmp copy_bmp(Bmp bmp);

// Free an image
// Make sure this is called once for every Bmp you create
void free_bmp(Bmp);

// convert an array of RGB colour (from a pixel) to 
// Hue-Saturation-Value (HSV)
HSV rgb2hsv(unsigned char *rgb);

// Calculates the difference in two hue values, each between
// 0-360, accounting for angle wrap-around
int hue_difference(int hue1, int hue2);

// Calculates the midpoint in two hue values, each between
// 0-360, accounting for angle wrap-around
int hue_midpoint(int hue1, int hue2);

// draws a white box on top of the image with (x,y) specifying 
// the bottom left-hand corner and (width,height) is the size of the box
void draw_box(Bmp image, int x, int y, int width, int height);

#endif

