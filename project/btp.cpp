//==============================================================================
/*
    Software License Agreement (BSD License)
    Copyright (c) 2003-2016, CHAI3D.
    (www.chai3d.org)

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

    * Neither the name of CHAI3D nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    \author    <http://www.chai3d.org>
    \author    Francois Conti
    \version   3.1.1 $Rev: 1925 $
*/
//==============================================================================

//------------------------------------------------------------------------------
#include "chai3d.h"
#include "winged-edge.h"
// #include "files/CMeshLoader.h"

//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------
#ifndef MACOSX
#include "GL/glut.h"
#else
#include "GLUT/glut.h"
#endif
//------------------------------------------------------------------------------
// file & directory reading
#include <dirent.h>
#include <stdio.h>
#include <string>
#include <iostream>
//------------------------------------------------------------------------------
// DECLARED VARIABLES
//------------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display
cCamera* camera;

// a camera attached to the endocope object
cCamera* cameraScope;

// a light source to illuminate the objects in the world
cDirectionalLight *light;

// a virtual object
cMultiMesh* skull;

// a virtual object
cMultiMesh* scope;

// a haptic device handler
cHapticDeviceHandler* handler;

// a pointer to the current haptic device
cGenericHapticDevicePtr hapticDevice;

// a virtual tool representing the haptic device in the scene
cToolCursor* tool;

// a label to display the rate [Hz] at which the simulation is running
cLabel* labelHapticRate;
vector<string> resource_3ds;

// indicates if the haptic simulation currently running
bool simulationRunning = false;

// indicates if the haptic simulation has terminated
bool simulationFinished = true;

// frequency counter to measure the simulation haptic rate
cFrequencyCounter frequencyCounter;

// information about computer screen and GLUT display 0
int windowW0;
int windowH0;
int windowPosX0;
int windowPosY0;

// information about computer screen and GLUT display 1
int windowW1;
int windowH1;
int windowPosX1;
int windowPosY1;

// Handles to both GLUT windows
int windowHandle0;
int windowHandle1;

// information about computer screen and GLUT display window
int screenW;
int screenH;
int windowW;
int windowH;
int windowPosX;
int windowPosY;

// root resource path
string resourceRoot;

// callback to handle mouse click
void mouseClick(int button, int state, int x, int y);

// callback to handle mouse motion when button is pressed
void mouseMove(int x, int y);

// some objects.
cShapeSphere* sphere;

// a small sphere which displays the position of a click hit in the world
cShapeSphere* sphereSelect;

// a small line to display the surface normal at the selection point
cShapeLine* normalSelect;

// a pointer to the selected object
cGenericObject* selectedObject = NULL;

// selected object
cMesh* mySelectedObject= NULL;

// offset between the position of the mmouse click on the object and the object reference frame location.
cVector3d selectedObjectOffset;

// flag that says if theselected object is being moved by the mous
bool flagMoveObject = false;

// position of mouse click.
cVector3d selectedPoint;

// last mouse position
// int mouseX;
// int mouseY;


//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------

// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())


//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// callback when the window 0 display is resized
void resizeWindow0(int w, int h);

// callback when the window 1 display is resized
void resizeWindow1(int w, int h);

// callback when a keyboard key is pressed
void keySelect(unsigned char key, int x, int y);

// function called before exiting the application
void close(void);

// main graphics timer and callback
void graphicsTimer0(int data);

// main graphics timer and callback
void graphicsTimer1(int data);

// graphics callback for window 0
void updateGraphics0(void);

// graphics callback for window 1
void updateGraphics1(void);

// main haptics loop
void updateHaptics(void);

// add Texture to cMesh object
void addTexture(cMesh * obj , string filepath);

void removeTexture(cMesh * obj);

//==============================================================================
/*
    DEMO:   18-endoscope.cpp

    This example demonstrates the use of dual display contexts. In the first
    window we illustrate a tool exploring a virtual skull. A second camera
    is attached at the extremity of the tools and the image rendered in a
    second viewport.
*/
//==============================================================================

