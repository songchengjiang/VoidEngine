#ifdef VE_USE_DIFFUSE_TEXTURE
uniform sampler2D u_diffuseTex;
#endif
uniform vec3 u_diffuse;
uniform float u_alpha;

varying vec2 v_texcoord;
             
void main(){

#ifdef VE_USE_DIFFUSE_TEXTURE
    vec4 color = texture2D(u_diffuseTex, v_texcoord);
	gl_FragColor = vec4(u_diffuse * color.rgb, color.a * u_alpha);
#else
	gl_FragColor = vec4(u_diffuse, u_alpha);
#endif
    
}
