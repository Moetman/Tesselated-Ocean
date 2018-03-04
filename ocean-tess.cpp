/* A demonstration of Tessellation Shaders in OpenScenegraph.
 * Original code by Philip Rideout
 * Adapted to OpenScenegraph by John Kaniarz
 */
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>

#include <osg/Program>
#include <osg/ShapeDrawable>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/StateSetManipulator>
#include <osg/BlendFunc>
#include <osgUtil/TangentSpaceGenerator>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/io_utils>
#include <osgDB/FileUtils>

#define NORMALMAP_WAVES 50
#define GEOMETRIC_WAVES 4
#define DIM 300
#define UNIT 1.f
#define TEXSIZE 256
#define GW GEOMETRIC_WAVES
#define NMW NORMALMAP_WAVES

GLuint verticesLoc = 1;
GLuint textureLoc = 0;
//#include <osgUtil/GLObjectsVisitor>


const int NUM_WAVES = 2 ;


struct WaveParameters
{
    float wavelength;
    float steepness;
    float speed;
    float kAmpOverLen;
    osg::Vec2 wave_dir;
};



WaveParameters params;

struct Wave
    {
        Wave(){}
        Wave(const WaveParameters &params){}
        WaveParameters params;
    };

Wave m_geo_waves[NUM_WAVES]; // geometric waves
Wave m_nm_waves[NUM_WAVES]; // normal map waves

WaveParameters m_params;   


