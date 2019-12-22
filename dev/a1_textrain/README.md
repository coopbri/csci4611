### CSCI 4611 Assignment 1: Text Rain
##### Brian Cooper
<hr>

This assignment involves a reimagination of the _Text Rain_ exhibit by Camille Utterback and Romy Achituv in the Processing language. Various design decisions and features of the program are described below.

## Program Design

#### Video Display
Upon running the program, a menu is provided with options to choose a test movie file (by pressing `0` on the keyboard) or one of nine slots for possible connected webcams (by pressing `1-9` on the keyboard). If a webcam input is used, the video is flipped horizontally and a grayscale filter is applied as an overlay for a smoother user experience.

#### Letter Class
An inline class is packaged in the source code called `Letter`. This class controls, as the name suggests, attributes native to the letters raining down on the screen.

#### Random Character Selection
For the text component of the visualization, I sample text from Robert Frost's poem, _Lodged_. This poem is stored as a `String` object. All of the punctuation is removed, so only the letters are left for convenient parsing. In `setup()`, I initialize an array of Letter objects and fill the array by selecting a character at a random index of the poem text string using Processing's pseudorandom built-in function `random()`, and adding this returned letter to a slot in the array.

## Extra Features

#### Dynamic Thresholding
The threshold value (default `128`) controls the difference between a "light" spot and a "dark" spot for the text to react to. If a pixel on the video frame that a letter reaches is thresholded to white (a light spot), the text will fall. Conversely, if a pixel is thresholded to black (a dark spot), the text will float up past (or sit on top of) a dark region until it is at the "top" of the dark region. This value can be changed by pressing `↑` (up arrow) and `↓` (down arrow) to increase or decrease the threshold value, respectively. The value is constrained between `0` and `255`. The current threshold value is logged to the console.

#### Disco Mode
Disco mode randomizes the text color every 30 frames. Activate or deactivate it by pressing the `d` key.

#### Debug Mode
Debug mode allows you to see the threshold computation in action: black/white thresholding is displayed for each pixel frame-by-frame. Activate or deactivate it by pressing `spacebar`.
