#include "dance_floor.h"
#include "config.h"

DanceFloor::DanceFloor(float s, int c) {
  tileSize_ = s;
  tileCount_ = c;
    /**
  Array<int> indices;
  Array<Vector3> coords;
  Array<Vector2> texcoords;
  tex = Texture::fromFile(DANCEFLOOR_PATH);
  
  indices.append(0,1,2,3);
  coords.append(Vector3(-tileSize/2, 0, -tileSize/2));
  texcoords.append(Vector2(0,0));
  coords.append(Vector3(-tileSize/2, 0, tileSize/2));
  texcoords.append(Vector2(0,1));
  coords.append(Vector3(tileSize/2, 0, tileSize/2));
  texcoords.append(Vector2(1,1));
  coords.append(Vector3(tileSize/2, 0, -tileSize/2));
  texcoords.append(Vector2(1,0));
  
  vdatabuf = VertexBuffer::create(sizeof(Vector3) * coords.size()
                                  + sizeof(Vector2) * texcoords.size() + sizeof(int)*indices.size(),
                                  VertexBuffer::WRITE_ONCE);
  
  vcoords = AttributeArray(coords, vdatabuf);
  vtexcoords = AttributeArray(texcoords, vdatabuf);
  vindices = IndexStream(indices, vdatabuf);
  
  colors.resize(tileCount*2);
  for (int i=0; i<tileCount*2; i++) {
    colors[i].resize(tileCount*2);
  }
     **/
}

Color DanceFloor::tile_color(int index) {
  return colors_[0][index];
}

void DanceFloor::Draw(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix, const Matrix4 &projMatrix) {
  /*rd->pushState();
  rd->setTexture(0, tex);
  
  rd->beginIndexedPrimitives();
  rd->setVertexArray(vcoords);
  rd->setTexCoordArray(0, vtexcoords);
  
  
  for (int x=0; x<tileCount*2; x++) {
    for (int y=0; y<tileCount*2; y++) {
      colors[x][y] *= 0.99;
      rd->setColor(colors[x][y]+Color3(0.1,0.1,0.1));
      rd->setObjectToWorldMatrix(CoordinateFrame(Vector3((x - tileCount) * tileSize,
                                                         0,
                                                         (y - tileCount) * tileSize)));
      rd->sendIndices(PrimitiveType::QUADS, vindices);
      if (Random::common().uniform(0, 1) < 0.015) {
        colors[x][y] = Color3(Random::common().uniform(0.3, 2.0),
                              Random::common().uniform(0.3, 2.0),
                              Random::common().uniform(0.3, 2.0));
      }
    }
  }
  
  rd->endIndexedPrimitives();
  rd->popState();
  */
}

