varying vec2 vTexCoord;
varying vec4 vColor;

uniform sampler2D uMainTex;
uniform vec4 uTint;

void main()
{
	vec4 col = texture2D(uMainTex, vTexCoord);
	col *= vColor;
	gl_FragColor = col;
};
