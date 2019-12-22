/** CSci-4611 Assignment 5:  Art Render
 */

#include "artrender_app.h"
#include "config.h"

#include <iostream>
#include <sstream>



using namespace std;

ArtRenderApp::ArtRenderApp() : GraphicsApp(1024,768, "Art Render"),
    shader_style_(0), current_model_(0), light_pos_(3,3,3),
    diffuse_ramp_(GL_CLAMP_TO_EDGE), specular_ramp_(GL_CLAMP_TO_EDGE)
{
    // Define a search path for finding data files (images and shaders)
    search_path_.push_back(".");
    search_path_.push_back("./data");
    search_path_.push_back("./shaders");
    search_path_.push_back(DATA_DIR_INSTALL);
    search_path_.push_back(DATA_DIR_BUILD);
    search_path_.push_back(SHADERS_DIR_INSTALL);
    search_path_.push_back(SHADERS_DIR_BUILD);

    // NOTE: YOU CAN COMMENT OUT SOME OF THESE IF THE APP IS LOADING TOO SLOWLY
    // THE MODEL_FILES ARRAY JUST NEEDS TO HOLD AT LEAST ONE MODEL.
    model_files_.push_back("bunny.obj");
    model_files_.push_back("chamferedCube.obj");
    model_files_.push_back("cow.obj");
    model_files_.push_back("hippo.obj");
    model_files_.push_back("maxplanck.obj");
    model_files_.push_back("sphere.obj");
    model_files_.push_back("teapot.obj");
}


ArtRenderApp::~ArtRenderApp() {
}


void ArtRenderApp::InitNanoGUI() {
    // Setup the GUI window
    nanogui::Window *window = new nanogui::Window(screen(), "Shading Style");
    window->setPosition(Eigen::Vector2i(10, 10));
    window->setSize(Eigen::Vector2i(200,100));
    window->setLayout(new nanogui::GroupLayout());

    new nanogui::Label(window, "Rendering Style", "sans-bold");

	nanogui::Button* btn1 = new nanogui::Button(window, "Gouraud Shading");
	btn1->setCallback(std::bind(&ArtRenderApp::OnGouraudBtnPressed, this));

    nanogui::Button* btn2 = new nanogui::Button(window, "Phong Shading");
    btn2->setCallback(std::bind(&ArtRenderApp::OnPhongBtnPressed, this));

    nanogui::Button* btn3 = new nanogui::Button(window, "Artsy Shading");
    btn3->setCallback(std::bind(&ArtRenderApp::OnArtsyBtnPressed, this));




    new nanogui::Label(window, "Model", "sans-bold");

    for (int i=0; i<model_files_.size(); i++) {
        nanogui::Button* btn = new nanogui::Button(window, model_files_[i]);
        btn->setCallback( [this,i]{ this->current_model_ = i; } );
        Mesh m;
        m.LoadFromOBJ(Platform::FindFile(model_files_[i], search_path_));
        meshes_.push_back(m);
        EdgeMesh em;
        em.CreateFromMesh(m);
        edge_meshes_.push_back(em);
    }
    screen()->performLayout();
}


void ArtRenderApp::OnLeftMouseDown(const Point2 &pos) {
    Point2 normalizedMousePos = PixelsToNormalizedDeviceCoords(pos);
    float mouseZ = ReadZValueAtPixel(pos);
    uni_cam_.OnButtonDown(normalizedMousePos, mouseZ);
}


void ArtRenderApp::OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta) {
    Point2 normalizedMousePos = PixelsToNormalizedDeviceCoords(pos);
    uni_cam_.OnDrag(normalizedMousePos);
}


void ArtRenderApp::OnLeftMouseUp(const Point2 &pos) {
    Point2 normalizedMousePos = PixelsToNormalizedDeviceCoords(pos);
    uni_cam_.OnButtonUp(normalizedMousePos);
}


void ArtRenderApp::UpdateSimulation(double dt)  {
    uni_cam_.AdvanceAnimation(dt);
}


void ArtRenderApp::OnPhongBtnPressed() {
    shader_style_ = 0;
}


void ArtRenderApp::OnArtsyBtnPressed() {
    shader_style_ = 1;
}

void ArtRenderApp::OnGouraudBtnPressed() {
    shader_style_ = 2;
}


void ArtRenderApp::InitOpenGL() {
    // Set up the camera in a good position to see the entire field
    proj_matrix_ = Matrix4::Perspective(30, aspect_ratio(), 1, 20);
    uni_cam_.set_view_matrix(Matrix4::LookAt(Point3(0,0,3), Point3(0,0,0), Vector3(0,1,0)));
    glClearColor(0.7, 0.7, 0.7, 1);


    phong_shaderprog_.AddVertexShaderFromFile(Platform::FindFile("phong.vert", search_path_));
    phong_shaderprog_.AddFragmentShaderFromFile(Platform::FindFile("phong.frag", search_path_));
    phong_shaderprog_.LinkProgram();

    artsy_shaderprog_.AddVertexShaderFromFile(Platform::FindFile("artsy.vert", search_path_));
    artsy_shaderprog_.AddFragmentShaderFromFile(Platform::FindFile("artsy.frag", search_path_));
    artsy_shaderprog_.LinkProgram();

    outline_shaderprog_.AddVertexShaderFromFile(Platform::FindFile("outline.vert", search_path_));
    outline_shaderprog_.AddFragmentShaderFromFile(Platform::FindFile("outline.frag", search_path_));
    outline_shaderprog_.LinkProgram();

	gouraud_shaderprog_.AddVertexShaderFromFile(Platform::FindFile("gouraud.vert", search_path_));
    gouraud_shaderprog_.AddFragmentShaderFromFile(Platform::FindFile("gouraud.frag", search_path_));
    gouraud_shaderprog_.LinkProgram();

    // load ramps for toon shader
    diffuse_ramp_.InitFromFile(Platform::FindFile("toonDiffuse.png", search_path_));
    specular_ramp_.InitFromFile(Platform::FindFile("toonSpecular.png", search_path_));
}


