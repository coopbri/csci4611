
#ifndef POSE_H_
#define POSE_H_

#include <mingfx.h>
using namespace mingfx;



/** This data structure holds the transformations for the root node and for each
 joint of a Skeleton that are needed to place the Skeleton in one specific pose
 (i.e., one frame of mocap data).  It is possible to linearly interpolate between
 poses using the Lerp command, treating the two end poses as keyframes.
 */
class Pose {
public:
    /// Creates a default (identity) pose
    Pose();
    virtual ~Pose();

    /// Returns the 4x4 transformation matrix (both translation and rotation)
    /// for the root of the skeleton at the time of this pose.
    Matrix4 RootTransform() const;

    /// Returns a 4x4 rotation-only matrix (the translation component will be zero)
    /// that describes the rotation of the joint about which the named bone
    /// rotates at the time of this pose.
    Matrix4 JointRotation(const std::string &bone_name) const;

    /// Linearly iterpolates between this Pose and another.  Internally, rotations
    /// are interpolated using spherical linear interpolation with Quaternions.
    Pose Lerp(const Pose &other, float alpha) const;

    
    /// Returns the position of the root node as a point.
    Point3 root_position() { return root_position_; }

    /// Returns just the rotational component of the root transformation.
    Matrix4 root_rotation() { return root_rotation_; }
    
    /// Returns just the translational component of the root transformation.
    Matrix4 root_translation() { return root_translation_; }

    /// Returns the relative translation from a previous pose to this one.  This
    /// must be set first using a function, such as, MotionClip::CalcRelativeTranslations().
    Matrix4 root_relative_translation() { return root_rel_translation_; }
    
    /// Sets a frame number to associate with the pose.
    void set_frame_number(int frame_num);
    
    /// Sets the root position.
    void set_root_position(const Point3 &pos);

    /// Sets the translation of the root since the last frame.
    void set_root_relative_translation(const Vector3 &delta_since_last_frame);
    
    /// Sets the root orientation using Euler angles, following AMC file format conventions.
    void set_root_orientation(const Vector3 &angles);
    
    /// Sets the joint angles using Euler angles, following AMC file format conventions.
    void set_joint_angles(const std::string &bone_name, const Vector3 &angles);
    
    
private:
    int frame_num_;

    Point3 root_position_;
    Matrix4 root_translation_;
    Matrix4 root_rel_translation_;
    
    Vector3 root_orientation_;
    Matrix4 root_rotation_;
    
    std::map<std::string, Vector3> joint_angles_;
    std::map<std::string, Matrix4> joint_rotations_;
};

#endif


