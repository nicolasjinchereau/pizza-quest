uniform mat3 uMtxMVP;
attribute vec2 aPosition;
attribute vec2 aTexCoord;
varying vec2 vTexCoord;

void main()
{
	gl_Position = vec4(uMtxMVP * vec3(aPosition.x, aPosition.y, 1.0), 1.0);
	vTexCoord = aTexCoord;
};
