#ifndef DANCE_FLOOR_H_
#define DANCE_FLOOR_H_

#include <mingfx.h>

#include <vector>

class DanceFloor {
public:
    DanceFloor(float s, int c);

    void Draw(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix, const Matrix4 &projMatrix);

    Color tile_color(int index);

private:

    float tileSize_;
    int tileCount_;

    std::vector< std::vector<Color> > colors_;

    Texture2D tex_;
    Mesh floorMesh_;
};


#endif
