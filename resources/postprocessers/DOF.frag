uniform sampler2D u_blurTexture;
uniform sampler2D u_sceneTexture;
uniform highp sampler2D u_depthTexture;

uniform float u_focus;
uniform float u_range;

in vec2 v_texcoord; 
layout(location = 0) out vec4 fragColor;

void main() {
    vec4 blur   = texture(u_blurTexture, v_texcoord);
	highp float depth = texture(u_depthTexture, v_texcoord).x;
    vec4 scene  = texture(u_sceneTexture, v_texcoord);

	fragColor = mix(scene, blur, clamp(u_range * abs(u_focus - depth), 0.0, 1.0));
}