void ArtRenderApp::DrawUsingOpenGL() {
    // Just the identity matrix
    Matrix4 model_matrix;

    // Lighting parameters
    static const Color Ia(0.3, 0.3, 0.3, 1);
    static const Color Id(0.7, 0.7, 0.7, 1);
    static const Color Is(1.0, 1.0, 1.0, 1);

    // Material parameters
    static const Color ka(1.0, 0.4, 0.4, 1);
    static const Color kd(1.0, 0.4, 0.4, 1);
    static const Color ks(0.6, 0.6, 0.6, 1);
    static const float s = 50;

    // Precompute items needed in the shader

    // Light positions are usually defined in world space.  For lighting calculations
    // we need the position of the light in view space (a.k.a. eye space).
    Point3 light_in_eye_space = uni_cam_.view_matrix() * light_pos_;

    // The shader also needs these matrices
    Matrix4 model_view_matrix = uni_cam_.view_matrix()*model_matrix;
    Matrix4 normal_matrix = model_view_matrix.Inverse().Transpose();

    // Make sure the default option to only draw front facing triangles is set
    glEnable(GL_CULL_FACE);

    if (shader_style_ == 0) {

        // Render the current model's mesh using the Phong shader program
        phong_shaderprog_.UseProgram();
        phong_shaderprog_.SetUniform("model_view_matrix", model_view_matrix);
        phong_shaderprog_.SetUniform("normal_matrix", normal_matrix);
        phong_shaderprog_.SetUniform("proj_matrix", proj_matrix_);
        phong_shaderprog_.SetUniform("ka", ka);
        phong_shaderprog_.SetUniform("kd", kd);
        phong_shaderprog_.SetUniform("ks", ks);
        phong_shaderprog_.SetUniform("s", s);
        phong_shaderprog_.SetUniform("light_in_eye_space", light_in_eye_space);
        phong_shaderprog_.SetUniform("Ia", Ia);
        phong_shaderprog_.SetUniform("Id", Id);
        phong_shaderprog_.SetUniform("Is", Is);
        meshes_[current_model_].Draw();
        phong_shaderprog_.StopProgram();

    }
    else if (shader_style_ == 1) {

        // Rendering using the Artsy shader programs

        // Step 1: Use the toon shader to draw the object's mesh
        artsy_shaderprog_.UseProgram();
        artsy_shaderprog_.SetUniform("model_view_matrix", model_view_matrix);
        artsy_shaderprog_.SetUniform("normal_matrix", normal_matrix);
        artsy_shaderprog_.SetUniform("proj_matrix", proj_matrix_);
        artsy_shaderprog_.SetUniform("ka", ka);
        artsy_shaderprog_.SetUniform("kd", kd);
        artsy_shaderprog_.SetUniform("ks", ks);
        artsy_shaderprog_.SetUniform("s", s);
        artsy_shaderprog_.SetUniform("light_in_eye_space", light_in_eye_space);
        artsy_shaderprog_.SetUniform("Ia", Ia);
        artsy_shaderprog_.SetUniform("Id", Id);
        artsy_shaderprog_.SetUniform("Is", Is);
        artsy_shaderprog_.BindTexture("diffuse_ramp", diffuse_ramp_);
        artsy_shaderprog_.BindTexture("specular_ramp", specular_ramp_);
        meshes_[current_model_].Draw();
        artsy_shaderprog_.StopProgram();


        // Step 2: Draw the silhouette edge using the edge mesh and outline shader

        // Disable back face culling so OpenGL will draw both front and back facing triangles
        glDisable(GL_CULL_FACE);

        // Set the OpenGL polygon offset so it will draw triangles even if they
        // exactly on top of another triangle
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1,1);

        static const float thickness = 0.01;

        // Draw edge mesh
        outline_shaderprog_.UseProgram();
        outline_shaderprog_.SetUniform("model_view_matrix", model_view_matrix);
        outline_shaderprog_.SetUniform("normal_matrix", normal_matrix);
        outline_shaderprog_.SetUniform("proj_matrix", proj_matrix_);
        outline_shaderprog_.SetUniform("thickness", thickness);
        edge_meshes_[current_model_].Draw();
        outline_shaderprog_.StopProgram();
    }
	else if (shader_style_ == 2) {

		// Render the current model's mesh using the Gouraud shader program
        gouraud_shaderprog_.UseProgram();
        gouraud_shaderprog_.SetUniform("model_view_matrix", model_view_matrix);
        gouraud_shaderprog_.SetUniform("normal_matrix", normal_matrix);
        gouraud_shaderprog_.SetUniform("proj_matrix", proj_matrix_);
        gouraud_shaderprog_.SetUniform("ka", ka);
        gouraud_shaderprog_.SetUniform("kd", kd);
        gouraud_shaderprog_.SetUniform("ks", ks);
        gouraud_shaderprog_.SetUniform("s", s);
        gouraud_shaderprog_.SetUniform("light_in_eye_space", light_in_eye_space);
        gouraud_shaderprog_.SetUniform("Ia", Ia);
        gouraud_shaderprog_.SetUniform("Id", Id);
        gouraud_shaderprog_.SetUniform("Is", Is);
        meshes_[current_model_].Draw();
        gouraud_shaderprog_.StopProgram();

	}


    // Draw the UniCam widget when in rotation mode
    uni_cam_.Draw(proj_matrix_);
}
