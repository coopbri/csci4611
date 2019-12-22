
#include "pose.h"


Pose::Pose() {}

Pose::~Pose() {}

Matrix4 Pose::RootTransform() const {
    return root_translation_ * root_rotation_;
}

Matrix4 Pose::JointRotation(const std::string &bone) const {
    std::map<std::string, Matrix4>::const_iterator it = joint_rotations_.find(bone);
    if (it == joint_rotations_.end()) {
        return Matrix4();
    }
    else {
        return it->second;
    }
}

Pose Pose::Lerp(const Pose &other, float alpha) const {
    if (alpha == 0.0) {
        return *this;
    }
    else if (alpha == 1.0) {
        return other;
    }
    else {
        Pose new_pose;
        new_pose.set_frame_number(GfxMath::iLerp(frame_num_, other.frame_num_, alpha));
        new_pose.set_root_position(root_position_.Lerp(other.root_position_, alpha));
        Vector3 t1 = root_rel_translation_.ColumnToVector3(3);
        Vector3 t2 = other.root_rel_translation_.ColumnToVector3(3);
        new_pose.set_root_relative_translation(t1.Lerp(t2, alpha));
        
        Quaternion q_root_this = Quaternion::FromEulerAnglesZYX(GfxMath::ToRadians(root_orientation_));
        Quaternion q_root_other = Quaternion::FromEulerAnglesZYX(GfxMath::ToRadians(other.root_orientation_));
        Quaternion q_root = q_root_this.Slerp(q_root_other, alpha);
        Vector3 root_angles = GfxMath::ToDegrees(q_root.ToEulerAnglesZYX());
        new_pose.set_root_orientation(root_angles);
        
        for (std::map<std::string, Vector3>::const_iterator it = joint_angles_.begin(); it != joint_angles_.end(); ++it) {
            
            Quaternion q_joint_this = Quaternion::FromEulerAnglesZYX(GfxMath::ToRadians(it->second));
            Quaternion q_joint_other = Quaternion::FromEulerAnglesZYX(GfxMath::ToRadians(other.joint_angles_.find(it->first)->second));
            Quaternion q_joint = q_joint_this.Slerp(q_joint_other, alpha);
            Vector3 joint_angles = GfxMath::ToDegrees(q_joint.ToEulerAnglesZYX());
            new_pose.set_joint_angles(it->first, joint_angles);
        }
        return new_pose;
    }
}


void Pose::Pose::set_frame_number(int frame_num) {
    frame_num_ = frame_num;
}

void Pose::set_root_position(const Point3 &position) {
    root_position_ = position;
    root_translation_ = Matrix4::Translation(root_position_ - Point3::Origin());
}

void Pose::set_root_relative_translation(const Vector3 &delta_since_last_frame) {
    root_rel_translation_ = Matrix4::Translation(delta_since_last_frame);
}


void Pose::set_root_orientation(const Vector3 &angles) {
    root_orientation_ = angles;
    root_rotation_ = Matrix4::RotationZ(GfxMath::ToRadians(root_orientation_[2])) *
        Matrix4::RotationY(GfxMath::ToRadians(root_orientation_[1])) *
        Matrix4::RotationX(GfxMath::ToRadians(root_orientation_[0]));
}

void Pose::set_joint_angles(const std::string &bone, const Vector3 &angles) {
    joint_angles_[bone] = angles;
    joint_rotations_[bone] = Matrix4::RotationZ(GfxMath::ToRadians(angles[2])) *
        Matrix4::RotationY(GfxMath::ToRadians(angles[1])) *
        Matrix4::RotationX(GfxMath::ToRadians(angles[0]));
}

