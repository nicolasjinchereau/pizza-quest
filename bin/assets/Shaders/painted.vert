uniform mat3 uMtxMVP;
attribute vec2 aPosition;
attribute vec4 aColor;
attribute vec2 aTexCoord;

varying vec2 vTexCoord;
varying vec4 vColor;

void main()
{
	gl_Position = vec4(uMtxMVP * vec3(aPosition.x, aPosition.y, 1.0), 1.0);
	vTexCoord = aTexCoord;
	vColor = aColor;
};
