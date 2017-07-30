#ifdef VE_USE_DIFFUSE_TEXTURE
uniform sampler2D u_diffuseTex;
#endif

varying vec4 v_color;
varying vec2 v_texcoord;
             
void main(){

#ifdef VE_USE_DIFFUSE_TEXTURE
	gl_FragColor = v_color * texture2D(u_diffuseTex, v_texcoord);
#else
	gl_FragColor = v_color;
#endif
    
}
