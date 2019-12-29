<center>
<h3>CSCI 4611 Assignment 5: GLSL Shader Demonstrations</h3>
<h4>Brian Cooper</h4>
<hr>

![a5 preview](a5.png =500x400)

</center>

## Program Overview
This program implements various shading models and techniques using GLSL shaders. The models are <a href="https://en.wikipedia.org/wiki/Gouraud_shading">Gourard</a>, <a href="https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model">Blinn-Phong</a>, and a cartoon model based on specular and diffuse ramps (textures).

## Compiling and Running
To compile and run the program, execute
```sh
make && ./a5-artrender
```
from within a directory called `build` within the main project directory. Note that this requires Cmake initialization before any of these steps are possible.

## Usage
Upon running the program, an interface is presented with three buttons on top to choose the rendering style (lighting/shader method) and seven buttons with various models for testing the rendering styles. Within the viewport, the object can be translated with horizontal left-mouse-button movement, scaled with vertical left-mouse-button movement, and rotated by clicking a point on the model and then dragging after a point is marked and deselected.

## Specular Lighting Design
For the specular component in the Blinn-Phong shader, I used the "halfway vector" method in contrast to the reflection vector method.
