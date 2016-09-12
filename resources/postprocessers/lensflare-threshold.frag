uniform sampler2D u_colorTexure;
uniform float u_threshold;

in vec2 v_texcoord;
layout(location = 0) out vec4 fragColor;

void main() {
    vec4 color = texture(u_colorTexure, v_texcoord) - vec4(vec3(u_threshold), 0.0);
    
    fragColor = color;
}
