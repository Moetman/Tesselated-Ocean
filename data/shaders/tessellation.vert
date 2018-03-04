#version 430 compatibility

layout (location = 1) in vec3 Position; 
layout (location = 0) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
//in vec4 osg_Vertex;\n"
out vec3 vPosition;
out vec2 TexCoord_CS_in;
out vec3 Normal_CS_in; 

void main()
{
	vPosition =Position.xyz;
	TexCoord_CS_in = TexCoord;
	Normal_CS_in = Normal;
}