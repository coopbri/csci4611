/** CSci-4611 Assignment 2:  Car Soccer
 */

#include "car_soccer.h"
#include "config.h"

CarSoccer::CarSoccer() : GraphicsApp(1024,768, "Car Soccer") {
    // Define a search path for finding data files (images and shaders)
    searchPath_.push_back(".");
    searchPath_.push_back("./data");
    searchPath_.push_back(DATA_DIR_INSTALL);
    searchPath_.push_back(DATA_DIR_BUILD);

    // seed random number generator for ball velocity on launch
    srand(time(0));
}

CarSoccer::~CarSoccer() {}

Vector2 CarSoccer::joystick_direction() {
    Vector2 dir;
    if (IsKeyDown(GLFW_KEY_LEFT))
        dir[0]--;
    if (IsKeyDown(GLFW_KEY_RIGHT))
        dir[0]++;
    if (IsKeyDown(GLFW_KEY_UP))
        dir[1]++;
    if (IsKeyDown(GLFW_KEY_DOWN))
        dir[1]--;
    return dir;
}

void CarSoccer::OnSpecialKeyDown(int key, int scancode, int modifiers) {
    if (key == GLFW_KEY_SPACE) {
        // Reset ball at center of pitch
        ball_.Reset();
    }
}

void CarSoccer::UpdateSimulation(double timeStep) {
    // ball position components
    float ballX = ball_.position()[0];
    float ballY = ball_.position()[1];
    float ballZ = ball_.position()[2];

    // goal position values (x and z)
    int goalX = 10;
    int goalZ = 50;

    // radius of ball
    float radBall = ball_.radius();

    // radius of car
    float radCar = car_.collision_radius();

    // goal detection (ball in goal)
    if ((ballX >= (-goalX + radBall))
    && ((ballX <= ( goalX - radBall)))
    &&  (ballY <= ( goalX - radBall))
    && ((ballZ <= (-goalZ + radBall))
    ||  (ballZ >= ( goalZ - radBall)))) {
          // reset ball position
          ball_.Reset();

          // reset car position
          car_.Reset();
    }

    // ball-ground contact (y-value check)
    if (ball_.position()[1] < (radBall)) {
      // slow down ball after collision
      ball_.set_velocity(Vector3(
          ball_.velocity()[0] * 0.8,
          abs(ball_.velocity()[1]) * 0.8,
          ball_.velocity()[2] * 0.8)
      );

      // fix position above ground
      ball_.set_position(Point3(
          ball_.position()[0],
          radBall,
          ball_.position()[2])
      );
    }

    // ball-ceiling contact (y-value check)
    if (ball_.position()[1] > (35 - radBall)) {
        // slow down ball after collision
    		ball_.set_velocity(Vector3(
            ball_.velocity()[0] * 0.8,
            -abs(ball_.velocity()[1]) * 0.8,
            ball_.velocity()[2] * 0.8)
        );

        // fix position under ceiling
    		ball_.set_position(Point3(
            ball_.position()[0],
            35 - radBall,
            ball_.position()[2])
        );
  	}

    // ball collision with far wall
    if (ball_.position()[2] < (-50 + radBall)) {
      // slow down ball after collision
      ball_.set_velocity(Vector3(
          ball_.velocity()[0] * 0.8,
          ball_.velocity()[1],
          abs(ball_.velocity()[2]) * 0.8)
      );

      // fix position back into playing area
  		ball_.set_position(Point3(
          ball_.position()[0],
          ball_.position()[1],
          -50 + radBall)
      );
    }

    // ball collision with close wall
    if (ball_.position()[2] > (50 - radBall)) {
      // slow down ball after collision
  		ball_.set_velocity(Vector3(
          ball_.velocity()[0] * 0.8,
          ball_.velocity()[1],
          -abs(ball_.velocity()[2]) * 0.8)
      );

      // fix position back into playing area
  		ball_.set_position(Point3(
          ball_.position()[0],
          ball_.position()[1],
          50 - radBall)
      );
  	}

    // ball collision with left wall
    if (ball_.position()[0] < (-40 + radBall)) {
        // slow down ball after collision
    		ball_.set_velocity(Vector3(
            abs(ball_.velocity()[0]),
            ball_.velocity()[1],
            ball_.velocity()[2] * 0.8)
        );

        // fix position back into playing area
    		ball_.set_position(Point3(
            -40 + radBall,
            ball_.position()[1],
            ball_.position()[2])
        );
    	}

    // ball collision with right wall
    if (ball_.position()[0] > (40 - radBall)) {
      // slow down ball after collision
      ball_.set_velocity(Vector3(
          -abs(ball_.velocity()[0]),
          ball_.velocity()[1],
          ball_.velocity()[2] * 0.8)
      );

      // fix position back into playing area
      ball_.set_position(Point3(
          40 - radBall,
          ball_.position()[1],
          ball_.position()[2])
      );
    }

    // car collision with far wall
    if (car_.position()[2] < (-50 + radCar)) {
      car_.set_position(Point3(
          car_.position()[0],
          car_.position()[1],
          -50 + radCar
      ));

      // stop car motion
      car_.set_velocity(Vector3(0, 0, 0));
    }

    // car collision with close wall
    if (car_.position()[2] > (50 - radCar)) {
      car_.set_position(Point3(
          car_.position()[0],
          car_.position()[1],
          50 - radCar
      ));

      // stop car motion
      car_.set_velocity(Vector3(0, 0, 0));
    }

    // car collision with left wall
    if (car_.position()[0] < (-40 + radCar)) {
			car_.set_position(Point3(
          -40 + radCar,
          car_.position()[1],
          car_.position()[2]
      ));

      // stop car motion
      car_.set_velocity(Vector3(0, 0, 0));
		}

    // car collision with right wall
    if (car_.position()[0] > (40 - radCar)) {
			car_.set_position(Point3(
          40 - radCar,
          car_.position()[1],
          car_.position()[2]
      ));

      // stop car motion
      car_.set_velocity(Vector3(0, 0, 0));
		}

    // direction for car based on left/right arrow keys
    Vector2 dir = joystick_direction();

    // angle for car movement
    float theta = car_.angle();

    // thrust force (forward motion)
    Vector3 thrust = 250 * Vector3(-dir[1] * sin(theta), 0, -dir[1] * cos(theta));

    // drag force (backward motion)
    Vector3 drag = 5 * car_.velocity();

    // set car direction
    car_.set_angle(car_.angle() + (-dir[0] * 4 * timeStep));

    // set car velocity based on thrust and drag forces
    car_.set_velocity(car_.velocity() + (thrust - drag) * timeStep);

    // update car position based on velocity
    car_.set_position(car_.position() + car_.velocity() * timeStep);

    // acceleration faster than Earth's gravity for improved UX
    a_ = Vector3(0, -12.8 * timeStep, 0);

    // update ball velocity
    ball_.set_velocity(ball_.velocity() + a_);

    // determine vector for ball-car interaction
    Vector3 hit = ball_.position() - car_.position();
    Vector3 hitNorm = Vector3::Normalize(hit);

    // handle ball-car interaction
    if (hit.Length() < (radBall + radCar)) {
      // update ball position
      ball_.set_position(car_.position() + (radBall + radCar) * hitNorm);

      // determine velocity difference between ball and car
      Vector3 vel = ball_.velocity() - car_.velocity();

      // update ball velocity
      ball_.set_velocity(car_.velocity() + (vel - 1.8 * (vel.Dot(hitNorm) * hitNorm)));
    }

    // Update ball position based on velocity
    ball_.set_position(ball_.position() + (ball_.velocity() * timeStep));
}

