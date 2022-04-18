#version 330

precision mediump float;

// Uniform input vertex attributes (from vertex shader)
uniform vec4 _TexRotationVec;
uniform highp float _power;
uniform highp float _alpha;
uniform sampler2D texture0;
uniform sampler2D texture1;

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;


out vec4 finalColor;


bool inside(vec2 uv){
   highp float EPS = 0.001;
   return EPS <= uv.x && uv.x <= float(1)-EPS && EPS <= uv.y && uv.y <= float(1)-EPS;
}

void main()
{
   vec4 BLACK = vec4(0, 1, 0, 0);
   vec4 RED = vec4(1, 0, 0, 1);
   vec2 HALF = vec2(0.5, 0.5);

   mat2 rotMat = mat2 (_TexRotationVec.x, _TexRotationVec.y, _TexRotationVec.z, _TexRotationVec.w);
   vec2 mapUV = rotMat*(fragTexCoord-HALF)+HALF;
   
   if (!inside(mapUV)) {
      finalColor = RED;
   } 

   vec4 map = texture(texture1, mapUV);

   vec2 renderedTexUV = vec2(map.x, map.y);
   if (!inside(renderedTexUV)) {
     finalColor = BLACK;
   } 

   vec4 temTexture = texture(texture0, renderedTexUV);
   
   finalColor = _alpha * vec4 (pow(temTexture[0], _power),pow(temTexture[1], _power), pow(temTexture[2], _power), pow(temTexture[3], _power));



}