int main(int argc, char* argv[])
{
    //--------------------------------------------------------------------------
    // INITIALIZATION
    //--------------------------------------------------------------------------

    cout << endl;
    cout << "-----------------------------------" << endl;
    cout << "CHAI3D" << endl;
    cout << "Demo: 18-endoscope" << endl;
    cout << "Copyright 2003-2016" << endl;
    cout << "-----------------------------------" << endl << endl << endl;
    cout << "Keyboard Options:" << endl << endl;
    cout << "[x] - Exit application" << endl;
    cout << endl << endl;

    // parse first arg to try and locate resources
    resourceRoot = string(argv[0]).substr(0,string(argv[0]).find_last_of("/\\")+1);


    //--------------------------------------------------------------------------
    // OPEN GL - WINDOW DISPLAY
    //--------------------------------------------------------------------------

    // initialize GLUT
    glutInit(&argc, argv);

    // retrieve the resolution of the computer display and estimate the position
    // of the GLUT window so that it is located at the center of the screen
    int screenW = glutGet(GLUT_SCREEN_WIDTH);
    int screenH = glutGet(GLUT_SCREEN_HEIGHT);
    windowW0 = 0.5 * screenH;
    windowH0 = 0.5 * screenH;
    windowW1 = 0.5 * screenH;
    windowH1 = 0.5 * screenH;
    windowPosX0 = (screenW - 2 * windowW0) / 2;
    windowPosY0 = (screenH - windowH0) / 2;
    windowPosX1 = screenW / 2;
    windowPosY1 = (screenH - windowH1) / 2;

    // initialize the first GLUT window
    glutInitWindowPosition(windowPosX0, windowPosY0);
    glutInitWindowSize(windowW0, windowH0);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    windowHandle0 = glutCreateWindow(argv[0]);
    glutDisplayFunc(updateGraphics0);
    glutKeyboardFunc(keySelect);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMove);
    glutReshapeFunc(resizeWindow0);
    glutSetWindowTitle("CHAI3D");
    glutTimerFunc(50, graphicsTimer0, 0);

    // initialize the second GLUT window
    glutInitWindowPosition(windowPosX1, windowPosY1);
    glutInitWindowSize(windowW1, windowH1);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    windowHandle1 = glutCreateWindow(argv[0]);
    glutDisplayFunc(updateGraphics1);
    glutKeyboardFunc(keySelect);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMove);
    glutReshapeFunc(resizeWindow1);
    glutSetWindowTitle("CHAI3D");
    glutTimerFunc(50, graphicsTimer1, 0);

#ifdef GLEW_VERSION
    // initialize GLEW
    glewInit();