void CarSoccer::InitOpenGL() {
    // Set up the camera in a good position to see the entire field
    projMatrix_ = Matrix4::Perspective(60, aspect_ratio(), 1, 1000);
    modelMatrix_ = Matrix4::LookAt(Point3(0,60,70), Point3(0,0,10), Vector3(0,1,0));

    // Set a background color for the screen
    glClearColor(0.8,0.8,0.8, 1);

    // Load some image files we'll use
    fieldTex_.InitFromFile(Platform::FindFile("pitch.png", searchPath_));
    crowdTex_.InitFromFile(Platform::FindFile("crowd.png", searchPath_));
}

void CarSoccer::DrawUsingOpenGL() {
    // Draw the crowd as a fullscreen background image
    quickShapes_.DrawFullscreenTexture(Color(1,1,1), crowdTex_);

    // Draw the field with field texture
    Color col(16.0/255.0, 46.0/255.0, 9.0/255.0);
    Matrix4 M = Matrix4::Translation(Vector3(0,-0.201,0)) * Matrix4::Scale(Vector3(50, 1, 60));
    quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, col);
    M = Matrix4::Translation(Vector3(0,-0.2,0)) * Matrix4::Scale(Vector3(40, 1, 50));
    quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, Color(1,1,1), fieldTex_);

    // Draw the car
    Color carcol(1, 0.65, 0);
    Matrix4 Mcar =
        Matrix4::Translation(car_.position() - Point3(0,0,0)) *
        Matrix4::RotationY(car_.angle()) *
        Matrix4::Scale(car_.size()) *
        Matrix4::Scale(Vector3(0.5,0.5,0.5));
    quickShapes_.DrawCube(modelMatrix_ * Mcar, viewMatrix_, projMatrix_, carcol);

    // Draw the ball
    Color ballcol(1,1,1);
    Matrix4 Mball =
        Matrix4::Translation(ball_.position() - Point3(0,0,0)) *
        Matrix4::Scale(Vector3(ball_.radius(), ball_.radius(), ball_.radius()));
    quickShapes_.DrawSphere(modelMatrix_ * Mball, viewMatrix_, projMatrix_,
        ballcol);

    // Draw the ball's shadow
    Color shadowcol(0.2,0.4,0.15);
    Matrix4 Mshadow =
        Matrix4::Translation(Vector3(ball_.position()[0], -0.1, ball_.position()[2])) *
        Matrix4::Scale(Vector3(ball_.radius(), 0, ball_.radius())) *
        Matrix4::RotationX(90);
    quickShapes_.DrawSphere(modelMatrix_ * Mshadow, viewMatrix_, projMatrix_,
        shadowcol);

    // Draw far playing field boundary
    Matrix4 MboundFar;
    Color boundCol = Color(1,1,1);
    std::vector<Point3> boundFar;
    boundFar.push_back(Point3(-40, 35, -50));
    boundFar.push_back(Point3(40, 35, -50));
    boundFar.push_back(Point3(40, 0, -50));
    boundFar.push_back(Point3(-40, 0, -50));
    quickShapes_.DrawLines(modelMatrix_ * MboundFar, viewMatrix_, projMatrix_,
        boundCol, boundFar, QuickShapes::LinesType::LINE_LOOP, 0.1);

    // Draw close playing field boundary
    Matrix4 MboundClose;
    std::vector<Point3> boundClose;
    boundClose.push_back(Point3(-40, 35, 50));
    boundClose.push_back(Point3(40, 35, 50));
    boundClose.push_back(Point3(40, 0, 50));
    boundClose.push_back(Point3(-40, 0, 50));
    quickShapes_.DrawLines(modelMatrix_ * MboundClose, viewMatrix_, projMatrix_,
        boundCol, boundClose, QuickShapes::LinesType::LINE_LOOP, 0.1);

    // Draw left playing field boundary
    Matrix4 MboundLeft;
    std::vector<Point3> boundLeft;
    boundLeft.push_back(Point3(-40, 0, -50));
    boundLeft.push_back(Point3(-40, 0, 50));
    boundLeft.push_back(Point3(-40, 35, -50));
    boundLeft.push_back(Point3(-40, 35, 50));
    quickShapes_.DrawLines(modelMatrix_ * MboundLeft, viewMatrix_, projMatrix_,
        boundCol, boundLeft, QuickShapes::LinesType::LINES, 0.1);

    // Draw right playing field boundary
    Matrix4 MboundRight;
    std::vector<Point3> boundRight;
    boundRight.push_back(Point3(40, 0, -50));
    boundRight.push_back(Point3(40, 0, 50));
    boundRight.push_back(Point3(40, 35, -50));
    boundRight.push_back(Point3(40, 35, 50));
    quickShapes_.DrawLines(modelMatrix_ * MboundRight, viewMatrix_, projMatrix_,
        boundCol, boundRight, QuickShapes::LinesType::LINES, 0.1);

    // Draw blue goal boundary
    Matrix4 Mblue;
    Color bCol(0, 1, 1);
    std::vector<Point3> bGoal;
    bGoal.push_back(Point3(-10, 0, -50));
    bGoal.push_back(Point3(10, 0, -50));
    bGoal.push_back(Point3(10, 10, -50));
    bGoal.push_back(Point3(-10, 10, -50));
    quickShapes_.DrawLines(modelMatrix_ * Mblue, viewMatrix_, projMatrix_,
        bCol, bGoal, QuickShapes::LinesType::LINE_LOOP, 0.2);

    // Draw blue goal net
    std::vector<Point3> bNet;
    for (int x = -10; x <= 10; x++) {
      bNet.push_back(Point3(x, 0, -50));
      bNet.push_back(Point3(x, 10, -50));
    }
    for (int y = 2; y <= 10; y++) {
      bNet.push_back(Point3(-10, y, -50));
      bNet.push_back(Point3(10, y, -50));
    }
    quickShapes_.DrawLines(modelMatrix_ * Mblue, viewMatrix_, projMatrix_,
        bCol, bNet, QuickShapes::LinesType::LINES, 0.2);

    // Draw orange goal boundary
    Matrix4 Morange;
    Color oCol(1, 0.65, 0);
    std::vector<Point3> oGoal;
    oGoal.push_back(Point3(-10, 0, 50));
    oGoal.push_back(Point3(10, 0, 50));
    oGoal.push_back(Point3(10, 10, 50));
    oGoal.push_back(Point3(-10, 10, 50));
    quickShapes_.DrawLines(modelMatrix_ * Morange, viewMatrix_, projMatrix_,
        oCol, oGoal, QuickShapes::LinesType::LINE_LOOP, 0.2);

    // Draw orange goal net
    std::vector<Point3> oNet;
    for (int x = -10; x <= 10; x++) {
      oNet.push_back(Point3(x, 0, 50));
      oNet.push_back(Point3(x, 10, 50));
    }
    for (int y = 2; y <= 10; y++) {
      oNet.push_back(Point3(-10, y, 50));
      oNet.push_back(Point3(10, y, 50));
    }
    quickShapes_.DrawLines(modelMatrix_ * Morange, viewMatrix_, projMatrix_,
        oCol, oNet, QuickShapes::LinesType::LINES, 0.2);
}
