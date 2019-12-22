/** CSci-4611 Assignment 2:  Car Soccer
 */

#ifndef CAR_SOCCER_H_
#define CAR_SOCCER_H_

#include <mingfx.h>
using namespace mingfx;

#include "ball.h"
#include "car.h"

// The main class for the Car Soccer application
class CarSoccer : public GraphicsApp {
public:
    CarSoccer();
    virtual ~CarSoccer();

    /// This is called when special keys like SPACEBAR are pressed
    void OnSpecialKeyDown(int key, int scancode, int modifiers);

    /// This is called once each frame.  dt is "delta time", the time elapsed
    /// since the last call.
    void UpdateSimulation(double dt);

    /// This is called when it is time to initialize graphics objects, like
    /// texture files.
    void InitOpenGL();

    /// This is called once each frame, and you should draw the scene inside
    /// this function.
    void DrawUsingOpenGL();

    /// This is a little utility function that is helpful.  It treats the
    /// arrow keys like a joystick and returns the direction you are pressing
    /// as a 2D vector, taking into account the fact that you might be holding
    /// down more than one key at a time.
    Vector2 joystick_direction();

private:
    // movable car object (user-input-controlled)
    Car car_;

    // ball object
    Ball ball_;

    // acceleration due to gravity
    Vector3 a_;

    // Support for drawing some simple shapes
    QuickShapes quickShapes_;

    // Images to use as textures
    Texture2D fieldTex_;
    Texture2D crowdTex_;

    // Control the computer graphics camera
    Matrix4 modelMatrix_;
    Matrix4 viewMatrix_;
    Matrix4 projMatrix_;

    // A list of paths to search for data files (images)
    std::vector<std::string> searchPath_;
};

#endif
