#version 330

// CSci-4611 Assignment 5:  Art Render

in vec3 position_in_eye_space;
in vec3 normal_in_eye_space;

out vec4 color;

uniform vec3 light_in_eye_space;

// component light intensities
uniform vec4 Ia, Id, Is;

// component light coefficients
uniform vec4 ka, kd, ks;

// shininess for specular lighting
uniform float s;

uniform sampler2D diffuse_ramp;
uniform sampler2D specular_ramp;

void main() {
  // unit vector from the vertex to the light
  vec3 l = normalize(light_in_eye_space - position_in_eye_space);

  // unit vector from the vertex to the eye point, which is at 0,0,0 in "eye space"
  vec3 e = normalize(-position_in_eye_space);

  // normal vector
  vec3 n = normalize(normal_in_eye_space);

  // halfway vector
  vec3 h = normalize(e + l);

  // calculate light contribution per component
  vec4 ambient = ka * Ia;
  vec4 diffuse = kd * Id * texture(diffuse_ramp, vec2(max(0.5 * dot(n, l) + 0.5, 0), 0));
  vec4 specular = ks * Is * texture(specular_ramp, vec2(pow(max(dot(h, n), 0), s), 0));

  // calculate complete lighting based on component intensities
  color = ambient + diffuse + specular;
}
