uniform sampler2D tex;
varying vec2 vTexCoord;

void main()
{
   vec4 sum = vec4(0);
   int j;
   int i;

   for( i= -5 ;i < 5; i++)
   {
        for (j = -5; j < 5; j++)
        {
            sum += texture2D(tex, vTexCoord + vec2(j, i)*0.004) * 0.08;
        }
   }

   if (texture2D(tex, vTexCoord).r < 0.3)
   {
      gl_FragColor = sum*sum*0.012 + texture2D(tex, vTexCoord);
   }
   else
   {
      if (texture2D(tex, vTexCoord).r < 0.5)
      {
          gl_FragColor = sum*sum*0.009 + texture2D(tex, vTexCoord);
      }
      else
      {
          gl_FragColor = sum*sum*0.0075 + texture2D(tex, vTexCoord);
      }
   }
}
