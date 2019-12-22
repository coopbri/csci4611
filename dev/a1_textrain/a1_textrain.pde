/**
 CSci-4611 Assignment #1 Text Rain
 Original code by Professor Dan Keefe & CSCI 4611 staff, University of Minnesota
 Modified code by Brian Cooper
 **/

// import video library
import processing.video.*;

// Global variables for handling video data and the input selection screen
String[] cameras;
Capture cam;
Movie mov;
PImage inputImage;
PImage flippedImage;
boolean inputMethodSelected = false;

// font for letter rendering
PFont font;

// text from Robert Frost's "Lodged" with punctuation removed
String text = "The rain to the wind said You push and Ill pelt They so smote "
            + "the garden bed That the flowers actually knelt And lay lodged "
            + "though not dead I know how the flowers felt";

// threshold value for measure of dark/light
int threshold = 128;

// array of letters
Letter[] letters;

// disco mode
boolean disco = false;

// debug mode
boolean debug = false;

// -------------------------------------------------------------------------- //
// Letter class                                                               //
// -------------------------------------------------------------------------- //
class Letter {
  // coordinates (x, y)
  int x;
  int y;

  // falling speed (change in y)
  float dy;
  float dyTemp;

  // color of pixel being checked
  color c;

  // color of fourth pixel above checked pixel (1280 pixel offset; 4 rows up)
  color above;

  // character to display
  char letter;

  // constructor
  Letter(char l) {
    x = int(random(width));
    y = -10;
    // add scalar based on real time
    dy = int(random(1, 10)) + scale();
    dyTemp = dy;
    letter = l;
    c = 255;
  }

  // allow letter to fall
  void descend() {
    // bottom of viewport reached
    if (y >= height) {
      // new, random x-coordinate
      x = int(random(width));

      // move to top
      y = -10;

      // change speed of descension with scalar based on real time
      dy = int(random(1, 10)) + scale();
      dyTemp = dy;
    }

    // descend the letter
    y += dy;
  }

  // render letter to screen
  void display() {
    text(letter, x, y);
  }

  // check below to see if object is dark
  void scan() {
    // load scene frame pixels
    flippedImage.loadPixels();

    // determine index of single pixel
    int index1D = int(x + y * flippedImage.width);

    // calculate threshold value
    if (index1D > 0 && index1D <= 921600) {
      c = threshold(flippedImage.pixels[index1D-1]);

      if (index1D > 5121) {
        above = threshold(flippedImage.pixels[index1D-5121]);
      }
    }

    if (c == 0 && index1D > 0) {
      // check if above pixel is dark
      if (above == 0) {
        // raise text above dark region
        dy = -4;
      } else {
        // stop vertical movement
        dy = 0;
      }

    } else {
      // restore vertical movement downwards
      dy = dyTemp;
    }

    inputImage.updatePixels();
  }

  // augment rain velocity based on time of day (faster at night)
  int scale() {
    // get time of day in seconds
    int time = (hour() * 60 * 60) + (minute() * 60) + second();

    // scale down the time scalar
    return int(time * 0.00002);
  }
} // end of Letter class

// -------------------------------------------------------------------------- //
// threshold() function:                                                      //
//    determine threshold value based on green channel of pixel and           //
//    "collapse" the pixel into black or white                                //
// -------------------------------------------------------------------------- //
color threshold(color pixel) {
  // green channel (0 - 255) of pixel
  float grayscale = green(pixel);

  if (grayscale > threshold) {
    // above threshold : white
    pixel = 255;
  } else {
    // below threshold : black
    pixel = 0;
  }

  return pixel;
}

// -------------------------------------------------------------------------- //
// flip() function:                                                           //
//    reflect image across the y-axis (horizontal reflection)                 //
// -------------------------------------------------------------------------- //
PImage flip(PImage input) {
  // make a copy of input image
  PImage flipped = input.copy();

  // loop through pixels and reflect
  for (int i = 0; i < input.width; i++) {
    for (int j = 0; j < input.height; j++) {
      int temp = ((input.width - i) - 1) + input.width * j;
      int idx = i + input.width * j;
      flipped.pixels[idx] = input.pixels[temp];
    }
  }

  return flipped;
}

// -------------------------------------------------------------------------- //
// setup() function:                                                          //
//    traditional Processing setup() function; called once                    //
// -------------------------------------------------------------------------- //
void setup() {
  // specify viewport
  size(1280, 720);

  // generate image for viewport
  inputImage = createImage(width, height, RGB);

  // load font into scene
  font = loadFont("SansSerif.bold-30.vlw");

  // fill text with blue color
  fill(0, 0, 128);

  // set text to loaded font
  textFont(font);

  // allocate and fill letters array
  letters = new Letter[100];
  for (int i = 0; i < letters.length; i++) {
    letters[i] = new Letter(text.charAt(int(random(text.length()))));
  }
} // end of setup function

