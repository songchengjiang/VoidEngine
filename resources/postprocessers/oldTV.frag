precision mediump sampler3D;
uniform sampler2D u_colorTex;
uniform sampler3D u_randomTex;
uniform sampler3D u_noiseTex;
uniform float u_time;
uniform float u_sinTime;
uniform float u_distortionFreq;
uniform float u_distortionScale;
uniform float u_distortionRoll;
uniform float u_interference;
uniform float u_frameLimit;
uniform float u_frameShape;
uniform float u_frameSharpness;

in vec2 v_texcoord; 
layout(location = 0) out vec4 fragColor;

void main() {

	// Define a frame shape
	vec2 pos = v_texcoord * 2.0 - 1.0;
	float f = (1.0 - pos.x * pos.x) * (1.0 - pos.y * pos.y);
	float frame = clamp(u_frameSharpness * (pow(f, u_frameShape) - u_frameLimit), 0.0, 1.0);

	// Interference ... just a texture filled with rand()
	float rand = texture(u_randomTex, vec3(1.5 * pos, u_time)).x - 0.2;

	// Some signed noise for the distortion effect
	float noisy = texture(u_noiseTex, vec3(0.0, 0.5 * pos.y, 0.1 * u_time)).x - 0.5;

    // Repeat a 1 - x^2 (0 < x < 1) curve and roll it with sinus.
    float dst = fract(pos.y * u_distortionFreq + u_distortionRoll * u_sinTime);
    dst *= (1.0 - dst);
    // Make sure distortion is highest in the center of the image
    dst /= 1.0 + u_distortionScale * abs(pos.y);

	vec2 texcoord = v_texcoord;
	texcoord.x += u_distortionScale * noisy * dst;
	vec4 color = texture(u_colorTex, texcoord);

	fragColor = vec4(frame * (u_interference * rand + color));
}