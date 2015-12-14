layout (location = ATTR_POSITION) in vec3 position;      
uniform mat4 u_ModelViewProjectMat;              

void main()                                                 
{     
	gl_Position = u_ModelViewProjectMat * vec4(position, 1.0);
}