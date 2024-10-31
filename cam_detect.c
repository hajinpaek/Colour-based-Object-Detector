#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "bitmap.h"

//mode s 
void show_calibration(char *filepath) {
    //open file and error message when can't be opened
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Cannot open calibration file\n");
        return;
    }

    printf("Calibrated Objects:\n");

    //reading calibration file
    char object_name[50];
    int hue, max_diff, min_s, min_v;
    //use format specifier to print details for objects
    while (fscanf(file, "%s %d %d %d %d", object_name, &hue, &max_diff, &min_s, &min_v) == 5) {
        printf("%s: Hue: %d (Max. Diff: %d), Min. SV: %d %d\n", 
               object_name, hue, max_diff, min_s, min_v);
    }

    fclose(file);
}


void floodfill(int x, int y, bool **thresholded, int **labelsArray, int label, int width, int height) {
     // base case
    // Checking if x and y is too small
    if (x < 0 || y < 0 || x >= width || y >= height) return;
    if (!thresholded[y][x] || labelsArray[y][x] != 0) return;
    
    // labeling the current pixel
    labelsArray[y][x] = label;

    // recursive case
    floodfill(x - 1, y, thresholded, labelsArray, label, width, height);
    floodfill(x + 1, y, thresholded, labelsArray, label, width, height);
    floodfill(x, y - 1, thresholded, labelsArray, label, width, height);
    floodfill(x, y + 1, thresholded, labelsArray, label, width, height);
}

//mode d
void detect_objects(char *calibration_file, char *image_file) {
    //open image file
    Bmp image = read_bmp(image_file);
    if (image.pixels == NULL) {
        printf("Cannot open image file\n");
        return;
    }
    //open file and error message when can't be opened

    FILE *cal_file = fopen(calibration_file, "r");
    if (cal_file == NULL) {
        printf("Cannot open calibration file\n");
        free_bmp(image);
        return;
    }
    // allocate memory for thresholded map
    bool **thresholded = malloc(image.height * sizeof(bool *));
    //labeling the different areas
    int **labelsArray = malloc(image.height * sizeof(int *));
    for (int i = 0; i < image.height; i++) {
        thresholded[i] = calloc(image.width, sizeof(bool));
        labelsArray[i] = calloc(image.width, sizeof(int));
    }

    //get object type 
    char object_name[50];
    int middle_hue, max_hue_diff, min_saturation, min_value;
    while (fscanf(cal_file, "%s %d %d %d %d", object_name, &middle_hue, &max_hue_diff, &min_saturation, &min_value) == 5) {
        
        //thresolding using the conditons from the calibration
        for (int y = 0; y < image.height; y++) {
            for (int x = 0; x < image.width; x++) {
                unsigned char *pixel = image.pixels[y][x];
                HSV hsv = rgb2hsv(pixel);
                
                //color profile check
                int hue_diff = hue_difference(hsv.hue, middle_hue);
                if (hue_diff <= max_hue_diff && hsv.saturation >= min_saturation && hsv.value >= min_value) {
                    thresholded[y][x] = true;
                } else {
                    thresholded[y][x] = false;
                }
            }
        }

        //using flood fill to label connected areas
        int label = 1; 
        for (int y = 0; y < image.height; y++) {
            for (int x = 0; x < image.width; x++) {
                if (thresholded[y][x] && labelsArray[y][x] == 0) {
                    floodfill(x, y, thresholded, labelsArray, label, image.width, image.height);
                    label++;
                }
            }
        }

        //bounding boxes for every connected area
        for (int l = 1; l < label; l++) {
            int min_x = image.width, min_y = image.height, max_x = 0, max_y = 0, size = 0;

            for (int y = 0; y < image.height; y++) {
                for (int x = 0; x < image.width; x++) {
                    if (labelsArray[y][x] == l) {
                        size++;
                        if (x < min_x) min_x = x;
                        if (y < min_y) min_y = y;
                        if (x > max_x) max_x = x;
                        if (y > max_y) max_y = y;
                    }
                }
            }

            //bounding box coordinates for every area
            int width = max_x - min_x + 1;
            int height = max_y - min_y + 1;
            if (width >= 20 && height >= 20) {
                printf("Detected %s: %d %d %d %d\n", object_name, min_x, min_y, width, height);
            }
        }

        // reset the array so that the detection process can be repeated
        for (int i = 0; i < image.height; i++) {
            memset(labelsArray[i], 0, image.width * sizeof(int));
        }
    }

    fclose(cal_file);
    for (int i = 0; i < image.height; i++) {
        free(thresholded[i]);
        free(labelsArray[i]);
    }
    free(thresholded);
    free(labelsArray);
    free_bmp(image);
}

//mode c
void calibrate_object(char *label, char *image_file) {
    Bmp image = read_bmp(image_file);
    if (image.pixels == NULL) {
        printf("Cannot open image file\n");
        return;
    }

    //setting the pixel window in center
    int start_x = (image.width / 2) - 25;
    int start_y = (image.height / 2) - 25;
    int end_x = start_x + 50;
    int end_y = start_y + 50;

    // set min and max hue values for calibration
    int min_hue = 360, max_hue = 0;  


    //looping through pixels in window
    for (int y = start_y; y < end_y; y++) {
        for (int x = start_x; x < end_x; x++) {
            unsigned char *pixel = image.pixels[y][x];
            HSV hsv = rgb2hsv(pixel);

            // filter pixels for saturation value and hsv value
            if (hsv.saturation >= 50 && hsv.value >= 30) {

                //track min/max hue
                if (hsv.hue < min_hue) min_hue = hsv.hue;
                if (hsv.hue > max_hue) max_hue = hsv.hue;
            }
        }
    }


    int middle_hue = hue_midpoint(min_hue, max_hue);
    int max_hue_diff = hue_difference(min_hue, max_hue) / 2;

    int min_saturation = 50;
    int min_value = 30;

    printf("%s %d %d %d %d\n", label, middle_hue, max_hue_diff, min_saturation, min_value);

    free_bmp(image);
}

//for controlling the modes and able to use command line arguements
int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        printf("Incorrect input\n");
        return 1;
    }

    char mode = argv[1][0];
    char *calibration_file = argv[2];
    char *image_file = (argc == 4) ? argv[3] : NULL; 

    //running the mode being inputted by user
    switch (mode) {
        case 's':
            if (argc != 3) {
                printf("Incorrect input\n");
                return 1;
            }
            show_calibration(calibration_file);
            break;

        case 'd':
            if (argc != 4) {
                printf("Incorrect input\n");
                return 1;
            }
            detect_objects(calibration_file, image_file);
            break;

        case 'c':
            if (argc != 4) {
                printf("Incorrect input\n");
                return 1;
            }
            calibrate_object(calibration_file, image_file);
            break;

        default:
            printf("Incorrect input\n");
            return 1;
    }

    return 0;
}

