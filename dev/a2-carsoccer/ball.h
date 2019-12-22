/** CSci-4611 Assignment 2:  Car Soccer
 */

#ifndef BALL_H_
#define BALL_H_

#include <mingfx.h>

/// Small data structure for a ball
class Ball {
public:

    /// The constructor sets the radius and calls Reset() to start the ball at
    /// the center of the field
    Ball() : radius_(2.6) {
        Reset();
    }

    /// Nothing special needed in the constructor
    virtual ~Ball() {}


    void Reset() {
        // start ball in center of pitch
        position_ = Point3(0, radius_ + 5, 0);

        // roll ball towards car side with random velocity
        // j-component is 0 to keep ball on ground at beginning
        float i = rand() % 60 + (-40);
        float k = rand() % 30 + 20;
        velocity_ = Vector3(i, 0, k);
        // velocity_ = Vector3(i, -10, k);
    }

    float radius() { return radius_; }

    Point3 position() { return position_; }
    void set_position(const Point3 &p) { position_ = p; }

    Vector3 velocity() { return velocity_; }
    void set_velocity(const Vector3 v) { velocity_ = v; }

private:
    Point3 position_;
    float radius_;
    Vector3 velocity_;
};

#endif
