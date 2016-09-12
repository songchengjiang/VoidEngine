uniform sampler2D u_colorTexure;
uniform float u_nSamples;
uniform float u_flareDispersal;
uniform float u_flareHaloWidth;
uniform vec3 u_flareChromaticDistortion;

in vec2 v_texcoord;
layout(location = 0) out vec4 fragColor;

vec3 textureDistorted(sampler2D tex, vec2 sample_center, vec2 sample_vector, vec3 distortion) {
    return vec3(
                texture(tex, sample_center + sample_vector * distortion.r).r,
                texture(tex, sample_center + sample_vector * distortion.g).g,
                texture(tex, sample_center + sample_vector * distortion.b).b
                );
}

void main() {
    vec2 imageCenter = vec2(0.5);
    vec2 sampleVector = (imageCenter - v_texcoord) * u_flareDispersal;
    vec2 haloVector = normalize(sampleVector) * u_flareHaloWidth;
    
    vec3 color = textureDistorted(u_colorTexure, v_texcoord + haloVector, haloVector, u_flareChromaticDistortion).rgb * 3.0;
    for(float i = 0.0; i < u_nSamples; i += 1.0){
        vec2 offset = sampleVector * i;
        color += textureDistorted(u_colorTexure, v_texcoord + offset, offset, u_flareChromaticDistortion).rgb;
    }
    
    fragColor = vec4(color, 1.0);
}