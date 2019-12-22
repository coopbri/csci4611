#version 330


// Gouraud Shader Example

// INPUTS:

// uniform = variables passed in from the C++ code
// model and camera matrices:
uniform mat4 model_view_matrix;
uniform mat4 normal_matrix;
uniform mat4 proj_matrix;

// properties of the light:
uniform vec3 light_in_eye_space;
uniform vec4 Ia, Id, Is;

// properties of the material we are lighting:
uniform vec4 ka, kd, ks;
uniform float s;

// these variables come from the mesh data stored in buffers in gfx card memory
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

// OUTPUT:

// for Gouraud shading, the key output of the vertex shader is the color
// calculated based on lighting this vertex
out vec4 color;

void main() {

    // vertex position in "eye space"
    vec3 v = (model_view_matrix * vec4(vertex, 1)).xyz;

    // unit vector from the vertex to the light
    vec3 l = normalize(light_in_eye_space - v);

    // unit vector from the vertex to the eye point, which is at 0,0,0 in "eye space"
    vec3 e = normalize(-v);

    // normal transformed into "eye space"
    vec3 n = (normal_matrix * vec4(normal, 0)).xyz;

    // halfway vector
    vec3 h = normalize(l + e);

    // calculate color using the light intensity equation
    vec4 ambient = ka * Ia;
    vec4 diffuse = kd * Id * max(dot(n, l), 0);
    vec4 specular = ks * Is * pow(max(dot(n, h), 0), s);
    color = ambient + diffuse + specular;

    // do the standard projection of the incoming vertex
    gl_Position = proj_matrix * vec4(v,1);
}
