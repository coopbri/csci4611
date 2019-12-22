/** CSci-4611 Assignment 3:  Earthquake
 */

#include "earth.h"
#include "config.h"

#include <vector>

// for M_PI constant
#define _USE_MATH_DEFINES
#include <math.h>

// init geometry
const int nslices = 10;
const int nstacks = 10;

Earth::Earth() {
}

Earth::~Earth() {
}

void Earth::Init(const std::vector<std::string> &search_path) {
    // init shader program
    shader_.Init();

    // init texture: you can change to a lower-res texture here if needed
    earth_tex_.InitFromFile(Platform::FindFile("earth-2k.png", search_path));

    // latitude and longitude
    float lat;
    float lon;

    std::vector<unsigned int> indices;
    std::vector<Point2> tex;

    // loop through slices and stacks, draw vertices accordingly
    for (int stack = 0; stack <= nstacks; stack++) {
      for (int slice = 0; slice <= nslices; slice++) {
        // update latitude and longitude coordinates
        lon = slice * 360 / nslices - 180;
        lat = stack * 180 / nstacks - 90;

        float texSlice = (float)slice;
        float texStack = (float)stack;

        // add vertices based on latitude and longitude coordinates
        if (sphere) {
          vertices.push_back(LatLongToSphere(lat, lon));
        } else {
          vertices.push_back(LatLongToPlane(lat, lon));
        }

        // overlay texture map (ints converted to floats for proper representation)
        tex.push_back(Point2(texSlice / nslices, 1.0 - texStack / nstacks));
      }
    }

    // fill indices for vertex-triangle drawing, iterate over all segments
    int n = 0;
    for (int i = 0; i < (nslices * nstacks); i++) {
      indices.push_back(i + n);
      indices.push_back(i + n + 1);
      indices.push_back(i + nslices + n + 1);

      indices.push_back(i + nslices + n + 1);
      indices.push_back(i + n + 1);
      indices.push_back(i + nslices + n + 2);

      if (((i + 1) % nslices) == 0) {
        n++;
      }
    }

    // update mesh
    earth_mesh_.SetVertices(vertices);
    earth_mesh_.SetIndices(indices);
    earth_mesh_.SetTexCoords(0, tex);
    earth_mesh_.UpdateGPUMemory();
}

void Earth::Draw(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix) {
    // Define a really bright white light.  Lighting is a property of the "shader"
    DefaultShader::LightProperties light;
    light.position = Point3(10,10,10);
    light.ambient_intensity = Color(1,1,1);
    light.diffuse_intensity = Color(1,1,1);
    light.specular_intensity = Color(1,1,1);
    shader_.SetLight(0, light);

    // Adust the material properties, material is a property of the thing
    // (e.g., a mesh) that we draw with the shader.  The reflectance properties
    // affect the lighting.  The surface texture is the key for getting the
    // image of the earth to show up.
    DefaultShader::MaterialProperties mat;
    mat.ambient_reflectance = Color(0.5, 0.5, 0.5);
    mat.diffuse_reflectance = Color(0.75, 0.75, 0.75);
    mat.specular_reflectance = Color(0.75, 0.75, 0.75);
    mat.surface_texture = earth_tex_;

    // Draw the earth mesh using these settings
    if (earth_mesh_.num_triangles() > 0) {
        shader_.Draw(model_matrix, view_matrix, proj_matrix, &earth_mesh_, mat);
    }
}

void Earth::Toggle() {
  // toggle boolean
  sphere = !sphere;

  std::vector<Point3> vertices;
  std::vector<Vector3> normals;

  // render sphere
  if (sphere) {
    for (int stack = 0; stack <= nstacks; stack++) {
      for (int slice = 0; slice <= nslices; slice++) {
        float lat = stack * 180 / nstacks - 90;
        float lon = slice * 360 / nslices - 180;
        vertices.push_back(LatLongToSphere(lat, lon));
        normals.push_back(Normal(lat, lon));
      }
    }
  // render plane
  } else {
    for (int stack = 0; stack <= nstacks; stack++) {
      for (int slice = 0; slice <= nslices; slice++) {
        float lat = stack * 180 / nstacks - 90;
        float lon = slice * 360 / nslices - 180;
        vertices.push_back(LatLongToPlane(lat, lon));
      }
    }
  }

  // update vertices and GPU memory
  earth_mesh_.SetVertices(vertices);
  earth_mesh_.SetNormals(normals);
  earth_mesh_.UpdateGPUMemory();
}

Point3 Earth::LatLongToSphere(double latitude, double longitude) const {
    float polarLat = GfxMath::ToRadians(latitude);
    float polarLon = GfxMath::ToRadians(longitude);

    return Point3(cos(polarLat) * sin(polarLon), sin(polarLat),
        cos(polarLat) * cos(polarLon));
}

Point3 Earth::LatLongToPlane(double latitude, double longitude) const {
    // "x" (longitude): [-pi,pi]
    // "y" (latitude): [-pi/2,pi/2]
    // xy-plane; z is fixed
    return Point3(GfxMath::ToRadians(longitude), GfxMath::ToRadians(latitude), 0);
}

Vector3 Earth::Normal(double latitude, double longitude) const {
    float polarLat = GfxMath::ToRadians(latitude);
    float polarLon = GfxMath::ToRadians(longitude);
    return Vector3(cos(polarLat) * sin(polarLon), sin(polarLat),
        cos(polarLat) * cos(polarLon));
}

void Earth::DrawDebugInfo(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix) {
    // This draws a cylinder for each line segment on each edge of each triangle in your mesh.
    // So it will be very slow if you have a large mesh, but it's quite useful when you are
    // debugging your mesh code, especially if you start with a small mesh.
    for (int t=0; t<earth_mesh_.num_triangles(); t++) {
        std::vector<unsigned int> indices = earth_mesh_.triangle_vertices(t);
        std::vector<Point3> loop;
        loop.push_back(earth_mesh_.vertex(indices[0]));
        loop.push_back(earth_mesh_.vertex(indices[1]));
        loop.push_back(earth_mesh_.vertex(indices[2]));
        quick_shapes_.DrawLines(model_matrix, view_matrix, proj_matrix,
            Color(1,1,0), loop, QuickShapes::LinesType::LINE_LOOP, 0.005);
    }
}