// -------------------------------------------------------------------------- //
// draw() function:                                                           //
//    traditional Processing draw() function; loop each frame                 //
// -------------------------------------------------------------------------- //
void draw() {
  // When the program first starts, draw a menu of different options for which
  //    camera to use for input
  // The input method is selected by pressing a key 0-9 on the keyboard
  if (!inputMethodSelected) {
    cameras = Capture.list();
    int y=40;
    text("O: Offline mode, test with TextRainInput.mov movie file instead of "
          + "live camera feed.", 20, y);
    y += 40;
    for (int i = 0; i < min(9, cameras.length); i++) {
      text(i+1 + ": " + cameras[i], 20, y);
      y += 40;
    }
    return;
  }

  // load an image, either from a movie file or from a live camera feed; store
  //    image in inputImage variable
  if ((cam != null) && (cam.available())) {
    cam.read();
    inputImage.copy(cam, 0, 0, cam.width, cam.height, 0, 0, inputImage.width,
      inputImage.height);
  } else if ((mov != null) && (mov.available())) {
    mov.read();
    inputImage.copy(mov, 0, 0, mov.width, mov.height, 0, 0, inputImage.width,
      inputImage.height);
  }

  // Draw the current input image to the screen (flipped and filtered)
  flippedImage = flip(inputImage);
  flippedImage.filter(GRAY);
  set(0, 0, flippedImage);

  // check if disco mode activated
  if (disco) {
    // random text color every 30 frames
    if (frameCount % 30 == 1) {
      fill(random(255), random(255), random(255));
    }
  } else {
    // fill text with blue color
    fill(0, 0, 128);
  }

  // check if debug mode activated
  if (debug) {
    // render input image as binary black/white
    for (int i = 0; i < flippedImage.pixels.length; i++) {
      flippedImage.pixels[i] = color(threshold(flippedImage.pixels[i]));
    }
    // reset image with threshold value view
    set(0, 0, flippedImage);
  }

  for (int i = 0; i < letters.length; i++) {
    // allow letters to fall
    letters[i].descend();

    // render letters to screen
    letters[i].display();

    // scan for pixel brightness
    letters[i].scan();
  }
} // end of draw function

// -------------------------------------------------------------------------- //
// keyPressed() function:                                                     //
//    register keyboard user input                                            //
// -------------------------------------------------------------------------- //
void keyPressed() {
  if (!inputMethodSelected) {
    // If we haven't yet selected the input method, then check for 0 to 9
    //    keypresses to select from the input menu
    if ((key >= '0') && (key <= '9')) {
      int input = key - '0';
      if (input == 0) {
        println("Offline mode selected.");
        mov = new Movie(this, "TextRainInput.mov");
        mov.loop();
        inputMethodSelected = true;
      } else if ((input >= 1) && (input <= 9)) {
        println("Camera " + input + " selected.");
        // The camera can be initialized directly using an element from the
        //    array returned by list():
        cam = new Capture(this, cameras[input-1]);
        cam.start();
        inputMethodSelected = true;
      }
    }
    return;
  }

  // This part of the keyPressed routine gets called after the input selection
  //    screen during normal execution of the program
  if (key == CODED) {
    // up arrow key pressed
    if (keyCode == UP) {
      // send to 255 if exceeds 255 (equals sign necessary for input delay)
      if (threshold >= 255) {
        threshold = 255;
      } else {
        // increase threshold
        threshold++;
      }

      println("Threshold: " + threshold);
    // down arrow key pressed
    } else if (keyCode == DOWN) {
      // send to 0 if exceeds 0 (equals sign necessary for input delay)
      if (threshold <= 0) {
        threshold = 0;
      } else {
        // decrease threshold
        threshold--;
      }

      println("Threshold: " + threshold);

    }
  // space bar pressed
  } else if (key == ' ') {
      // toggle debug mode
      debug = !debug;

      if (debug) {
        println("Debug mode activated.");
      } else {
        println ("Debug mode deactivated.");
      }
  } else if (key == 'd') {
    // toggle disco mode
    disco = !disco;

    if (disco) {
      println("Disco mode activated.");
    } else {
      println ("Disco mode deactivated.");
    }
  }
} // end of keyPressed function
