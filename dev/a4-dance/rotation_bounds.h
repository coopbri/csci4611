#ifndef ROTATION_BOUNDS_H_
#define ROTATION_BOUNDS_H_

/** This class just provides a data structure to store information about how each bone can move,
    including whehter it can rotate in the x, y, and z directions (some joints support rotation
    along all axes, others are just 1-dimensional) and the min and max angles of rotation for
    each axis */
class RotationBounds {
public:
    RotationBounds();

    void setdof(bool rx, bool ry, bool rz);

    void setR(int index, float min, float max);

    bool dofRX;
    bool dofRY;
    bool dofRZ;

    int dofs;

    float minRX;
    float maxRX;
    float minRY;
    float maxRY;
    float minRZ;
    float maxRZ;
};

#endif
