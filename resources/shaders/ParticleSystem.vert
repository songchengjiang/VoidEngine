attribute vec3 a_position;
attribute vec2 a_texCoord0;
attribute vec4 a_color0;

varying vec4 v_color;
varying vec2 v_texcoord;

void main()                                                 
{
    v_color = a_color0;
	v_texcoord = a_texCoord0;
	gl_Position = vec4(a_position, 1.0);
}
