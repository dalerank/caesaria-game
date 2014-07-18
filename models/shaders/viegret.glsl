uniform sampler2D tex;
varying vec2 vTexCoord;

void main() {
  vec4 color = texture2D(tex, vTexCoord.xy);
  color.rgb = 1.0 - color.rgb;
  gl_FragColor = color;
}
