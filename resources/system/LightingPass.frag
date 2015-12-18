in vec3 v_normInView;

layout(location=0) out vec4 normal;

#if VE_PLATFORM == VE_PLATFORM_ANDROID
vec3 encode (vec3 normal)
{
    float p = sqrt(normal.z * 8.0 + 8.0);
    return vec3(normal.xy / p + 0.5, 0.0);
}
#endif

void main(){
#if VE_PLATFORM == VE_PLATFORM_ANDROID
	normal = vec4(encode(normalize(v_normInView)), 1.0);
#else
	normal = vec4(normalize(v_normInView), 1.0);
#endif
}