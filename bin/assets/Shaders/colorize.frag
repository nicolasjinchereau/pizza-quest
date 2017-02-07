varying vec2 vTexCoord;
uniform sampler2D uMainTex;
uniform vec4 uTint;
uniform float uTintStrength;

void main()
{
	vec4 col = texture2D(uMainTex, vTexCoord);
	float g = max(max(col.r, col.g), col.b);
	col.rgb = lerp(col.rgb, uTint.rgb * g, uTintStrength);
	gl_FragColor = col;
};
