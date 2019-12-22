
#ifndef ANIMATION_CLIP_H_
#define ANIMATION_CLIP_H_

#include <mingfx.h>
#include "pose.h"
#include "skeleton.h"


/** A MotionClip is a series of poses that can be applied to an animated character.
 In addition to storing that list, this class provides some utility functions for
 editing the clip, including triming unwanted frames from the front or back or 
 prepended or appending additional frames to the clip.  You can also make the clip
 loop smoothly. */
class MotionClip {
public:
    /// Creates an empty clip.
    MotionClip();
    virtual ~MotionClip();

    /// Loads a clip from a file in the AMC mocap data format.  A reference to
    /// the skeleton used during the data capture is required in order to know
    /// the proper bone names and hierarchical arrangement.
    void LoadFromAMC(const std::string &filename, const Skeleton &skeleton);

    /// Adds a single pose to the beginning of the motion clip, shifting all
    /// frames later by 1.
    void PrependPose(const Pose &pose);

    /// Adds a single pose to the end of the motion clip.
    void AppendPose(const Pose &pose);
    
    /// Combines this motion clip and another by prepending the other.  If you
    /// wish to smoothly blend (i.e., cross disolve) between the two clips, then
    /// use a value > 0 for num_blend_frames.
    void PrependClip(const MotionClip &clip, int num_blend_frames);

    /// Combines this motion clip and another by appending the other.  If you
    /// wish to smoothly blend (i.e., cross disolve) between the two clips, then
    /// use a value > 0 for num_blend_frames.
    void AppendClip(const MotionClip &clip, int num_blend_frames);

    /// Turns the motion clip into a continuous loop by blending the last
    /// num_blend_frames/2 poses with the first num_blend_frames/2 poses.
    void MakeLoop(int num_blend_frames);

    /// Removes some poses from the front of the motion clip.
    void TrimFront(int num_frames);

    /// Removes some poses from the end of the motoin clip.
    void TrimBack(int num_frames);

    /// Runs through the motion clip, frame by frame, and calculates the relative
    /// translation from one frame to the next, storing the results in each pose.
    /// This is not needed if the motion clip will only be used for playback in
    /// an absolute positioning mode, but it is needed for playback in a relative
    /// positioning mode.
    void CalcRelativeTranslations();

    /// Returns the length (in number of frames) of the motoin clip.
    int size() const;
    
    /// Returns the character's pose for the given frame.
    Pose pose(int frame_num) const;

    /// Returns the character's pose for the given frame.
    Pose& pose(int frame_num);
    
    /// Returns the character's pose for the given frame.
    Pose operator[](const int frame_num) const;
    
    /// Returns the character's pose for the given frame.
    Pose& operator[](const int frame_num);
    
private:
    std::vector<Pose> frames_;
};

#endif
