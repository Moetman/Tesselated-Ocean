#version  430 compatibility


const float pi = 3.14159;


out vec4 FragColor;
in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;
in vec4 gColor;
in float gPrimitive;
in vec2 TexCoord_FS_in;
in vec3 Normal_FS_in;
in vec3 Position_FS_in;

in vec3 lightv_FS_in;

uniform vec3 LightPosition;
uniform vec3 gEyeWorldPos;

uniform vec3 DiffuseMaterial;
uniform vec3 AmbientMaterial;

//attribute vec3 Tangent;
//attribute vec3 Binormal;

uniform sampler2D baseTexture;

uniform sampler2D oceanHeightmap;
uniform sampler2D oceanNormal;

const int numWaves = 2;

uniform float time;
//uniform int numWaves;
uniform float amplitudes[numWaves];
uniform float wavelengths[numWaves];
uniform float speeds[numWaves];
uniform vec3 directions[numWaves];
uniform float steepness[numWaves];
//uniform WaveCharachteristics waves;


            
void main(){
  
      vec2 index = TexCoord_FS_in.st;
      
	     
	    
	    FragColor = vec4(Normal_FS_in,1.0); 
}


