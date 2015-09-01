#version 430        
uniform sampler2D u_diffuseTex;
in vec3 v_position;   
in vec3 v_normal;                   
in vec2 v_texcoord; 
in float v_linearDepth;  
layout(location=0) out vec4 o_position;
layout(location=1) out vec4 o_normal;
layout(location=2) out vec4 o_color;                  
void main(){
    o_position = vec4(v_position, 1.0);
	o_normal = vec4(v_normal, v_linearDepth);
	o_color = texture(u_diffuseTex, v_texcoord);
}