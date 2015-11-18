precision mediump sampler3D;
uniform sampler2D u_colorTex;
uniform sampler3D u_asc2Tex;
uniform vec2 u_iNumTiles;
uniform vec2 u_iNumTiles2;
uniform vec2 u_numTiles;
uniform vec4 u_lum;
uniform float u_charBias;

in vec2 v_texcoord; 
layout(location = 0) out vec4 fragColor;

void main() {

    vec3 local;

	//sample RT
	local.xy = mod(v_texcoord, u_iNumTiles);
	vec2 middle = v_texcoord - local.xy;
	local.xy = local.xy * u_numTiles;
	
	//iNumTiles2 = iNumTiles / 2
	middle = middle + u_iNumTiles2;
	vec4 color = texture(u_colorTex, middle);
	
	//multiply luminance by charbias , beacause not all slices of the ascii
	//volume texture are used
	local.z = dot(color , u_lum) * u_charBias;
	
	//fix to brighten the dark pixels with small characters
	//color *= lerp(2.0,1.0, local.z);
	
	color *= texture(u_asc2Tex, local).x;

	fragColor = color;
}