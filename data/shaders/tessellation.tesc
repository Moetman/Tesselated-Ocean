#version  430 compatibility


uniform vec3 gEyeWorldPos;


layout(vertices = 3) out;
in vec3 vPosition[];
out vec3 tcPosition[];
uniform float TessLevelInner;
uniform float TessLevelOuter;
in vec2 TexCoord_CS_in[]; 
in vec3 Normal_CS_in[]; 
out vec2 TexCoord_ES_in[];
out vec3 Normal_ES_in[];


#define ID gl_InvocationID
                          

float GetTessLevel(float Distance0, float Distance1)                                            
{                                                                                               
    float AvgDistance = (Distance0 + Distance1) / 2.0;                                          
                                                                                                
    if (AvgDistance <= 2.0) {                                                                   
        return 7.0;                                                                            
        }                                                                                           
    else if (AvgDistance <= 5.0) {                                                              
        return 3.0;                                                                             
    }                                                                                           
    else {                                                                                      
        return 1.0;                                                                             
    }                                                                                           
}                                                                                               
                  





void main(){
    tcPosition[ID] = vPosition[ID];
    Normal_ES_in[ID]   = Normal_CS_in[ID];          
    
    
    if (ID == 0) {

	
	 // Calculate the distance from the camera to the three control points                 
	float EyeToVertexDistance0 = distance(gEyeWorldPos, vPosition[0]);                     
	float EyeToVertexDistance1 = distance(gEyeWorldPos, vPosition[1]);                     
	float EyeToVertexDistance2 = distance(gEyeWorldPos, vPosition[2]);                     

        //gl_TessLevelInner[0] = TessLevelInner;
        //gl_TessLevelOuter[0] = TessLevelOuter;
        //gl_TessLevelOuter[1] = TessLevelOuter;
        //gl_TessLevelOuter[2] = TessLevelOuter;
	// Calculate the tessellation levels                                                        
    	gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);            
    	gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);            
    	gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);            
    	gl_TessLevelInner[0] = gl_TessLevelOuter[2];                                                

    }
     TexCoord_ES_in[gl_InvocationID] = TexCoord_CS_in[gl_InvocationID];  
    


}