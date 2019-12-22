#ifndef ANIMATED_CHARACTER_H_
#define ANIMATED_CHARACTER_H_

#include <mingfx.h>
#include <string>
#include <map>

#include "motion_clip.h"
#include "pose.h"
#include "skeleton.h"


/** An AnimatedCharacter combines a hierarchical Skeleton of bones with one or
 more Poses that specify how the bones are currently positioned in space.  The
 Poses can be stored in the form of a MotionClip, and this allows for animated
 playback.  Example:
 ~~~
 AnimatedCharacter bob;
 MotionClip jump_motion;
 
 void MyGraphicsApp::InitOpenGL() {
    std::vector<std::string> search_path;
    search_path.push_back("./data");

    // Load a skeleton for bob
    bob.LoadSkeleton(Platform::FindFile("bob.asf", search_path));
   
    // Load a walking motion and tell bob to start playing it back on repeat
    MotionClip walk1;
    walk1.LoadFromAMC(Platform::FindFile("human_walk.amc", searchPath_), bob.skeleton_ptr());
    bob.Play(walk1);
 
    // Load a jumping motion clip to use later...
    jump_motion.LoadFromAMC(Platform::FindFile("human_jump.amc", searchPath_), bob.skeleton_ptr());
 }
 
 void MyGraphicsApp::UpdateSimulation(double dt)  {
    // Advances bob to the next frame of the animation
    bob.AdvanceAnimation(dt);
 }
 
 void MyGraphicsApp::DrawUsingOpenGL() {
    // Draws bob using his current pose (i.e., whatever pose was setup during
    // the last call to bob.AdvanceAnimation()
    bob.Draw(model_matrix, view_matrix, proj_matrix);
 }
 
 void MyGraphicsApp::OnButtonPressed() {
    // Imagine we want bob to jump every time a button is pressed, this overlays
    // a jumping mocap clip on top of bob's current motion.  Once the jump is
    // played back once, bob returns to his previous motion.
    bob.OverlayClip(jump_motion, 50);
 }
 ~~~
 */
class AnimatedCharacter {
public:
    
    /** Creates an animated character with a skeleton loaded from the specified
     file */
    AnimatedCharacter(const std::string &asf_filename);

    /** Creates an animated character with an empty skeleton.  The skeleton must
     be loaded with LoadSkeleton() before the character can be animated. */
    AnimatedCharacter();

    virtual ~AnimatedCharacter();
    
    /** Loads a skeleton from the specified file.  The ASF file format is the
     only format supported.*/
    void LoadSkeleton(const std::string &asf_filename);
    
    
    /** Clears the current motion queue and starts playing the specified motion 
     clip immediately.  The clip will automatically repeat when finished.*/
    void Play(const MotionClip &motion_clip);

    /** Adds the specified motion clip to end of the current play queue.  If the
     queue is currently empty, then the motion will start playing immediately.
     If the queue is not empty then the motion will start playing after all the
     previously added clips finish playing.*/
    void Queue(const MotionClip &motion_clip);
    
    /** Removes all motion clips from the current queue and resets the current
     pose to the default pose of the skeleton.*/
    void ClearQueue();

    
    /** Assuming the character is currently in the middle of an animation, this
     function briefly interrupts that current motion in order to play a new 
     motion clip.  Use this to apply new behaviors on command.  In a game where
     you press the 'A' button to punch, you could call this function to overlay
     a punching motion clip on top of the current base motion of the character.
     We call this an "overlay" rather than simply "insert" because the function
     also interpolates between the current motion clip and the overlay clip so 
     that there is a smooth transition in the motion.  You can control how smooth
     the transition is by setting the num_transition_frames parameter.  A larger
     number will create a longer, smoother transition.  num_transition_frames are
     used both to "fade in" the overlay motion and to "fade it out".  If you have
     an overlay_clip that is 300 frames long and num_transition_frames=50, then
     the first 50 frames of the overlay_clip will be blended with the next 50
     frames of the motion clip that the character is currently using.  Then, the
     middle 200 frames of the overlay_clip will be played on their own.  Then,
     the final 50 frames of the overlay_clip will again be blended with the next
     50 frames of the character's current motion clip to "fade out" the overlay.
     */
    void OverlayClip(const MotionClip &overlay_clip, int num_transition_frames);


    /** You must call this function to advance the frame of the current motion
     clip.  Pass the elapsed time since the last call to AdvanceAnimation in the 
     dt (delta t) argument.  The number of frames to advance is determined based
     on the real-world elapsed time, and is determined by the character's frames
     per second (fps).  For example, if the character is set to have a fps=120,
     and the dt since the last frame is 1/120 seconds, then the character will
     advance 1 frame.  If the fps=120 and the dt=1/60, then the character will
     advance 2 frames, etc..*/
    void AdvanceAnimation(double dt);
    
    
    /** Draws the character using the supplied matrices.  If use_absolute_position
     is true, then the character's root position will be set using the absolute
     position in the root transformation matrix for the current pose.  If it is
     false, then the relative change in */
    void Draw(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix,
              bool use_absolute_position=true);
    
    
    
    Skeleton* skeleton_ptr();
    
    void set_fps(int fps);
    
    int fps();
    
private:
    
    void CalcCurrentPose();
    
    void DrawBoneRecursive(const std::string &bone_name, const Matrix4 &parent_transform,
                           const Matrix4 &view_matrix, const Matrix4 &proj_matrix);
    
    // Raw skeleton data
    Skeleton skeleton_;

    // Raw character motion data
    std::vector<MotionClip> motion_queue_;

    // "frames per second" - we assume all clips in the playlist are played back at the same rate
    double fps_;
    
    // time since we last advanced to the next frame in the clip
    double elapsed_since_last_frame_;

    // current frame to display from motion_queue_[0]
    int current_frame_;

    // if we are overlaying a clip on top of the main motion, this holds that clip
    MotionClip overlay_clip_;
    
    // current frame to display from overlay_clip_
    int overlay_frame_;

    // number of frames used to fade-in and fade-out the overlay clip
    int overlay_transition_frames_;

    // cached value of the current pose
    Pose pose_;
    
    // the accumulated relative translations from frame-to-frame, not used when
    // drawing with absolute position turned on.
    Matrix4 accum_translation_matrix_;
    
    // for drawing the character
    QuickShapes quick_shapes_;
};


#endif
