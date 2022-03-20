#version 140

precision mediump float;

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 _fragColor;

uniform vec4 _TexRotationVec;
uniform highp float _power;
uniform highp float _alpha;
uniform sampler2D RenderedTex;
uniform sampler2D MapTex;

bool inside(vec2 uv){
   highp float EPS = 0.001;
   return EPS <= uv.x && uv.x <= float(1)-EPS && EPS <= uv.y && uv.y <= float(1)-EPS;
}

void main()
{

   vec4 BLACK = vec4(0, 0, 0, 0);
   vec2 HALF = vec2(0.5, 0.5);
   mat2 rotMat = mat2 (_TexRotationVec.x, _TexRotationVec.y, _TexRotationVec.z, _TexRotationVec.w);
   vec2 mapUV = rotMat*(fragTexCoord-HALF)+HALF;
   
   if (!inside(mapUV)) {
      _fragColor = BLACK;
   }

   vec4 map = texture(MapTex, mapUV);
   vec2 renderedTexUV = vec2(map.x, map.y);
   if (!inside(renderedTexUV)) {
     _fragColor = BLACK;
   }

   vec4 temTexture = texture(RenderedTex, renderedTexUV);
   _fragColor = _alpha * vec4 (pow(temTexture[0], _power),pow(temTexture[1], _power), pow(temTexture[2], _power), pow(temTexture[3], _power));


}