#endif


    //--------------------------------------------------------------------------
    // WORLD - CAMERA - LIGHTING
    //--------------------------------------------------------------------------

    // create a new world.
    world = new cWorld();

    // set the background color of the environment
    world->m_backgroundColor.setWhite();

    // create a camera and insert it into the virtual world
    camera = new cCamera(world);
    world->addChild(camera);

    // position and orient the camera
    camera->set(cVector3d(10.0, 10.0, 10.0),    // camera position (eye)
                cVector3d(0.0, 0.0, 0.0),    // lookat position (target)
                cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector

    // set the near and far clipping planes of the camera
    // anything in front or behind these clipping planes will not be rendered
    camera->setClippingPlanes(0.01, 100);

    // create a light source
    light = new cDirectionalLight(world);

    // add light to world
    world->addChild(light);

    // enable light source
    light->setEnabled(true);

    // define the direction of the light beam
    light->setDir(-1.0,-1.0, -1.0);

    // set lighting conditions
    light->m_ambient.set(0.4f, 0.4f, 0.4f);
    light->m_diffuse.set(0.8f, 0.8f, 0.8f);
    light->m_specular.set(1.0f, 1.0f, 1.0f);


    //--------------------------------------------------------------------------
    // HAPTIC DEVICES / TOOLS
    //--------------------------------------------------------------------------

    // create a haptic device handler
    handler = new cHapticDeviceHandler();

    // get access to the first available haptic device found
    handler->getDevice(hapticDevice, 0);

    // retrieve information about the current haptic device
    cHapticDeviceInfo hapticDeviceInfo = hapticDevice->getSpecifications();

    // create a tool (cursor) and insert into the world
    tool = new cToolCursor(world);
    world->addChild(tool);

    // connect the haptic device to the virtual tool
    tool->setHapticDevice(hapticDevice);

    // define the radius of the tool (sphere)
    double toolRadius = 0.01;

    // define a radius for the tool
    tool->setRadius(toolRadius);

    // hide the device sphere. only show proxy.
    tool->setShowContactPoints(true, false);

    // create a white cursor
    tool->m_hapticPoint->m_sphereProxy->m_material->setWhite();

    // map the physical workspace of the haptic device to a larger virtual workspace.
    tool->setWorkspaceRadius(1.0);

    // haptic forces are enabled only if small forces are first sent to the device;
    // this mode avoids the force spike that occurs when the application starts when
    // the tool is located inside an object for instance.
    tool->setWaitForSmallForce(true);

    tool->enableDynamicObjects(true);

    // start the haptic tool
    tool->start();


    //--------------------------------------------------------------------------
    // CREATE OBJECTS
    //--------------------------------------------------------------------------

    // read the scale factor between the physical workspace of the haptic
    // device and the virtual workspace defined for the tool
    double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

    // properties
    double maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;


    /////////////////////////////////////////////////////////////////////////
    // OBJECT "SKULL"
    /////////////////////////////////////////////////////////////////////////

    // create a virtual mesh
    skull = new cMultiMesh();

    // add object to world
    world->addChild(skull);


    // reading directory for resources
    DIR *d;
    struct dirent *dir;
    d = opendir("./resources/subcorticals");
    string filename ;
    if (d)
    {
      while ((dir = readdir(d)) != NULL)
      {
        // filter files with .3ds extension

        filename = string(dir->d_name) ;
        // cout <<  filename << endl;

        if(filename != "." && filename!= ".." && filename.substr(filename.length()-3,filename.length())== "obj"){
          resource_3ds.push_back(filename);
          // cout <<  filename << endl;
        }
      }
      closedir(d);
    }
    // loading  object files (.3ds)
    bool fileload;
    for (int i = 0 ;i < resource_3ds.size();i++){
      cMultiMesh* mesh1 = new cMultiMesh();


      string file_location = "../../project/resources/subcorticals/" + resource_3ds[i];
      // fileload = cLoadMeshFromFile(skull , RESOURCE_PATH("../../btp/resources/skull/Skull.obj"));
      fileload = mesh1->loadFromFile(RESOURCE_PATH(file_location));
      // skull->addChild(mesh1);

      cMesh * newMesh = skull->newMesh() ;
      mesh1->convertToSingleMesh(newMesh);
      // cout << file_location<< endl;
      // if (!fileload)
      // {
      //     #if defined(_MSVC)
      //     // fileload = cLoadMeshFromFile(skull,"../../../btp/resources/skull/Skull.obj");
      //     fileload = skull->loadFromFile("../../../btp/resources/subcorticals/subcorticals.3ds");
      //     #endif
      // }
      if (!fileload)
      {
          cout << "Error - 3D Model failed to load correctly." << endl;
          close();
          return (-1);
      }
    }


    // disable culling so that faces are rendered on both sides
    skull->setUseCulling(true);

    // scale model
    skull->scale(0.06);

    // compute collision detection algorithm
    skull->createAABBCollisionDetector(toolRadius);

    // define a default stiffness for the object
    skull->setStiffness(0.1 * maxStiffness, true);

    // use display list for faster rendering
    skull->setUseDisplayList(true);

    // position and orient object in scene
    skull->setLocalPos(0.0, -0.5, 0.9);
    skull->rotateExtrinsicEulerAnglesDeg(90, 0, 0, C_EULER_ORDER_XYZ);

    // set haptic properties
    skull->m_material->setStiffness(0.5 * maxStiffness);

    // build collision detection tree
    skull->createAABBCollisionDetector(toolRadius);

    cMaterial mat;
    mat.setHapticTriangleSides(true, true);
    skull->setMaterial(mat);


    /////////////////////////////////////////////////////////////////////////
    // OBJECT "SCOPE"
    /////////////////////////////////////////////////////////////////////////

    // create a virtual mesh
    scope = new cMultiMesh();

    // attach scope to tool
    tool->m_image = scope;

    // load an object file
    fileload = scope->loadFromFile(RESOURCE_PATH("../resources/models/endoscope/endoscope.3ds"));

    if (!fileload)
    {
#if defined(_MSVC)
        fileload = scope->loadFromFile("../../../bin/resources/models/endoscope/endoscope.3ds");
#endif
    }
    if (!fileload)
    {
        cout << "Error - 3D Model failed to load correctly." << endl;
        close();
        return (-1);
    }

    // disable culling so that faces are rendered on both sides
    scope->setUseCulling(false);

    // scale model
    scope->scale(0.1);

    // use display list for faster rendering
    scope->setUseDisplayList(true);

    // position object in scene
    scope->rotateExtrinsicEulerAnglesDeg(0, 0, 0, C_EULER_ORDER_XYZ);


    /////////////////////////////////////////////////////////////////////////
    // CAMERA "SCOPE"
    /////////////////////////////////////////////////////////////////////////

    // create a camera and insert it into the virtual world
    cameraScope = new cCamera(world);
    scope->addChild(cameraScope);

    // position and orient the camera
    cameraScope->setLocalPos(-0.03, 0.0, 0.1);

    // set the near and far clipping planes of the camera
    // anything in front or behind these clipping planes will not be rendered
    cameraScope->setClippingPlanes(0.01, 100);



    //--------------------------------------------------------------------------
    // WIDGETS
    //--------------------------------------------------------------------------

    // create a font
    cFont *font = NEW_CFONTCALIBRI20();

    // create a label to display the haptic rate of the simulation
    labelHapticRate = new cLabel(font);
    labelHapticRate->m_fontColor.setBlack();
    camera->m_frontLayer->addChild(labelHapticRate);

    // create a background
    cBackground* background = new cBackground();
    camera->m_backLayer->addChild(background);

    // set background properties
    background->setCornerColors(cColorf(1.0f, 1.0f, 1.0f),
                                cColorf(1.0f, 1.0f, 1.0f),
                                cColorf(0.9f, 0.9f, 0.9f),
                                cColorf(0.9f, 0.9f, 0.9f));

    // create a frontground for the encoscope
    cBackground* frontground = new cBackground();
    cameraScope->m_frontLayer->addChild(frontground);

    // load an texture map
    fileload = frontground->loadFromFile(RESOURCE_PATH("../resources/images/scope.png"));
    if (!fileload)
    {
#if defined(_MSVC)
        fileload = frontground->loadFromFile("../../../bin/resources/images/scope.png");
#endif
    }
    if (!fileload)
    {
        cout << "Error - Image failed to load correctly." << endl;
        close();
        return (-1);
    }



    // create a small sphere to display a selection hit with the mouse
    sphereSelect = new cShapeSphere(0.005);
    world->addChild(sphereSelect);
    sphereSelect->m_material->setRedCrimson();
    sphereSelect->setShowEnabled(false);
    sphereSelect->setGhostEnabled(true);

    normalSelect = new cShapeLine();
    sphereSelect->addChild(normalSelect);
    normalSelect->m_colorPointA.setRedCrimson();
    normalSelect->m_colorPointB.setRedCrimson();
    normalSelect->setShowEnabled(false);
    normalSelect->setGhostEnabled(true);

    // verify loaded models

    cout << skull->getNumMeshes()<< endl ;
    // verify winged
    w_mesh * wmesh = new w_mesh();
    bool wload =  wingedFromMesh(wmesh,skull) ;
    if(wload) cout << "loaded " << endl;
    cout << wmesh->edges.size() << endl;


    //--------------------------------------------------------------------------
    // START SIMULATION
    //--------------------------------------------------------------------------

    // create a thread which starts the main haptics rendering loop
    cThread* hapticsThread = new cThread();
    hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

    // setup callback when application exits
    atexit(close);

    // start the main graphics rendering loop
    glutMainLoop();



    // exit
    return (0);
}


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void addTexture (cMesh * obj , string filepath){
  obj->m_texture = cTexture2d::create();
  bool fileload = obj->m_texture->loadFromFile(RESOURCE_PATH(filepath));
  // if (!fileload)
  // {
  //         #if defined(_MSVC)
  //         fileload = object1->m_texture->loadFromFile("../../../bin/resources/images/whitefoam.jpg");
  //         #endif
  // }
  if (!fileload)
  {
          cout << "Error - Texture image failed to load correctly." << endl;
          close();
          return;
  }

  // enable texture mapping
  obj->setUseTexture(true);
  obj->m_material->setWhite();

  // create normal map from textur-e data
  cNormalMapPtr normalMap1 = cNormalMap::create();
  normalMap1->createMap(obj->m_texture);
  obj->m_normalMap = normalMap1;
  //
  // // set haptic properties
  // obj->m_material->setStiffness(0.1 * maxStiffness);
  // obj->m_material->setStaticFriction(0.0);
  // obj->m_material->setDynamicFriction(0.3);
  // obj->m_material->setTextureLevel(1.5);
  // obj->m_material->setHapticTriangleSides(true, false);


}

