#ifdef VE_USE_DIFFUSE_TEXTURE
uniform sampler2D u_diffuseTex;
#endif

in vec4 v_color;
in vec2 v_texcoord;

layout(location=0) out vec4 fragColor;
             
void main(){

#ifdef VE_USE_DIFFUSE_TEXTURE
	fragColor = v_color * texture(u_diffuseTex, v_texcoord);
#else
	fragColor = v_color;
#endif
    
}