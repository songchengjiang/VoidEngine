uniform sampler2D u_Texture;
uniform float weights[5];         
uniform float width; 
in vec2 v_texcoord; 
layout(location = 0) out vec4 fragColor;

void main()
{
	float dx = 1.0 / width;        
	vec4 sum = texture(u_Texture, v_texcoord) * weights[0];   
	for (int i = 1; i < 5; ++i){                         
		sum += texture(u_Texture, v_texcoord + vec2(float(i) * dx, 0.0)) * weights[i];  
		sum += texture(u_Texture, v_texcoord - vec2(float(i) * dx, 0.0)) * weights[i];  
	}
	fragColor = vec4(sum.xyz, 1.0);
}