// ---- remove texture of cMesh object
void removeTexture(){
    for (int i = 0 ; i < skull->getNumMeshes(); i++){
      skull->getMesh(i)->setUseTexture(false);
    }
}

//------------------------------------------------------------------------------

void mouseClick(int button, int state, int x, int y)
{
    // mouse button down
    if (state == GLUT_DOWN)
    {
        cCollisionRecorder recorder;
        cCollisionSettings settings;

        flagMoveObject = false;

        bool hit = camera->selectWorld(x, (windowH1 - y), windowW1, windowH1, recorder, settings);
        if (hit)
        {
            removeTexture();
            sphereSelect->setShowEnabled(true);
            normalSelect->setShowEnabled(true);
            selectedPoint = recorder.m_nearestCollision.m_globalPos;
            sphereSelect->setLocalPos(selectedPoint);
            normalSelect->m_pointA.zero();
            normalSelect->m_pointB = 0.1 * recorder.m_nearestCollision.m_globalNormal;
            selectedObject = recorder.m_nearestCollision.m_object;
            cout << selectedObject << endl;
            cMesh* object = (cMesh *) selectedObject;
            // mySelectedObject = object;
            cout << object->getNumVertices() <<endl;
            addTexture(object ,"../../project/resources/textures/sand.jpg");
            for (int i = 0 ; i < skull->getNumMeshes() ; i++){
              // if(skull->m_meshes[i] == object){
                int ver = skull->getMesh(i)->getNumVertices();
                if (ver == object->getNumVertices()){
                  cout << resource_3ds[i] << endl;
                  labelHapticRate->setText(resource_3ds[i]);
                }
                //  cout << (skull->getMesh(i))->getNumVertices() <<endl;
              // }
            }
            selectedObjectOffset = recorder.m_nearestCollision.m_globalPos - selectedObject->getLocalPos();
            flagMoveObject = true;

        }
    }
}



