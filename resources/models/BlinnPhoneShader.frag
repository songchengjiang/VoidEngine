uniform float u_screenWidth;
uniform float u_screenHeight;
uniform vec3 u_ambient;        
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess; 
uniform float u_opacity;
uniform float u_lightMask;
uniform float u_roughness;
uniform float u_fresnel; 
#ifdef VE_USE_DIFFUSE_TEXTURE
uniform sampler2D u_diffuseTex;
#endif
#ifdef VE_USE_SPECULAR_TEXTURE
uniform sampler2D u_specularTex;
#endif
#ifdef VE_USE_NORMAL_TEXTURE
uniform sampler2D u_normalTex;
#endif

in vec3 v_viewNormal;
#ifdef VE_USE_NROMAL_MAPPING
in vec3 v_viewTangent;
in vec3 v_viewBitangent; 
#endif
            
in vec2 v_texcoord;

layout(location=0) out vec4 RT0;
//#ifdef VE_USE_DEFERRED_PATH
layout(location=1) out vec4 RT1;
layout(location=2) out vec4 RT2;
//#endif

vec2 encode (vec3 normal)
{
    float p = sqrt(normal.z * 8.0 + 8.0);
    return vec2(normal.xy / p + 0.5);
}
             
void main(){

#ifdef VE_USE_NROMAL_MAPPING
	mat3 normCoords = mat3(v_viewTangent, v_viewBitangent, v_viewNormal);
	vec3 norm = texture(u_normalTex, v_texcoord).xyz * 2.0 - 1.0;
	RT0.xy = encode(normCoords * norm);
#else
	RT0.xy = encode(v_viewNormal);
#endif
	RT0.z = u_lightMask;

#ifdef VE_USE_DIFFUSE_TEXTURE
	RT1.xyz = u_diffuse * texture(u_diffuseTex, v_texcoord).xyz;
#else
	RT1.xyz = u_diffuse;
#endif
	RT1.w = u_roughness;

#ifdef VE_USE_SPECULAR_TEXTURE
	RT2.xyz = u_specular * texture(u_specularTex, v_texcoord).xyz;
#else
	RT2.xyz = u_specular;
#endif
	RT2.w = u_fresnel;
	
//#ifdef VE_USE_DEFERRED_PATH
//#ifdef VE_USE_DIFFUSE_TEXTURE
//    fragColor = clamp(vec4(texture(u_diffuseTex, v_texcoord).xyz, u_opacity), 0.0, 1.0);
//#else //NOT VE_USE_DIFFUSE_TEXTURE
//    fragColor = clamp(vec4(u_diffuse + u_specular + u_ambient, u_opacity), 0.0, 1.0);
//#endif
//    position = vec4(v_position.xyz, u_shininess);
//    normAndepth = v_normalAndepth;
//#else //NOT VE_USE_DEFERRED_PATH
//#ifdef VE_USE_LIGHTS
//	vec2 lightTexCoords = gl_FragCoord.xy / vec2(u_screenWidth, u_screenHeight);
//	vec4 lightVal = texture(ve_lightTex, lightTexCoords);
//    vec3 diffactor = lightVal.rgb;
//    vec3 specfactor = lightVal.rgb * pow(lightVal.a, u_shininess);
//#ifdef VE_USE_DIFFUSE_TEXTURE
//    fragColor = clamp(vec4(diffactor * u_diffuse * texture(u_diffuseTex, v_texcoord).xyz + specfactor * u_specular + u_ambient, u_opacity), 0.0, 1.0);
//#else //NOT VE_USE_DIFFUSE_TEXTURE
//    fragColor = clamp(vec4(diffactor * u_diffuse + specfactor * u_specular + u_ambient, u_opacity), 0.0, 1.0);
//#endif //VE_USE_DIFFUSE_TEXTURE
//#else //NOT VE_USE_LIGHTS
//#ifdef VE_USE_DIFFUSE_TEXTURE
//    fragColor = clamp(vec4(texture(u_diffuseTex, v_texcoord).xyz, u_opacity), 0.0, 1.0);
//#else //NOT VE_USE_DIFFUSE_TEXTURE
//    fragColor = clamp(vec4(u_diffuse + u_specular + u_ambient, u_opacity), 0.0, 1.0);
//#endif //VE_USE_DIFFUSE_TEXTURE
//#endif //END VE_USE_LIGHTS
//#endif //END VE_USE_DEFERRED_PATH
}