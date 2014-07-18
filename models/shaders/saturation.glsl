uniform sampler2D tex;
varying vec2 vTexCoord;
const vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);
uniform float saturation;

void main() 
{
  vec4 textureColor = texture2D(tex, vTexCoord.xy);
  float luminance = dot(textureColor.rgb, luminanceWeighting);
  vec3 greyScaleColor = vec3(luminance);

  gl_FragColor = vec4(mix(greyScaleColor, textureColor.rgb, saturation), textureColor.w);
}
