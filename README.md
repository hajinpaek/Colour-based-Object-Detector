# Colour-based-Object-Detector

The code uses stdbool.h for handling thresholded as a boolean array to identify 
object pixels and string.h primarily for resetting labelsArray in detection mode.

show_calibration: Displays the contents of a calibration file by opening the 
file, checking it, and then reading and outputting each objects parameters 
in the specified format.

floodfill: Labels connected regions in the thresholded image, recursively 
marking adjacent pixels (left, right, up, down) to build an area, while skipping 
already labeled or non-object pixels.

detect_objects: Detects objects in the image based on HSV profiles from the 
calibration file. It allocates memory for thresholded, which marks each pixel 
as part of an object or background based on HSV values, and labelsArray, which 
labels connected areas. It repeats thresholding for each calibration log, 
and finds connected regions with floodfill, and calculates bounding boxes 
based on min and max x/y coordinates.

calibrate_object: Generates a color profile for an object by analyzing a 50x50 
central pixel window. It loads the image, filters pixels based on  the values 
saturation ≥ 50, value ≥ 30, and tracks min/max hue values. It then calculates 
the midpoint and max hue difference using hue_midpoint and hue_difference, and
outputs the profile.

main: The programs entry point, parsing command-line arguments to determine the 
mode. Based on the mode, it calls the appropriate function to display calibration
data, detect objects, or generate a calibration entry, ensuring the correct
number of arguments are provided and displaying an error message if it isnt.
