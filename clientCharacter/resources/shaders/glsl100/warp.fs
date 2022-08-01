#version 100

precision mediump float;

// Uniform input vertex attributes (from vertex shader)
uniform vec4 _TexRotationVec;
uniform highp float _power;
uniform highp float _alpha;
uniform sampler2D texture0; //renderedtex
uniform sampler2D texture1; //maptex


// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;



bool inside(vec2 uv){
   highp float EPS = 1e-3;
   return EPS <= uv.x && uv.x <= float(1)-EPS && EPS <= uv.y && uv.y <= float(1)-EPS;
}

void main()
{
   vec4 BLACK = vec4(0, 0, 0, 0);
   vec4 RED = vec4(1, 0, 0, 1);
   vec2 HALF = vec2(0.5, 0.5);

   mat2 rotMat = mat2 (_TexRotationVec.x, _TexRotationVec.y, _TexRotationVec.z, _TexRotationVec.w);
   vec2 mapUV = rotMat*(fragTexCoord-HALF)+HALF;

   
   if (!inside(mapUV)) {
      gl_FragColor = BLACK;
   } 

   vec4 map = texture2D(texture1, mapUV);

   vec2 renderedTexUV = vec2(map.x, map.y);
   if (!inside(renderedTexUV)) {
     gl_FragColor = BLACK;
   } 

   vec4 temTexture = texture2D(texture0, renderedTexUV);
   
   gl_FragColor = _alpha * vec4 (pow(temTexture[0], _power),pow(temTexture[1], _power), pow(temTexture[2], _power), pow(temTexture[3], _power));


}

