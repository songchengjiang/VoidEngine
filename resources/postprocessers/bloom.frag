uniform sampler2D u_bloomBlur;
uniform sampler2D u_sceneTex;
uniform float     u_exposure;
in vec2 v_texcoord; 
layout(location = 0) out vec4 fragColor;
const float gamma = 1.0 / 2.2;

void main()
{
	vec3 hdrColor = texture(u_bloomBlur, v_texcoord).rgb;
	hdrColor += texture(u_sceneTex, v_texcoord).rgb;
	vec3 result = vec3(1.0) - exp(-hdrColor * u_exposure);
	result = pow(result, vec3(gamma));
	fragColor = vec4(result, 1.0);
}