//------------------------------------------------------------------------------

// void mouseClick(int button, int state, int x, int y)
// {
//     mouseX = x;
//     mouseY = y;
//
//     if (state == GLUT_DOWN)
//     {
//         cCollisionRecorder recorder;
//         cCollisionSettings settings;
//
//         bool hit = camera->selectWorld(x, (windowH1 - y), windowW1, windowH1, recorder, settings);
//         if (hit)
//         {
//             // check if hit involves voxl object
//             if (recorder.m_nearestCollision.m_object == skull)
//             {
//                 // get selected voxel
//                 int voxelX = recorder.m_nearestCollision.m_voxelIndexX;
//                 int voxelY = recorder.m_nearestCollision.m_voxelIndexY;
//                 int voxelZ = recorder.m_nearestCollision.m_voxelIndexZ;
//
//                 // set color to black
//                 cColorb color(0x00, 0x00, 0x00, 0x00);
//
//                 // set color to voxel
//                 skull->m_texture->m_image->setVoxelColor(voxelX, voxelY, voxelZ, color);
//
//                 // update voxel data
//                 skull->m_texture->markForUpdate();
//             }
//         }
//     }
// }



//------------------------------------------------------------------------------

void mouseMove(int x, int y)
{
    // here we move the selected object according to the position of the mouse.
    if ((selectedObject != NULL) && (flagMoveObject))
    {
        // get the vector that goes from the camera to the selected point (mouse click)
        cVector3d vCameraObject = selectedPoint - camera->getLocalPos();

        // get the vector that point in the direction of the camera. ("where the camera is lookint at")
        cVector3d vCameraLookAt = camera->getLookVector();

        // compute the angle between both vectors
        double angle = cAngle(vCameraObject, vCameraLookAt);

        // compute the distance between the camera and the plane that intersects the object and
        // which is parallel to the camera plane
        double distanceToObjectPlane = vCameraObject.length() * cos(angle);

        // convert the pixel in mouse space into a relative position in the world
        double factor = (distanceToObjectPlane * tan(0.5 * camera->getFieldViewAngleRad())) / (0.5 * windowH1);
        double posRelX = factor * (x - (0.5 * windowW1));
        double posRelY = factor * ((windowH1 - y) - (0.5 * windowH1));

        // compute the new position in world coordinates
        cVector3d pos = camera->getLocalPos() +
                        distanceToObjectPlane * camera->getLookVector() +
                        posRelX * camera->getRightVector() +
                        posRelY * camera->getUpVector();

        // compute position of object by taking in account offset
        cVector3d posObject = pos - selectedObjectOffset;

        // apply new position to object
        selectedObject->setLocalPos(posObject);

        // place cursor at the position of the mouse click
        sphereSelect->setLocalPos(pos);
    }
}



