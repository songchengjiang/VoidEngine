uniform sampler2D u_colorTexure;
uniform sampler2D u_sceneTexure;
uniform sampler2D u_distTexure;
uniform float u_amount;
uniform float u_colorWidth;
uniform float u_colorHeight;
uniform vec2  u_distSize;

in vec2 v_texcoord;
layout(location = 0) out vec4 fragColor;

void main() {

    vec4 originalColor = texture(u_colorTexure, v_texcoord);
    vec4 secondaryColor = texture(u_distTexure, v_texcoord);
    vec4 scene = texture(u_sceneTexure, v_texcoord);
    
    fragColor = vec4(scene.rgb + originalColor.rgb * secondaryColor.rgb * vec3(u_amount), 1.0);
}
