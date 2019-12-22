/** CSci-4611 Assignment 5:  Art Render
 */

#ifndef ART_RENDER_APP_H_
#define ART_RENDER_APP_H_

#include <mingfx.h>
using namespace mingfx;


#include "edge_mesh.h"

#include <string>
#include <vector>


/** Main application class for the ArtRender project.*/
class ArtRenderApp : public GraphicsApp {
public:
  
    ArtRenderApp();
    virtual ~ArtRenderApp();

    /// Switches to phong shading
    void OnPhongBtnPressed();
    
    /// Switches to an artsy, toon rendering style
    void OnArtsyBtnPressed();

	/// Switches to gouraud shading
    void OnGouraudBtnPressed();
	
    /// Callbacks used for UniCam camera controls
    void OnLeftMouseDown(const Point2 &pos);
    void OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta);
    void OnLeftMouseUp(const Point2 &pos);
    void UpdateSimulation(double dt);

    /// The GUI is setup here
    void InitNanoGUI();
    
    /// Models and shaders are loaded here, note these cannot be loaded within
    /// the constructor because the OpenGL context is not yet created at the
    /// time the constructor is called.
    void InitOpenGL();

    /// Rendering using shaders goes here.
    void DrawUsingOpenGL();
  
private:
  
    // 0 = phong, 1 = artsy
    int shader_style_;

    // 0 for the first model loaded, ...
    int current_model_;

    // filenames, also used as the caption for the buttons
    std::vector<std::string> model_files_;
    // triangle mesh stored for each model file
    std::vector<Mesh> meshes_;
    // the "edge mesh" computed from the triangle mesh for each file
    std::vector<EdgeMesh> edge_meshes_;

    // position for the light source used in the shaders
    Point3 light_pos_;

    
    // standard per-pixel phong shading
    ShaderProgram phong_shaderprog_;

    
    // toon shading
    ShaderProgram artsy_shaderprog_;
    // textures to use for calculating lighting cutoffs inside the toon shader
    Texture2D diffuse_ramp_;
    Texture2D specular_ramp_;
    
    // special program to use for drawing silhouette edges with the edge mesh
    ShaderProgram outline_shaderprog_;
	
	// gouraud shading
	ShaderProgram gouraud_shaderprog_;

    
    // Unicam stores/sets the view matrix based on mouse interaction
    UniCam uni_cam_;
    Matrix4 proj_matrix_;
    
    // A list of paths to search for data files (images and shaders)
    std::vector<std::string> search_path_;
};

#endif