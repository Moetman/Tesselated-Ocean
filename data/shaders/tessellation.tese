#version  430 compatibility

const float pi = 3.14159;


layout(triangles, equal_spacing, cw) in;
in vec3 tcPosition[];
out vec3 tePosition;
out vec3 tePatchDistance;
in vec2 TexCoord_ES_in[];
out vec2 TexCoord_GS_in;
in vec3 Normal_ES_in[];    

out vec3 Normal_GS_in;

out vec3 lightv_GS_in;

uniform mat4 osg_ProjectionMatrix;
uniform mat4 osg_ModelViewMatrix;
uniform float time;


const int numWaves = 2;


uniform sampler2D oceanHeightmap;
//uniform int numWaves;
uniform vec3 LightPosition;
uniform float waterHeight;

uniform float amplitudes[numWaves];
uniform float wavelengths[numWaves];
uniform float speeds[numWaves];
uniform vec3 directions[numWaves];
uniform float steepness[numWaves];




//uniform vec4 waveCharacteristics[numWaves];



#define textureSize 512.0
#define texelSize 1.0 / 512.0

vec4 texture2D_bilinear(  sampler2D tex, vec2 uv )
{
	vec2 f = fract( uv.xy * textureSize );
	vec4 t00 = texture2D( tex, uv );
	vec4 t10 = texture2D( tex, uv + vec2( texelSize, 0.0 ));
	vec4 tA = mix( t00, t10, f.x );
	vec4 t01 = texture2D( tex, uv + vec2( 0.0, texelSize ) );
	vec4 t11 = texture2D( tex, uv + vec2( texelSize, texelSize ) );
	vec4 tB = mix( t01, t11, f.x );
	return mix( tA, tB, f.y );
}


vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)    
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}      
                                                                                                
vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)                                                   
{                                                                                               
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;   
}




vec3 GerstnerPosition(vec3 pos, float range,  float time)
{

int N = numWaves;
float S = 1.0;

vec3 position = vec3(0.0,0.0,0.0);
float spd = 1.1;

for(int i=0; i<numWaves; ++i)
{
	
	float A = amplitudes[i] ; 		       //Amplitude
	float frequency = 2*pi / wavelengths[i];       //Frequency
	float phase = speeds[i] * frequency;	       //Phase	
	float Qi = steepness[i]/(frequency * A * 4.0); // Steepness
	float theta = dot(directions[i].xy, pos.xy) ;
	
    
	 
		
		  //position += vec3(Qi * A * directions[i].x * C,  Qi * A * directions[i].y * C,  A * S  );
		  //position += vec3(Qi * A * directions[i].x * C, A * S , Qi * A * directions[i].z );
    
    
	
	
	position.x += amplitudes[i] * cos(theta * frequency + time * phase);
	position.y += amplitudes[i] * cos(theta * frequency + time * phase);
	position.z += amplitudes[i] * sin(theta * frequency + time * phase);
	
	


}

pos.z += position.z;

return pos;


 


}


float dWavedx(int i, float x, float y) {

    float frequency = 2*pi/wavelengths[i];
    float phase = speeds[i] * frequency;
    float theta = dot(directions[i].xy, vec2(x, y));
    float A = amplitudes[i] * directions[i].x * frequency;
    
    //return A * cos(theta * frequency + time * phase);
     return A * cos(theta * frequency  + (time * phase) );
}

float dWavedy(int i, float x, float y) {

    float frequency = 2*pi/wavelengths[i];
    float phase = speeds[i] * frequency;
    float theta = dot(directions[i].xy, vec2(x, y));

    float A = amplitudes[i] * directions[i].y * frequency;
    
    
    //return A * sin(theta * frequency + time * phase);
    return A * cos(theta * frequency  + (time * phase) );
}

vec3 waveNormal(float x, float y) {
    float dx = 0.0;
    float dy = 0.0;
    for (int i = 0; i < numWaves; ++i) {   //numWaves
        dx +=   dWavedx(i, x, y);
        dy +=   dWavedy(i, x, y);
    }
    vec3 n = vec3(-dx, -dy, 1.0);
    return normalize(n);
}

/*

struct WaveParameters
{
    float wavelength; //0 
    float steepness;  //1
    float speed;      //2	
    float kAmpOverLen;//3
    Vector2 wave_dir; //4
};

*/


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void main(){
    vec3 p0 = gl_TessCoord.x * tcPosition[0];
    vec3 p1 = gl_TessCoord.y * tcPosition[1];
    vec3 p2 = gl_TessCoord.z * tcPosition[2];
    
    tePatchDistance = gl_TessCoord;
	 tePosition = (p0 + p1 + p2);

     TexCoord_GS_in = interpolate2D(TexCoord_ES_in[0], TexCoord_ES_in[1], TexCoord_ES_in[2]);
     Normal_GS_in = interpolate3D(Normal_ES_in[0], Normal_ES_in[1], Normal_ES_in[2]);  
     
     
     vec3 P, N, B, T;
     
     P = GerstnerPosition(tePosition.xyz, 1.0f,  time);
		 	
	
	
	 vec3 lightv = vec3(dot(LightPosition, B), dot(LightPosition, T),dot(LightPosition, N));
     lightv_GS_in = (lightv);
	
	
	
	Normal_GS_in = waveNormal(P.x, P.y) ;
	
	
	
	gl_Position = osg_ProjectionMatrix * osg_ModelViewMatrix *  vec4(P,1.0);
	
	
	
	


}