//------------------------------------------------------------------------------

void resizeWindow0(int w, int h)
{
    windowW0 = w;
    windowH0 = h;
}

//------------------------------------------------------------------------------

void resizeWindow1(int w, int h)
{
    windowW1 = w;
    windowH1 = h;
}

//------------------------------------------------------------------------------

void keySelect(unsigned char key, int x, int y)
{
    // option ESC: exit
    if ((key == 27) || (key == 'x'))
    {
        // exit application
        exit(0);
    }
}

//------------------------------------------------------------------------------

void close(void)
{
    // stop the simulation
    simulationRunning = false;

    // wait for graphics and haptics loops to terminate
    while (!simulationFinished) { cSleepMs(100); }

    // close haptic device
    tool->stop();
}

//------------------------------------------------------------------------------

void graphicsTimer0(int data)
{
    if (simulationRunning)
    {
        glutPostRedisplay();
    }

    glutSetWindow(windowHandle1);
    glutTimerFunc(50, graphicsTimer1, 0);
}

//------------------------------------------------------------------------------

void graphicsTimer1(int data)
{
    if (simulationRunning)
    {
        glutPostRedisplay();
    }

    glutSetWindow(windowHandle0);
    glutTimerFunc(50, graphicsTimer0, 0);
}

//------------------------------------------------------------------------------

void updateGraphics0(void)
{
    /////////////////////////////////////////////////////////////////////
    // UPDATE WIDGETS
    /////////////////////////////////////////////////////////////////////

    // update haptic rate data
    //labelHapticRate->setText(cStr(frequencyCounter.getFrequency(), 0) + " Hz");


    // update position of label
    labelHapticRate->setLocalPos((int)(0.5 * (windowW0 - labelHapticRate->getWidth())), 15);


    /////////////////////////////////////////////////////////////////////
    // RENDER SCENE
    /////////////////////////////////////////////////////////////////////

    // update shadow maps (if any)
    world->updateShadowMaps(false, false, 0);

    // render world
    camera->renderView(windowW0, windowH0, 0);

    // swap buffers
    glutSwapBuffers();

    // wait until all GL commands are completed
    glFinish();

    // check for any OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) cout << "Error: " << gluErrorString(err) << endl;
}

//------------------------------------------------------------------------------

void updateGraphics1(void)
{
    /////////////////////////////////////////////////////////////////////
    // RENDER SCENE
    /////////////////////////////////////////////////////////////////////

    // update shadow maps (if any)
    world->updateShadowMaps(false, false, 1);

    // render world
    cameraScope->renderView(windowW1, windowH1, 1);

    // swap buffers
    glutSwapBuffers();

    // wait until all GL commands are completed
    glFinish();

    // check for any OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) cout << "Error: " << gluErrorString(err) << endl;
}

//------------------------------------------------------------------------------

void updateHaptics(void)
{
    // simulation in now running
    simulationRunning  = true;
    simulationFinished = false;

    // main haptic simulation loop
    while(simulationRunning)
    {
        /////////////////////////////////////////////////////////////////////////
        // HAPTIC RENDERING
        /////////////////////////////////////////////////////////////////////////

        // update frequency counter
        frequencyCounter.signal(1);

        // compute global reference frames for each object
        world->computeGlobalPositions(true);

        // update position and orientation of tool
        tool->updateFromDevice();

        // compute interaction forces
        tool->computeInteractionForces();

        // send forces to haptic device
        tool->applyToDevice();
    }

    // exit haptics thread
    simulationFinished = true;
}

//------------------------------------------------------------------------------
