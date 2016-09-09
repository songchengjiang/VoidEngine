uniform sampler2D u_Texture;
uniform float width;
uniform float height;

in vec2 v_texcoord; 
layout(location = 0) out vec4 fragColor;

void main() {

    vec4 accum = vec4(0.0, 0.0, 0.0, 0.0);
    vec2 texelSize = vec2(1.0 / width, 1.0 / height);
    
    // Get colour from source
    accum += texture(u_Texture, v_texcoord + texelSize * vec2(-1.0, -1.0));
    accum += texture(u_Texture, v_texcoord + texelSize * vec2( 0.0, -1.0));
    accum += texture(u_Texture, v_texcoord + texelSize * vec2( 1.0, -1.0));
    accum += texture(u_Texture, v_texcoord + texelSize * vec2(-1.0,  0.0));
    accum += texture(u_Texture, v_texcoord + texelSize * vec2( 0.0,  0.0));
    accum += texture(u_Texture, v_texcoord + texelSize * vec2( 1.0,  0.0));
    accum += texture(u_Texture, v_texcoord + texelSize * vec2(-1.0,  1.0));
    accum += texture(u_Texture, v_texcoord + texelSize * vec2( 0.0,  1.0));
    accum += texture(u_Texture, v_texcoord + texelSize * vec2( 1.0,  1.0));
    
    // take average of 9 samples
    accum *= 0.1111111111111111;
    
    fragColor = accum;
}