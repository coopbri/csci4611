#include "dance_app.h"
#include "config.h"

#include <iostream>
#include <sstream>



DanceApp::DanceApp() : GraphicsApp(1280,768, "So You Think Ants Can Dance") {
    // Define a search path for finding data files (images and shaders)
    searchPath_.push_back(".");
    searchPath_.push_back("./data");
    searchPath_.push_back(DATA_DIR_INSTALL);
    searchPath_.push_back(DATA_DIR_BUILD);


    // INIT SALSA DANCING PAIR

    // 1. Load skeletons
    salsa_ant_male_.LoadSkeleton(Platform::FindFile("60.asf", searchPath_));
    salsa_ant_male_.set_fps(60.0);
    salsa_ant_female_.LoadSkeleton(Platform::FindFile("61.asf", searchPath_));
    salsa_ant_female_.set_fps(60.0);

    // 2. Prep the motion clips (trim, loop) the exact same way since the pair
    //    must be coordinated.
    MotionClip male_salsa_motion;
    male_salsa_motion.LoadFromAMC(Platform::FindFile("60_12.amc", searchPath_), *salsa_ant_male_.skeleton_ptr());
    male_salsa_motion.TrimFront(100);
    male_salsa_motion.TrimBack(150);
    male_salsa_motion.MakeLoop(100);

    MotionClip female_salsa_motion;
    female_salsa_motion.LoadFromAMC(Platform::FindFile("61_12.amc", searchPath_), *salsa_ant_female_.skeleton_ptr());
    female_salsa_motion.TrimFront(100);
    female_salsa_motion.TrimBack(150);
    female_salsa_motion.MakeLoop(100);

    // 3. Make the animated characters play the motion clips, the clips will automatically repeat
    salsa_ant_male_.Play(male_salsa_motion);
    salsa_ant_female_.Play(female_salsa_motion);


    // INIT BALLET DANCER

    // 1. Load skeleton
    ballet_ant_.LoadSkeleton(Platform::FindFile("61.asf", searchPath_));

    // 2. Load the raw motion clips
    ballet_base_loop_.LoadFromAMC(Platform::FindFile("05_20.amc", searchPath_), *ballet_ant_.skeleton_ptr());
    ballet_base_loop_.TrimBack(600);
    ballet_base_loop_.MakeLoop(50);
    ballet_base_loop_.CalcRelativeTranslations();

    ballet_special1_.LoadFromAMC(Platform::FindFile("05_02.amc", searchPath_), *ballet_ant_.skeleton_ptr());
    ballet_special1_.TrimFront(280);
    ballet_special1_.TrimBack(200);
    ballet_special1_.CalcRelativeTranslations();

    // TODO: Add special motions 2-5 on your own.
    // You can pick your own motions from the CMU mocap database (http://mocap.cs.cmu.edu) or
    // you can use the same dance moves that we did.  We used:
    // 05_10.amc, 05_09.amc, 05_20.amc, and 05_06.amc -- you need to trim them
    // isolate the interesting portions of the motion.
    ballet_special2_.LoadFromAMC(Platform::FindFile("05_10.amc", searchPath_), *ballet_ant_.skeleton_ptr());
    ballet_special2_.TrimFront(30);
    ballet_special2_.TrimBack(400);
    ballet_special2_.CalcRelativeTranslations();

    ballet_special3_.LoadFromAMC(Platform::FindFile("05_09.amc", searchPath_), *ballet_ant_.skeleton_ptr());
    ballet_special3_.TrimFront(470);
    ballet_special3_.TrimBack(150);
    ballet_special3_.CalcRelativeTranslations();

    ballet_special4_.LoadFromAMC(Platform::FindFile("05_20.amc", searchPath_), *ballet_ant_.skeleton_ptr());
    ballet_special4_.TrimFront(470);
    ballet_special4_.TrimBack(200);
    ballet_special4_.CalcRelativeTranslations();

    ballet_special5_.LoadFromAMC(Platform::FindFile("05_06.amc", searchPath_), *ballet_ant_.skeleton_ptr());
    ballet_special5_.TrimFront(20);
    ballet_special5_.TrimBack(180);
    ballet_special5_.CalcRelativeTranslations();

    // 3. Start the base loop motion
    ballet_ant_.Play(ballet_base_loop_);

    salsa_ants_transform_ = Matrix4::Translation(Vector3(1, 0, 0.5));
    ballet_ant_transform_ = Matrix4::Translation(Vector3(-1, ballet_base_loop_.pose(0).root_position()[1], 0));
}