/*
struct WaveCharachteristics
{
    osg::Vec3 direction;
    float wavelength;
    float amplitude;
    osg::Vec3 surfaceSize;
    float ChoppinessFactor;
    float wavelengthGain;
    float amplitudeGain;

};

*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Wave(const WaveParameters &p)
{
    float wl = p.wavelength;
    wl = rand() * (2.f * wl - 0.7f * wl) + 0.7f * wl;

    static float angle = rand() * M_PI_4;// - M_PI_2;
    float c = cos(angle);
    float s = sin(angle);
    osg::Vec2f dir(c * p.wave_dir.x(),  s * p.wave_dir.y());
    float theta =  rand() * M_PI_4; 
    
    dir = osg::Vec2(cosf(theta), sinf(theta));
    
    float st = p.steepness;

    params.wavelength = wl;
    params.steepness = st;
    params.speed = sqrt(9.81f * 2.f*M_PI/wl)*wl*p.speed;
    params.kAmpOverLen = p.kAmpOverLen;
    //params.wave_dir = osg::Vec2::randomDirection();//dir.unit();
    float randomAngle = rand() * 2 * M_PI;
    params.wave_dir = osg::Vec2(cosf(theta), sinf(theta));;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void initializeWaves()
{
    // initialize geometric waves
    for (int i = 0; i < NUM_WAVES; i++) {
        m_geo_waves[i] = m_params;
    }

    // initialize normal map waves
    for (int i = 0; i < NUM_WAVES; i++)
    {
        float wl = m_nm_waves[i].params.wavelength = (rand() * 0.5f + 0.3f);
		  //#define SHITTY_TILE
		  #ifdef SHITTY_TILE
			  // tile but shitty
			  float theta = rand() * 2 * M_PI;
			  osg::Vec2 vec(floor(cosf(theta)), floor(sinf(theta))); 
			  m_nm_waves[i].params.wave_dir = (vec*6.f)/2.f * wl; //floor
		  #else
			  // not shitty but not tiled
			  
			  float theta = rand() * 2 * M_PI;
			  osg::Vec2 vec(floor(cosf(theta)), floor(sinf(theta))); 
			  m_nm_waves[i].params.wave_dir = vec;
		  #endif
			  m_nm_waves[i].params.steepness = 5.f*(rand() * 2.f + 1.f);
			  m_nm_waves[i].params.speed = 0.05f * sqrt(M_PI/wl);
			  m_nm_waves[i].params.kAmpOverLen = 0.03f;
    }
}




float uniformRandomInRange(float min, float max) {
    assert(min < max);
    double n = (double) rand() / (double) RAND_MAX;
    double v = min + n * (max - min);
    return v;
}

class EyePositionCallback: public osg::Uniform::Callback
{
    public:
        EyePositionCallback(osg::ref_ptr<osgViewer::Viewer> viewer) :_viewer(viewer){}

        virtual void operator() ( osg::Uniform* uniform, osg::NodeVisitor* nv )
        {

       	osg::Vec3f eye,centre,up;
		//Camera position
		//osg::Matrixd mat = viewer->getCamera()->getViewMatrix();	

		//getViewMatrixAsLookAt (osg::Vec3 &eye, osg::Vec3 &center, osg::Vec3 &up, float lookDistance=1.0f)
		_viewer->getCamera()->getViewMatrixAsLookAt (eye, centre,up);

		
		//mat.getLookAt (eye, centre,up);
		
		uniform->set(eye);
		
		//float limit = 10e6f;		

		float dx = eye.x();
		float dy = eye.y();
		float dz = eye.z();

		//osg::notify(osg::NOTICE)<<"\nEyePositionUpdateCallback: eye.x :"  << eye.x() << " "<< eye.y() << " "<< eye.z() <<std::endl;  

        }

    private:
       osg::ref_ptr<osgViewer::Viewer> _viewer; 
};

class TimeCallback: public osg::Uniform::Callback
{
    public:
    
        
    
	TimeCallback() :theLastFrameNumber(-1)
	{
	
	}

    virtual void operator() ( osg::Uniform* uniform,osg::NodeVisitor* nv )
    {
		//osg::notify(osg::NOTICE)<<"\nTimeCallback: "<<std::endl;  
	
		bool frameNumberChanged = false;
		if(nv->getFrameStamp())
   		{
      		frameNumberChanged = nv->getFrameStamp()->getFrameNumber() != theLastFrameNumber;
      		theLastFrameNumber = nv->getFrameStamp()->getFrameNumber();
   		}

		uniform->set((float)nv->getFrameStamp()->getReferenceTime());
		//osg::notify(osg::NOTICE)<<"\nTime: "<<nv->getFrameStamp()->getReferenceTime()<<std::endl;
	}

    private:
		unsigned int theLastFrameNumber;
 };


osg::ref_ptr<osg::Program> createProgram(){
	osg::Program *program = new osg::Program();
	//program->addShader(new osg::Shader(osg::Shader::VERTEX,vertSource));
	//program->addShader(new osg::Shader(osg::Shader::TESSCONTROL,tessControlSource));
	//program->addShader(new osg::Shader(osg::Shader::TESSEVALUATION,tessEvalSource));
	//program->addShader(new osg::Shader(osg::Shader::GEOMETRY,geomSource));
	//program->addShader(new osg::Shader(osg::Shader::FRAGMENT,fragSource));
	//program->addShader(new osg::Shader(osg::Shader::FRAGMENT,fragSourceSIMPLE));

	program->addShader(osg::Shader::readShaderFile(osg::Shader::VERTEX, osgDB::findDataFile("data/shaders/tessellation.vert")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::TESSCONTROL, osgDB::findDataFile("data/shaders/tessellation.tesc")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::TESSEVALUATION, osgDB::findDataFile("data/shaders/tessellation.tese")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::GEOMETRY, osgDB::findDataFile("data/shaders/tessellation.geom")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("data/shaders/tessellation.frag")));





	program->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 3);
	program->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
	program->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
	program->setParameter(GL_PATCH_VERTICES,3);

	//program->setParameter( GL_GEOMETRY_VERTICES_OUT_EXT, 4 );
    //program->setParameter( GL_GEOMETRY_INPUT_TYPE_EXT, GL_QUADS );
    //program->setParameter( GL_GEOMETRY_OUTPUT_TYPE_EXT, GGL_QUADS);



	return program;
}

osg::ref_ptr<osg::Geode> generateQuad(int tilesx, int tilesy, float scale )
{
osg::notify(osg::NOTICE) <<"generateQuad "<<std::endl;
osg::notify(osg::NOTICE) <<" "<<tilesx<<" "<<tilesy<<" "<<scale<<std::endl;

osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
	geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();


	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array();
	std::vector<int> faces;
	
	for(int x=0; x<tilesx; x++)
	{
		for(int y=0; y<tilesy; y++)
		{
		float u = (float)x / (float)(tilesx);
		float v = (float)y / (float)(tilesy);

		vertices->push_back(osg::Vec3(x * scale / (tilesx-1) - scale ,   y * scale / (tilesy-1) - scale, 0.0f));
	
		normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));

		texCoords->push_back(osg::Vec2(u, v));
		// texCoords->push_back( osg::Vec2((float)(x/tilesx),(float)y/tilesy));
			
		}
	} 
	//To test against a quad
	
	int numIndices=tilesx+1;
  
	osg::DrawElementsUInt* coordIndices =   new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	
	
	for(int iy=0;iy<tilesy;++iy)// -2 removes the shelf artifact: ie vertices with no values
	{
        	for(int ix=0;ix<tilesx;++ix)
        	{
            	// four vertices per quad.
            	coordIndices->push_back(ix    +(iy+1)*numIndices);
            	coordIndices->push_back(ix    +iy*numIndices);
            	coordIndices->push_back((ix+1)+iy*numIndices);
            	coordIndices->push_back((ix+1)+(iy+1)*numIndices);
           
        	}
    	}
    	
	
	
	geometry->setVertexArray(vertices);
	geometry->setNormalArray(normals);
	geometry->setTexCoordArray(textureLoc,texCoords);


	



	int numFaces = 0;
	for(int y=0; y<tilesy-1; y++)
	{
		for(int x=0; x<tilesx-1; x++)
		{
			// 1, 0, 2
			faces.push_back(y * tilesx + (x+1));
			faces.push_back(y * tilesx + x);
			faces.push_back((y+1) * tilesx + x);

			// 3, 1, 2
			faces.push_back((y+1) * tilesx + (x+1));
			faces.push_back(y * tilesx + (x+1));
			faces.push_back((y+1) * tilesx + x);
			
			numFaces += 2;
		}
	}

	int IndexCount = faces.size();
	
	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES, IndexCount, (GLuint* )&faces[0]));
	
	
	//testing against quads
	//geometry->addPrimitiveSet(coordIndices);
	
	 
	  geometry->setVertexAttribArray(verticesLoc,vertices);
	  geometry->setVertexAttribArray(textureLoc,texCoords);
	  geometry->setVertexAttribBinding( textureLoc ,osg::Geometry::BIND_PER_VERTEX); 
	  geometry->setVertexAttribBinding( verticesLoc ,osg::Geometry::BIND_PER_VERTEX); 
	  geometry->setVertexAttribArray(2,normals);
	  geometry->setVertexAttribBinding(2,osg::Geometry::BIND_PER_VERTEX);
	  
	   osg::ref_ptr<osgUtil::TangentSpaceGenerator> tsg = new osgUtil::TangentSpaceGenerator;
			    tsg->generate(geometry, 2);
			    //geometry->setVertexAttribData(6, osg::Geometry::ArrayData(tsg->getTangentArray(), osg::Geometry::BIND_PER_VERTEX,GL_FALSE));
			    //geometry->setVertexAttribData(5, osg::Geometry::ArrayData(tsg->getBinormalArray(), osg::Geometry::BIND_PER_VERTEX,GL_FALSE));	
			    geometry->setVertexAttribArray( 5,tsg->getTangentArray());
			    geometry->setVertexAttribArray( 6,tsg->getBinormalArray());	

			   

	  
	geometry->setUseVertexBufferObjects( true );
	geometry->setUseDisplayList(false);
	geometry->setTexCoordArray(textureLoc,texCoords);

	geode->addDrawable(geometry);



	return geode;
}

int main(int argc, char* argv[])
{
	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(600,500,1024,768);
	
	osg::ref_ptr<osg::Program> program = createProgram();

	int scale = 128;
	osg::ref_ptr<osg::Group> group = new osg::Group;


	osg::Vec3 pos(0.0,0.0f,0.0f);
	osg::Vec3 width(128,0.0f,0.0f);
	osg::Vec3 height(0.0f,128,0.0f);


	group->addChild(generateQuad(200,200,10));


		
	osg::StateSet *state;
	state = group->getOrCreateStateSet();
	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.0f, 0.75f, 0.75f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.0f, 10.25f, 0.0f)));
	
	state->setAttribute(program.get());
    
	
	osg::Texture2D* terrainTexture = new osg::Texture2D;
	terrainTexture->setDataVariance(osg::Object::DYNAMIC);
        terrainTexture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
        terrainTexture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
        terrainTexture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
        terrainTexture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
	terrainTexture->setInternalFormat(GL_RGBA);

	osg::Image* image = osgDB::readImageFile("data/water2_256.bmp");


        if (!image)
        {
        std::cout << " couldn't find texture, quiting." <<  std::endl;
        exit(1);
        }
 	terrainTexture->setImage(image);
	state->setTextureAttributeAndModes(textureLoc,terrainTexture,osg::StateAttribute::ON);
	state->addUniform( new osg::Uniform("baseTexture",textureLoc) );	
	 
	
	osg::Uniform* eyePositionUniform = new osg::Uniform("gEyeWorldPos", osg::Vec3(0.0,1.0,1.0) );
	eyePositionUniform->setUpdateCallback(new EyePositionCallback(&viewer));
	state->addUniform( eyePositionUniform );
	
	
	osg::Uniform* timeUniform = new osg::Uniform("time", 0.0f ); 	
	timeUniform->setUpdateCallback(new TimeCallback());
	state->addUniform( timeUniform );
	

	osg::Uniform* waterHeightUniform = new osg::Uniform("waterHeight", 1.0f ); 	
	state->addUniform(waterHeightUniform);
	
	osg::Uniform* numWavesUniform = new osg::Uniform("numWaves", NUM_WAVES ); 	
	state->addUniform(numWavesUniform);
	
	
	
	
	//texture waves - work out how to do hi res in glsl
	
		
	
	
	
	// Set up Heightmap

        osg::Texture2D* heightTexture = new osg::Texture2D;
	heightTexture->setDataVariance(osg::Object::DYNAMIC);
        heightTexture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::NEAREST);
        heightTexture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::NEAREST);
        heightTexture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
        heightTexture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
	//heightTexture->setInternalFormat(GL_RGBA_FLOAT32_ATI); //or 
	//heightTexture->setInternalFormat(GL_LUMINANCE_FLOAT32_ATI); 	
	
	osg::Image* heightmap_image = osgDB::readImageFile("/home/paul/workspace/projects/ocean-tess/data/Height0001.jpg");
	//osg::Image* heightmap_image = osgDB::readImageFile(_scene->getConfig()->marineSettings.heightmap0);
	//osg::Image* image = osgDB::readImageFile("data/heightmap.jpg");


        if (!heightmap_image)
        {
        std::cout << " couldn't find height texture, quiting." <<  std::endl;
        exit(1);
        }

        heightTexture->setImage(heightmap_image);
	state->setTextureAttributeAndModes(0,heightTexture,osg::StateAttribute::ON);
	state->addUniform( new osg::Uniform("oceanHeightmap",0) );	

	
	
	
	
	
	// Set up Normal map


	osg::Texture2D* oceanNormalTexture = new osg::Texture2D;
	oceanNormalTexture->setDataVariance(osg::Object::DYNAMIC);
        oceanNormalTexture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
        oceanNormalTexture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
        oceanNormalTexture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
        oceanNormalTexture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
	oceanNormalTexture->setInternalFormat(GL_RGBA);

	//osg::Image* image = osgDB::readImageFile(_scene->getConfig()->marineSettings.normalmap0);

	
	osg::Image* normalimage = osgDB::readImageFile("/home/paul/workspace/projects/ocean-tess/data/Height0001-normal.jpg");


        if (!normalimage)
        {
        std::cout << " couldn't find normal texture,  quiting." <<  std::endl;
        exit(1);
        }
 	oceanNormalTexture->setImage(normalimage);
	state->setTextureAttributeAndModes(1,oceanNormalTexture,osg::StateAttribute::ON);
	state->addUniform( new osg::Uniform("oceanNormal",1) );	

	
	const char* genNormalMapFrag(
	    "uniform float time;"
            "uniform float waves[300];"
            
            "void calc_normal(in vec2 uv, out vec3 N)"
            "{"
            " float PI = 3.14159265358979323846264;"
            " N = vec3(0.0, 0.0, 1.0);"
            " for (int i = 0; i < 300; i += 6) {"
            " float A = waves[i] * waves[i+3];" // Amplitude
            " float omega = 2.0 * PI / waves[i];" // Frequency
            " float phi = waves[i+2] * omega;" // Phase
            " float k = waves[i+1];"
            " float term = omega * dot(vec2(waves[i+4], waves[i+5]), uv) + phi * time;"
            " float C = cos(term);"
            " float S = sin(term);"
            " float val = pow(0.5 * (S + 1.0), k - 1.0) * C;"
            " val = omega * A * k * val;"
            " N += vec3(waves[i+4] * val,"
            " waves[i+5] * val,"
            " 0.0);"
            " }"
            " N = normalize(N);"
            "}"

            "void main(void)"
            "{"
            " vec3 N;"
            " calc_normal(gl_FragCoord.st/128.0, N);"
            " N = (N * 0.5) + 0.5;"
            " gl_FragColor = vec4(N.xyz, 1.0);"
            "}");
	
	const char* vertOcean(
	 "void wave_function(in float waves[24], in float time, in vec3 pos,"
            " out vec3 P, out vec3 N, out vec3 B, out vec3 T)"
            "{"
            " float PI = 3.14159265358979323846264;"
            " P = pos;"
            " for (int i = 0; i < 24; i += 6) {"
            " float A = waves[i] * waves[i+3];" // Amplitude
            " float omega = 2.0 * PI / waves[i];" // Frequency
            " float phi = waves[i+2] * omega;" // Phase
            " float Qi = waves[i+1]/(omega * A * 4.0);" // Steepness

            " float term = omega * dot(vec2(waves[i+4], waves[i+5]), vec2(pos.x, pos.z)) + phi * time;"
            " float C = cos(term);"
            " float S = sin(term);"
            " P += vec3(Qi * A * waves[i+4] * C,"
            " A * S,"
            " Qi * A * waves[i+5] * C);"
            " }"
            " B = vec3(0.0);"
            " T = vec3(0.0);"
            " N = vec3(0.0);"
            " for (int i = 0; i < 24; i += 6) {"
            " float A = waves[i] * waves[i+3];" // Amplitude
            " float omega = 2.0 * PI / waves[i];" // Frequency
            " float phi = waves[i+2] * omega;" // Phase
            " float Qi = waves[i+1]/(omega * A * 4.0);" // Steepness

            " float WA = omega * A;"
            " float term = omega * dot(vec2(waves[i+4], waves[i+5]), vec2(P.x, P.z)) + phi * time;"
            " float C = cos(term)/6.0;"
            " float S = sin(term);"
            " B += vec3 (Qi * waves[i+4]*waves[i+4] * WA * S,"
            " Qi * waves[i+4] * waves[i+5] * WA * S,"
            " waves[i+4] * WA * C);"

            " T += vec3 (Qi * waves[i+4] * waves[i+5] * WA * S,"
            " Qi * waves[i+5] * waves[i+5] * WA * S,"
            " waves[i+5] * WA * C);"

            " N += vec3 (waves[i+4] * WA * C,"
            " waves[i+5] * WA * C,"
            " Qi * WA * S);"
            " }"
            " B = normalize(vec3(1.0 - B.x, -B.y, B.z));"
            " T = normalize(vec3(-T.x, 1.0 - T.y, T.z));"
            " N = normalize(vec3(-N.x, -N.y, 1.0 - N.z));"
            "}"
	
	   "uniform float waves[24];"
            "uniform float time;"
            "uniform vec3 light;"

            "varying vec2 texcoord;"
            "varying vec3 lightv;"
            "varying vec3 viewv;"
            "void main(void)"
            "{"
            " vec3 P, N, B, T;"
            " wave_function(waves, time, gl_Vertex.xyz, P, N, B, T);"
            " lightv = vec3(dot(light, B),"
            " dot(light, T),"
            " dot(light, N));"
            " lightv = normalize(lightv);"
            " vec3 pos = (gl_ModelViewMatrix * vec4(P.xyz, 1.0)).xyz;"
            " viewv = vec3(dot(pos, B),"
            " dot(pos, T),"
            " dot(pos, N));"
            " viewv = normalize(viewv);"
            " texcoord = vec2(P.x, P.z) * 0.5 + 0.5;"
            " gl_Position = gl_ProjectionMatrix * vec4(pos, 1.0);"
            "}");
	
	
	const char* fragOcean(
	   "uniform sampler2D normalmap;"

            "varying vec2 texcoord;"
            "varying vec3 lightv;"
            "varying vec3 viewv;"
            "void main(void)"
            "{"
            " vec3 N = texture2D(normalmap, texcoord*0.125).xyz * 2.0 - 1.0;"
            " N = normalize(N);"
            " vec3 specular = vec3(1.0) * pow(clamp(dot(reflect(normalize(lightv), N), viewv), 0.0, 1.0), 50.0);"
            " vec3 oceanblue = vec3(0.0, 0.0, 0.2);"
            " vec3 skyblue = vec3(0.39, 0.52, 0.93) * 0.9;"
            " const float R_0 = 0.4;"
            " float fresnel = R_0 + (1.0 - R_0) * pow((1.0 - dot(-normalize(viewv), N)), 5.0);"
            " fresnel = max(0.0, min(fresnel, 1.0));"
            " gl_FragColor = vec4(mix(oceanblue, skyblue, fresnel) + specular, 1.0);"
            "}");
	  
	  
	
	  // seed random
	  srand(time(0));

	  // initialize parameters
	  m_params.wavelength = 10.f;
	  m_params.steepness = 0.8f;
	  m_params.kAmpOverLen = 0.02f;
	  m_params.speed = 0.15f;
	  m_params.wave_dir = osg::Vec2(1.f, 0.8f);
	
	//initializeWaves();
	
	//osg::Uniform *waves = new osg::Uniform(osg::Uniform::FLOAT, "waves",  NUM_WAVES);
	//state->addUniform(waves);
	
	
	//->setUniformValueArray("waves", (const GLfloat *)m_nm_waves, NMW * sizeof(Wave)/sizeof(float), 1);
	
	
	osg::Vec3 dominantdir(0.9,0.1,0.0);
	osg::Vec3 direction(0.0,0.0,0.0);
	
	 osg::Uniform *directions = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "directions",NUM_WAVES);
	  osg::Uniform *wavelengths = new osg::Uniform(osg::Uniform::FLOAT, "wavelengths",NUM_WAVES);
	   osg::Uniform *amplitudes = new osg::Uniform(osg::Uniform::FLOAT, "amplitudes", NUM_WAVES);
	     osg::Uniform *frequencys = new osg::Uniform(osg::Uniform::FLOAT, "frequency", NUM_WAVES);
	       osg::Uniform *speeds = new osg::Uniform(osg::Uniform::FLOAT, "speeds", NUM_WAVES);
		 osg::Uniform *steepness = new osg::Uniform(osg::Uniform::FLOAT, "steepness",NUM_WAVES);
	        // initialize random seed: 
		  srand ( time(NULL) );
	       
		  
		  float spread = 360.;
		  
		  
		  for (int i = 0; i < NUM_WAVES; ++i)
		  {
		    
		      

			// generate secret number: 
			float num = (rand() % 100 + 1) * .0008101;
			float amplitude = num;
			//float amplitude = 0.0005f / (i * 0.001 + 1);

			std::cout << "Amplitudes: "<<amplitude<<std::endl;
			amplitudes->setElement(i, amplitude);
			
				
			//float wavelength =  (rand() % 10 + 1) * ((rand() % 100 + 1) * 0.1);
			 float wavelength = ( (rand() % 100 + 1 )* amplitude)  ;

			std::cout << "WavelLengths: "<<wavelength<<std::endl;
			wavelengths->setElement(i, wavelength);
			
			 float speed = 0.0005f * sqrt(M_PI/wavelength);
			//float speed = ( (rand() % 10 + 1) * 0.09);
			//float speed = 0.01f + 2*i;

			std::cout << "Speed: "<<amplitude<<std::endl;
			speeds->setElement(i, speed);
			      
			float steep = .5f*( (rand() % 100) * .001f + .1f);
			std::cout << "Steepness: "<<steep<<std::endl;
			steepness->setElement(i, steep);
			
			//osg::Vec3 direction = osg::Vec3( sqrt((rand() % 20 + 1)) * ((rand() % 10 + 1) * 0.01), sqrt((rand() % 100 + 1)) * ((rand() % 10 + 1) * 0.01)  , 0.0);
			
			//direction = dominantdir + osg::Vec3(  ( (rand() % 10 ) * 0.1),( ( (rand() % 10 ) * 0.1))   , ( ( (rand() % 10 ) * 0.1)));
			
			
			 float angle = uniformRandomInRange(-M_PI/3, M_PI/3);
			 float x = ((rand() % 500 + 1 )* 0.1) ;
			 float y = ((rand() % 500 + 1 )* 0.1) ;
			  
			  osg::Vec3 direction = osg::Vec3(sin(x), cos(y) , 0.0);
			  //direction.x() += (spread *  (rand() % 10 * 0.1) - spread * 0.5 * .100 )* M_PI/180;
			  //direction.y() += (spread *  (rand() % 10 * 0.1) - spread * 0.5 * .100 )* M_PI/180;
			  //direction.z() += ( (sqrt(2.0)) * 0.5 + 0.1)/ 360;
			std::cout << "Direction: "<<direction.x()<<" "<<direction.y()<<" "<<std::endl;
			
			
			//osg::Vec3 direction = osg::Vec3(0.36,1.0, 0.0);
			directions->setElement(i, direction);
			
			std::cout<<std::endl;
			
			
			
			
			
			
			
		  }
		  
		  
		  
		  
		  
		  
		  
		  
		  
		  

	    state->addUniform(directions); 
	   state->addUniform(wavelengths); 
	  state->addUniform(amplitudes); 
	 state->addUniform(speeds); 
	
	 
	 
	 
	 
	
	 program->addBindAttribLocation( "Tangent" , 6 );
	 program->addBindAttribLocation( "Binormal", 5 );
	
	 //WaveCharachteristics waves;
	 //osg::Vec4 waves;
	 //osg::Uniform* waveUniform  = new osg::Uniform("waveCharacteristics[%d]", &waves );
	 //state->addUniform( waveUniform );
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	// switch on the uniforms that track the modelview and projection matrices
	osgViewer::Viewer::Windows windows;
	viewer.getWindows(windows);

	for(osgViewer::Viewer::Windows::iterator itr = windows.begin();
	    itr != windows.end();
	    ++itr)
	{
		    osg::State *s=(*itr)->getState();
		    s->setUseModelViewAndProjectionUniforms(true);
		    s->setUseVertexAttributeAliasing(true);
	}
	
	

	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);	
	viewer.addEventHandler(new osgViewer::ThreadingHandler);
	viewer.addEventHandler(new osgViewer::HelpHandler);
	viewer.addEventHandler(new osgViewer::RecordCameraPathHandler);
	viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));



	viewer.setSceneData(group);
	return viewer.run();
}

