#version  430 compatibility
uniform mat4 osg_ModelViewMatrix;
uniform mat3 osg_NormalMatrix;
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
in vec3 tePosition[3];
in vec3 tePatchDistance[3];
out vec3 gFacetNormal;
out vec3 gPatchDistance;
out vec3 gTriDistance;
in vec2 TexCoord_GS_in[];
out vec2 TexCoord_FS_in;
out vec3 Position_FS_in;
in vec3 Normal_GS_in[];
out vec3 Normal_FS_in;


in  vec3 lightv_GS_in[3];
out vec3 lightv_FS_in;

out vec4 gColor;
void main(){

   //lightv_FS_in = lightv_GS_in;


    vec3 A = tePosition[2] - tePosition[0];
    vec3 B = tePosition[1] - tePosition[0];
    gFacetNormal = osg_NormalMatrix * normalize(cross(A, B));

    gPatchDistance = tePatchDistance[0];
    gTriDistance = vec3(1, 0, 0);
    gColor = osg_ModelViewMatrix[0];
    TexCoord_FS_in = TexCoord_GS_in[0];
    Normal_FS_in = Normal_GS_in[0];
    Position_FS_in = gl_in[0].gl_Position.xyz;
    
    lightv_FS_in = lightv_GS_in[0];
    
    gl_Position = gl_in[0].gl_Position   ; 
	
	EmitVertex();

    gPatchDistance = tePatchDistance[1];
    gTriDistance = vec3(0, 1, 0);
    gColor = osg_ModelViewMatrix[1];
    TexCoord_FS_in = TexCoord_GS_in[1];
    Normal_FS_in = Normal_GS_in[1]; 
    Position_FS_in = gl_in[1].gl_Position.xyz;
    
    lightv_FS_in = lightv_GS_in[1];
    
    gl_Position = gl_in[1].gl_Position  ;

	 EmitVertex();

    gPatchDistance = tePatchDistance[2];
    gTriDistance = vec3(0, 0, 1);
    gColor = osg_ModelViewMatrix[2];
    TexCoord_FS_in = TexCoord_GS_in[2];
    Normal_FS_in = Normal_GS_in[2];
    Position_FS_in = gl_in[2].gl_Position.xyz;
    
    
    lightv_FS_in = lightv_GS_in[2];
    
    gl_Position = gl_in[2].gl_Position ;

	EmitVertex();
    
	EndPrimitive();
    //Position_FS_in = gl_Position;
   
 gFacetNormal = normalize (  cross (gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz,
	                gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz));

}
