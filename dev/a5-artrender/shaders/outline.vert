#version 330

// CSci-4611 Assignment 5:  Art Render

uniform mat4 model_view_matrix;
uniform mat4 normal_matrix;
uniform mat4 proj_matrix;
uniform float thickness;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 left_normal;
layout(location = 3) in vec3 right_normal;

void main() {
  // copy vertex for silhouette displacement
  vec3 v = vertex;

  // vector from vertex to eye
  vec3 e = -(model_view_matrix * vec4(vertex, 1)).xyz;

  // left and right normal vectors
  vec3 nl = (normal_matrix * vec4(left_normal, 0)).xyz;
  vec3 nr = (normal_matrix * vec4(right_normal, 0)).xyz;

  // determine if on silhouette edge
  if (dot(e, nl) * dot(e, nr) < 0.0) {
    // on edge; add outline
    v += thickness * normal;
  }

  gl_Position = proj_matrix * model_view_matrix * vec4(v, 1);
}
