#ifdef VE_USE_DIFFUSE_TEXTURE
uniform sampler2D u_diffuseTex;
#endif

in vec3 v_color;
in vec2 v_texcoord;

layout(location=0) out vec4 RT0;
layout(location=1) out vec4 RT1;
layout(location=2) out vec4 RT2;
             
void main(){

    RT0 = vec4(0.0);
    RT2 = vec4(0.0);

#ifdef VE_USE_DIFFUSE_TEXTURE
	RT1.xyz = v_color * texture(u_diffuseTex, v_texcoord).xyz;
#else
	RT1.xyz = v_color;
#endif
    RT1.w = 0.0;
    
}