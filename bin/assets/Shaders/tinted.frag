varying vec2 vTexCoord;
uniform sampler2D uMainTex;
uniform vec4 uTint;

void main()
{
	vec4 col = texture2D(uMainTex, vTexCoord);
	col *= uTint;
	gl_FragColor = col;
};