DanceApp::~DanceApp() {
}


void DanceApp::InitNanoGUI() {
    // Setup the GUI window
    nanogui::Window *window = new nanogui::Window(screen(), "Ballet Controls");
    window->setPosition(Eigen::Vector2i(10, 10));
    window->setSize(Eigen::Vector2i(200,100));
    window->setLayout(new nanogui::GroupLayout());

    nanogui::Button* btn1 = new nanogui::Button(window, "Motion 1");
    btn1->setCallback(std::bind(&DanceApp::OnMotion1BtnPressed, this));

    nanogui::Button* btn2 = new nanogui::Button(window, "Motion 2");
    btn2->setCallback(std::bind(&DanceApp::OnMotion2BtnPressed, this));

    nanogui::Button* btn3 = new nanogui::Button(window, "Motion 3");
    btn3->setCallback(std::bind(&DanceApp::OnMotion3BtnPressed, this));

    nanogui::Button* btn4 = new nanogui::Button(window, "Motion 4");
    btn4->setCallback(std::bind(&DanceApp::OnMotion4BtnPressed, this));

    nanogui::Button* btn5 = new nanogui::Button(window, "Motion 5");
    btn5->setCallback(std::bind(&DanceApp::OnMotion5BtnPressed, this));

    screen()->performLayout();
}

void DanceApp::InitOpenGL() {
    // Set up the camera in a good position to see the scene
    projMatrix_ = Matrix4::Perspective(60.0, aspect_ratio(), 0.01, 100);
    viewMatrix_ = Matrix4::LookAt(Point3(0,1.5,3.5), Point3(0,1,0), Vector3(0,1,0));
    glClearColor(0.5, 0.5, 0.5, 1);

    bg_tex_.InitFromFile(Platform::FindFile("ants-dance.png", searchPath_));
    floor_tex_.InitFromFile(Platform::FindFile("woodfloor.png", searchPath_));
}


void DanceApp::OnMotion1BtnPressed() {
    ballet_ant_.OverlayClip(ballet_special1_, 100);
}

void DanceApp::OnMotion2BtnPressed() {
    ballet_ant_.OverlayClip(ballet_special2_, 100);
}

void DanceApp::OnMotion3BtnPressed() {
    ballet_ant_.OverlayClip(ballet_special3_, 100);
}

void DanceApp::OnMotion4BtnPressed() {
    ballet_ant_.OverlayClip(ballet_special4_, 100);
}

void DanceApp::OnMotion5BtnPressed() {
    ballet_ant_.OverlayClip(ballet_special5_, 100);
}

void DanceApp::UpdateSimulation(double dt)  {
    salsa_ant_male_.AdvanceAnimation(dt);
    salsa_ant_female_.AdvanceAnimation(dt);
    ballet_ant_.AdvanceAnimation(dt);
}

void DanceApp::DrawUsingOpenGL() {
    quick_shapes_.DrawFullscreenTexture(Color(1,1,1), bg_tex_);

    // Draw the dancing characters.

    // The salsa ants are drawn using absolute positioning
    salsa_ant_male_.Draw(salsa_ants_transform_, viewMatrix_, projMatrix_, true);
    salsa_ant_female_.Draw(salsa_ants_transform_, viewMatrix_, projMatrix_, true);

    // The ballet ant is drawn with relative positioning
    ballet_ant_.Draw(ballet_ant_transform_, viewMatrix_, projMatrix_, false);


    // Draw the dance floor
    Matrix4 M_floor = Matrix4::Scale(Vector3(8, 1, 3));
    quick_shapes_.DrawSquare(M_floor, viewMatrix_, projMatrix_, Color(1,1,1), floor_tex_);
}
