#ifndef AMC_UTIL_H_
#define AMC_UTIL_H_

#include <mingfx.h>


// Converts amc mocap units to meters

inline Point3 amc2meter(Point3 p) {
    return Point3(p[0] * 0.056444f, p[1] * 0.056444f, p[2] * 0.056444f);
}

template <typename T>
T amc2meter(T t) {
    return t * 0.056444f;
}


#endif
