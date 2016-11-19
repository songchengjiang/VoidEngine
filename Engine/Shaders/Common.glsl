#define STRINGIFY(A)  #A

#define SHADER_COMMON_FUNCTION STRINGIFY(                                                                                  \
precision highp float;                                                                                            \n\
vec3 decode(vec3 encoded) {                                                                                       \n\
    float nz = floor(encoded.z * 255.0) / 16.0;                                                                   \n\
    vec2 dec = encoded.xy + vec2(floor(nz) / 16.0, fract(nz)) / 255.0;                                            \n\
    vec2 fenc = dec * 4.0 - 2.0;                                                                                  \n\
    float f = dot(fenc, fenc);                                                                                    \n\
    float g = sqrt(1.0 - f / 4.0);                                                                                \n\
    return vec3(fenc * g, 1.0 - f / 2.0);                                                                         \n\
}                                                                                                                 \n\
                                                                                                                  \n\
vec3 linearColor(vec3 col) {                                                                                      \n\
    return pow(col, vec3(0.45454545));                                                                            \n\
}                                                                                                                 \n\
                                                                                                                  \n\
const float PI = 3.1415926535;                                                                                    \n\
float OrenNayar(vec3 norm, vec3 ldir, vec3 vdir, float NdotL, float NdotV, float roughness) {                     \n\
                                                                                                                  \n\
    float roughness2 = roughness * roughness;                                                                     \n\
    float A = 1.0 - 0.5 * (roughness2 / (roughness2 + 0.33));                                                     \n\
    float B = 0.45 * (roughness2 / (roughness2 + 0.09));                                                          \n\
    float C = max(0.0, dot(normalize(vdir - norm * NdotV), normalize(ldir - norm * NdotL)));                      \n\
    float NdotLAgl = acos(NdotL);                                                                                 \n\
    float NdotVAgl = acos(NdotV);                                                                                 \n\
    float alpha = max(NdotLAgl, NdotVAgl);                                                                        \n\
    float beta = min(NdotLAgl, NdotVAgl);                                                                         \n\
                                                                                                                  \n\
    return NdotL * max(0.0, (A + B * sin(alpha) * tan(beta) * C));                                                \n\
}                                                                                                                 \n\
                                                                                                                  \n\
float CookTorrance(float NdotL, float NdotV, float HdotV, float NdotH, float roughness, float fresnel) {          \n\
    float alpha = pow(1.0 - (1.0 - roughness) * 0.7, 6.0);                                                        \n\
    float alpha2 = alpha * alpha;                                                                                 \n\
                                                                                                                  \n\
    float k = (0.8 + 0.5 * alpha) * (0.8 + 0.5 * alpha) / 2.0;                                                    \n\
    float Gl = NdotL / (NdotL * (1.0 - k) + k);                                                                   \n\
    float Gv = NdotV / (NdotV * (1.0 - k) + k);                                                                   \n\
    float G = Gl * Gv;                                                                                            \n\
                                                                                                                  \n\
    float F = fresnel + (1.0 - fresnel) * pow(1.0 - HdotV, 5.0);                                                  \n\
                                                                                                                  \n\
    float Db = PI * pow(NdotH * NdotH * (alpha2 - 1.0) + 1.0, 2.0);                                               \n\
    float D = alpha2 / Db; 	                                                                                      \n\
                                                                                                                  \n\
    return max(0.0, (F * D * G) / (4.0f * NdotL * NdotV));                                                        \n\
}                                                                                                                 \n\
)