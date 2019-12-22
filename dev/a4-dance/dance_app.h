#ifndef DANCE_APP_H_
#define DANCE_APP_H_

#include <mingfx.h>

#include "animated_character.h"

#include <string>
#include <vector>


/** DanceApp extends GraphicsApp to implement the dancing ants assignment. */
class DanceApp : public GraphicsApp {
public:
    
    DanceApp();
    virtual ~DanceApp();

    /// The OnMotion*BtnPressed() callbacks are called when the user clicks the
    /// corresponding button in the GUI.
    void OnMotion1BtnPressed();
    void OnMotion2BtnPressed();
    void OnMotion3BtnPressed();
    void OnMotion4BtnPressed();
    void OnMotion5BtnPressed();

    /// You can think of playing back mocap data as a form of simulation.  This
    /// gets called once per frame and passes the elapsed time since the last
    /// frame, so this is exactly the right place to advance the mocap playback.
    void UpdateSimulation(double dt);
    
    /// Used to initialize the on-screen GUI elements
    void InitNanoGUI();

    /// Used to initialize opengl-based graphics elements
    void InitOpenGL();

    /// This is the place to draw the actual animated characters based upon their
    /// current poses.
    void DrawUsingOpenGL();
    
    
private:
    
    // The pair of ants dancing the salsa
    AnimatedCharacter salsa_ant_male_;
    AnimatedCharacter salsa_ant_female_;
    // This transform is applied to both the salsa ants, it just shifts them to
    // the right a bit so that the ballet ant can have some space on the dance
    // floor too
    Matrix4 salsa_ants_transform_;
    
    // The ballet dancing ant
    AnimatedCharacter ballet_ant_;
    // Shifts the ballet dancing ant to the left a bit
    Matrix4 ballet_ant_transform_;

    // Motion clips loaded from disk for the ballet ant -- one for the base loop
    // to use which she is resting and then 5 "special motions" that can be
    // applied with the press of a button.
    MotionClip ballet_base_loop_;
    MotionClip ballet_special1_;
    MotionClip ballet_special2_;
    MotionClip ballet_special3_;
    MotionClip ballet_special4_;
    MotionClip ballet_special5_;

    
    
    // A list of paths to search for data files (images and shaders)
    std::vector<std::string> searchPath_;

    // Define the camera
    Matrix4 viewMatrix_;
    Matrix4 projMatrix_;

    // For drawing the floor, etc.
    QuickShapes quick_shapes_;

    // Textures for the background image and floor
    Texture2D bg_tex_;
    Texture2D floor_tex_;
};

#endif
