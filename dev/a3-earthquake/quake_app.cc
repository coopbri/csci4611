/** CSci-4611 Assignment 3:  Earthquake
 */

#include "quake_app.h"
#include "config.h"

#include <iostream>
#include <sstream>

// Number of seconds in 1 year (approx.)
const int PLAYBACK_WINDOW = 12 * 28 * 24 * 60 * 60;

using namespace std;

QuakeApp::QuakeApp() : GraphicsApp(1280,720, "Earthquake"),
    playback_scale_(15000000.0), debug_mode_(false)
{
    // Define a search path for finding data files (images and earthquake db)
    search_path_.push_back(".");
    search_path_.push_back("./data");
    search_path_.push_back(DATA_DIR_INSTALL);
    search_path_.push_back(DATA_DIR_BUILD);

    quake_db_ = EarthquakeDatabase(Platform::FindFile("earthquakes.txt", search_path_));
    current_time_ = quake_db_.earthquake(quake_db_.min_index()).date().ToSeconds();
 }

QuakeApp::~QuakeApp() {
}

void QuakeApp::InitNanoGUI() {
    // Setup the GUI window
    nanogui::Window *window = new nanogui::Window(screen(), "Earthquake Controls");
    window->setPosition(Eigen::Vector2i(10, 10));
    window->setSize(Eigen::Vector2i(400,200));
    window->setLayout(new nanogui::GroupLayout());

    date_label_ = new nanogui::Label(window, "Current Date: MM/DD/YYYY", "sans-bold");

    globe_btn_ = new nanogui::Button(window, "Globe");
    globe_btn_->setCallback(std::bind(&QuakeApp::OnGlobeBtnPressed, this));
    globe_btn_->setTooltip("Toggle between map and globe.");

    new nanogui::Label(window, "Playback Speed", "sans-bold");

    nanogui::Widget *panel = new nanogui::Widget(window);
    panel->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
                                            nanogui::Alignment::Middle, 0, 20));

    nanogui::Slider *slider = new nanogui::Slider(panel);
    slider->setValue(0.5f);
    slider->setFixedWidth(120);

    speed_box_ = new nanogui::TextBox(panel);
    speed_box_->setFixedSize(Eigen::Vector2i(60, 25));
    speed_box_->setValue("50");
    speed_box_->setUnits("%");
    slider->setCallback(std::bind(&QuakeApp::OnSliderUpdate, this, std::placeholders::_1));
    speed_box_->setFixedSize(Eigen::Vector2i(60,25));
    speed_box_->setFontSize(20);
    speed_box_->setAlignment(nanogui::TextBox::Alignment::Right);

    nanogui::Button* debug_btn = new nanogui::Button(window, "Toggle Debug Mode");
    debug_btn->setCallback(std::bind(&QuakeApp::OnDebugBtnPressed, this));
    debug_btn->setTooltip("Toggle displaying mesh triangles and normals (can be slow)");

    screen()->performLayout();
}

void QuakeApp::OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta) {
    // Optional: In our demo, we adjust the tilt of the globe here when the
    // mouse is dragged up/down on the screen.
}

void QuakeApp::OnGlobeBtnPressed() {
    // switch current Earth display mode
    earth_.Toggle();
}

void QuakeApp::OnDebugBtnPressed() {
    debug_mode_ = !debug_mode_;
}

void QuakeApp::OnSliderUpdate(float value) {
    speed_box_->setValue(std::to_string((int) (value * 100)));
    playback_scale_ = 30000000.0*value;
}

