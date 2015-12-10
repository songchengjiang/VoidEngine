uniform sampler2D u_Texture;
uniform float weights[5];         
uniform float height; 
in vec2 v_texcoord; 
layout(location = 0) out vec4 fragColor;

void main()
{
	float dy = 1.0 / height;        
	vec4 sum = texture(u_Texture, v_texcoord) * weights[0];   
	for (int i = 1; i < 5; ++i){                         
		sum += texture(u_Texture, v_texcoord + vec2(0.0, float(i) * dy)) * weights[i];  
		sum += texture(u_Texture, v_texcoord - vec2(0.0, float(i) * dy)) * weights[i];  
	}
	fragColor = vec4(sum.xyz, 1.0);
}