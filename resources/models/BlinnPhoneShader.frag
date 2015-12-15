uniform float u_screenWidth;
uniform float u_screenHeight;
uniform vec3 u_ambient;        
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess; 
uniform float u_opacity;         
#ifdef VE_USE_DIFFUSE_TEXTURE
uniform sampler2D u_diffuseTex;
#endif

in vec4 v_position;
in vec4 v_normalAndepth;                   
in vec2 v_texcoord;

layout(location=0) out vec4 fragColor;
#ifdef VE_USE_DEFERRED_PATH
layout(location=1) out vec4 position;
layout(location=2) out vec4 normAndepth;
#endif
             
void main(){
    
#ifdef VE_USE_DEFERRED_PATH
#ifdef VE_USE_DIFFUSE_TEXTURE
    fragColor = clamp(vec4(texture(u_diffuseTex, v_texcoord).xyz, u_opacity), 0.0, 1.0);
#else //NOT VE_USE_DIFFUSE_TEXTURE
    fragColor = clamp(vec4(u_diffuse + u_specular + u_ambient, u_opacity), 0.0, 1.0);
#endif
    position = vec4(v_position.xyz, u_shininess);
    normAndepth = v_normalAndepth;
#else //NOT VE_USE_DEFERRED_PATH
#ifdef VE_USE_LIGHTS
	vec2 lightTexCoords = gl_FragCoord.xy / vec2(u_screenWidth, u_screenHeight);
	vec4 lightVal = texture(ve_lightTex, lightTexCoords);
    vec3 diffactor = lightVal.rgb;
    vec3 specfactor = lightVal.rgb * pow(lightVal.a, u_shininess);
#ifdef VE_USE_DIFFUSE_TEXTURE
    fragColor = clamp(vec4(diffactor * u_diffuse * texture(u_diffuseTex, v_texcoord).xyz + specfactor * u_specular + u_ambient, u_opacity), 0.0, 1.0);
#else //NOT VE_USE_DIFFUSE_TEXTURE
    fragColor = clamp(vec4(diffactor * u_diffuse + specfactor * u_specular + u_ambient, u_opacity), 0.0, 1.0);
#endif //VE_USE_DIFFUSE_TEXTURE
#else //NOT VE_USE_LIGHTS
#ifdef VE_USE_DIFFUSE_TEXTURE
    fragColor = clamp(vec4(texture(u_diffuseTex, v_texcoord).xyz, u_opacity), 0.0, 1.0);
#else //NOT VE_USE_DIFFUSE_TEXTURE
    fragColor = clamp(vec4(u_diffuse + u_specular + u_ambient, u_opacity), 0.0, 1.0);
#endif //VE_USE_DIFFUSE_TEXTURE
#endif //END VE_USE_LIGHTS
#endif //END VE_USE_DEFERRED_PATH
}