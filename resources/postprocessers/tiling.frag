uniform sampler2D u_colorTex;
uniform float u_NumTiles;
uniform float u_Threshold;
in vec2 v_texcoord; 
layout(location = 0) out vec4 fragColor;

void main() {
	vec3 EdgeColor = vec3(0.7, 0.7, 0.7);

    float size = 1.0/u_NumTiles;
    vec2 Pbase = v_texcoord - mod(v_texcoord, vec2(size));
    vec2 PCenter = vec2(Pbase + (size/2.0));
    vec2 st = (v_texcoord - Pbase)/size;
    vec4 c1 = vec4(0.0);
    vec4 c2 = vec4(0.0);
    vec4 invOff = vec4((1.0-EdgeColor),1.0);
    if (st.x > st.y) { c1 = invOff; }
    float threshholdB =  1.0 - u_Threshold;
    if (st.x > threshholdB) { c2 = c1; }
    if (st.y > threshholdB) { c2 = c1; }
    vec4 cBottom = c2;
    c1 = vec4(0.0);
    c2 = vec4(0.0);
    if (st.x > st.y) { c1 = invOff; }
    if (st.x < u_Threshold) { c2 = c1; }
    if (st.y < u_Threshold) { c2 = c1; }
    vec4 cTop = c2;
    vec4 tileColor = texture(u_colorTex, PCenter);
    vec4 result = tileColor + cTop - cBottom;
    fragColor = result;
}