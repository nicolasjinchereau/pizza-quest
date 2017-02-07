varying vec2 vTexCoord;
uniform sampler2D uMainTex;

void main()
{
	gl_FragColor = texture2D(uMainTex, vTexCoord);
};
