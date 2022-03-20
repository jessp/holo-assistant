#version 140

precision mediump float;
 
attribute vec3 pos;
attribute vec3 normal;
 
varying vec3 col;
 
uniform mat4 projectionMatrix, viewMatrix, modelMatrix;
uniform mat3 normalMatrix;
 
uniform vec3 ambientLightColour, directionalLight, materialSpecular;
uniform float materialAmbient, materialDiffuse, shininess;
 
/* A function to determine the colour of a vertex, accounting
   for ambient and directional light */
vec3 ads( vec4 position, vec3 norm )
{
  vec3 s = normalize(vec3(vec4(directionalLight,1.0) - position));
  vec3 v = normalize(vec3(-position));
  vec3 r = reflect(-s, norm);
  return ambientLightColour +
    materialDiffuse * max(dot(s,norm), 0.0) +
    materialSpecular * pow(max(dot(r,v), 0.0), shininess);
}
 
void main() {
  vec3 eyeNormal = normalize(normalMatrix * normal);
  vec4 eyePosition =  viewMatrix * modelMatrix * vec4(pos, 1.0);
  col = min(vec3(0.0) + ads(eyePosition, eyeNormal), 1.0);
  gl_Position = 0.3 * projectionMatrix * viewMatrix * modelMatrix *
    vec4(pos, 1.0); 
}