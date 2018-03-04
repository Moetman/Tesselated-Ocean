#ifndef OCEAN_H
#define OCEAN_H

#include <osg/Camera>
#include <osg/Group>
#include <osg/PolygonMode>
#include <osg/Program>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgText/Text>
#include <osgViewer/Viewer>

#include <cassert>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <osg/BlendFunc>
#include <osg/ClipNode>
#include <osg/ClipPlane>
#include <osg/MatrixTransform>
#include <osg/PolygonMode>
#include <osg/PositionAttitudeTransform>
#include <osg/Program>
#include <osg/ShapeDrawable>
#include <osg/Uniform>
#include <osg/io_utils>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgText/Font>
#include <osgUtil/TangentSpaceGenerator>
#include <osgViewer/View>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

const int NUM_WAVES = 5;

class Ocean {
public:
  Ocean(int wave_number, int scale, int num_grids);

  ~Ocean();
  void init(osg::ref_ptr<osgViewer::Viewer> viewer);
  void createProgram();
  void setWaveAttributes(unsigned int waveNumber, osg::Vec2 direction,
                         float amplitude, float wavelength, float speed,
                         float steep);
  osg::ref_ptr<osg::Geode> getOceanGeode() { return oceanGeode; }
  osg::Texture2D *createTexture(const std::string &filename);
  void setScene(osg::Group *scene) { sceneGeode = scene; }
  osg::Camera *createRTTCamera(osg::Camera::BufferComponent buffer,
                               osg::Texture *tex, bool isAbsolute = false);
  osg::Geode *createScreenQuad(float width, float height);

public:
  int number_of_waves;
  int _scale;
  int _num_grids;
  osg::ref_ptr<osg::Uniform> directions;
  osg::ref_ptr<osg::Uniform> wavelengths;
  osg::ref_ptr<osg::Uniform> amplitudes;
  osg::ref_ptr<osg::Uniform> frequencys;
  osg::ref_ptr<osg::Uniform> speeds;
  osg::ref_ptr<osg::Uniform> steepness;
  osg::ref_ptr<osg::Uniform> eyePositionUniform;
  osg::ref_ptr<osg::Uniform> timeUniform;
  osg::ref_ptr<osg::Uniform> lightUniform;
  osg::ref_ptr<osg::Uniform> numWavesUniform;
  osg::ref_ptr<osg::Program> program;
  osg::ref_ptr<osg::Group> group;
  osg::ref_ptr<osg::Geode> oceanGeode;
  osg::ref_ptr<osg::Group> sceneGeode;
  osg::ref_ptr<osg::PositionAttitudeTransform> pat;
};

#endif
