uniform sampler2D u_colorTexure;
uniform float u_exposure;
uniform float u_decay;
uniform float u_density;
uniform float u_weight;
uniform vec2 u_lightPositionOnScreen;

in vec2 v_texcoord;
layout(location = 0) out vec4 fragColor;

const int NUM_SAMPLES = 100;

void main() {
    vec2 texCoord = v_texcoord;
    vec2 deltaTextCoord = vec2(texCoord - u_lightPositionOnScreen);
    deltaTextCoord *= 1.0 / float(NUM_SAMPLES) * u_density;
    float illuminationDecay = 1.0;
    
    vec4 color = vec4(0.0);
    for (int i = 0; i < NUM_SAMPLES; ++i) {
        texCoord -= deltaTextCoord;
        vec4 samples = texture(u_colorTexure, texCoord);
        samples *= illuminationDecay * u_weight;
        color += samples;
        illuminationDecay *= u_decay;
    }
    
    fragColor = color * u_exposure;
}