void QuakeApp::UpdateSimulation(double dt)  {
    // Advance the current time and loop back to the start if time is past the last earthquake
    current_time_ += playback_scale_ * dt;
    if (current_time_ > quake_db_.earthquake(quake_db_.max_index()).date().ToSeconds()) {
        current_time_ = quake_db_.earthquake(quake_db_.min_index()).date().ToSeconds();
    }
    if (current_time_ < quake_db_.earthquake(quake_db_.min_index()).date().ToSeconds()) {
        current_time_ = quake_db_.earthquake(quake_db_.max_index()).date().ToSeconds();
    }

    Date d(current_time_);
    stringstream s;
    s << "Current date: " << d.month()
        << "/" << d.day()
        << "/" << d.year();
    date_label_->setCaption(s.str());

    // rotate the earth over time
    if (earth_.Sphere()) {
      r_ += M_PI/120; // (pi/2) / 60fps
    } else {
      r_ = 0;
    }
}

void QuakeApp::InitOpenGL() {
    // Set up the camera in a good position to see the entire earth in either mode
    proj_matrix_ = Matrix4::Perspective(60, aspect_ratio(), 0.1, 50);
    view_matrix_ = Matrix4::LookAt(Point3(0,0,3.5), Point3(0,0,0), Vector3(0,1,0));
    glClearColor(0.0, 0.0, 0.0, 1);

    // Initialize the earth object
    earth_.Init(search_path_);

    // Initialize the texture used for the background image
    stars_tex_.InitFromFile(Platform::FindFile("iss006e40544.png", search_path_));
}

void QuakeApp::DrawUsingOpenGL() {
    quick_shapes_.DrawFullscreenTexture(Color(1,1,1), stars_tex_);

    // rotating view of earth
    Matrix4 model_matrix = Matrix4::RotationY(r_);

    // draw Earth
    earth_.Draw(model_matrix, view_matrix_, proj_matrix_);
    if (debug_mode_) {
        earth_.DrawDebugInfo(model_matrix, view_matrix_, proj_matrix_);
    }

    // draw earthquakes
    Earthquake quake;

    // one year of time (earlier than current)
    float year = current_time_ - PLAYBACK_WINDOW;

    // iterate over all earthquakes in the database
    for (int q = 0; q <= quake_db_.max_index(); q++) {
      // current quake at index q
      quake = quake_db_.earthquake(q);

      // time in seconds
      float sec = quake.date().ToSeconds();

      if (sec <= current_time_ && sec >= year) {
        // earthquake magnitude
        double mag = quake.magnitude();

        // earthquake position
        Point3 quakePosition;
        if (earth_.Sphere()) {
          quakePosition = earth_.LatLongToSphere(quake.latitude(),
              quake.longitude());
        } else {
          quakePosition = earth_.LatLongToPlane(quake.latitude(),
              quake.longitude());
        }

        // earthquake size, set in conditional below
        float quakeSize;

        // earthquake color, set in conditional below
        Color quakeColor;

        // determine earthquake class; minimum magnitude is 5.5 (moderate)
        if (mag >= 8) {
          // great; red
          quakeColor = Color(1, 0, 0);
          quakeSize = log(mag) / (quake_db_.max_magnitude() * 5);
        } else if (mag >= 7 && mag <= 7.9) {
          // major; orange
          quakeColor = Color(1, 0.6, 0.4);
          quakeSize = log(mag) / (quake_db_.max_magnitude() * 7);
        } else if (mag >= 6 && mag <= 6.9) {
          // strong; yellow
          quakeColor = Color(1, 1, 0);
          quakeSize = log(mag) / (quake_db_.max_magnitude() * 9);
        } else {
          // moderate; green
          quakeColor = Color(0, 1, 0);
          quakeSize = log(mag) / (quake_db_.max_magnitude() * 12);
        }

        // visual earthquake model transformations
        Matrix4 translate = Matrix4::Translation(Vector3(quakePosition[0],
            quakePosition[1], quakePosition[2]));
        Matrix4 scale = Matrix4::Scale(
            Vector3(quakeSize, quakeSize, quakeSize));
        Matrix4 transform = translate * scale;

        // draw sphere for earthquake
        quick_shapes_.DrawSphere(model_matrix * transform, view_matrix_,
            proj_matrix_, quakeColor);
      }
    }
}
