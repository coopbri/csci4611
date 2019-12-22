#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include "animated_character.h"
#include "amc_util.h"

AnimatedCharacter::AnimatedCharacter(const std::string &asf_filename) :
    fps_(120.0), elapsed_since_last_frame_(0.0), current_frame_(0)
{
    LoadSkeleton(asf_filename);
}

AnimatedCharacter::AnimatedCharacter() :
    fps_(120.0), elapsed_since_last_frame_(0.0), current_frame_(0)
{
}

AnimatedCharacter::~AnimatedCharacter() {
}


void AnimatedCharacter::LoadSkeleton(const std::string &asf_filename) {
    skeleton_.LoadFromASF(asf_filename);
}


void AnimatedCharacter::Play(const MotionClip &motion_clip) {
    motion_queue_.clear();
    motion_queue_.push_back(motion_clip);
    current_frame_ = 0;
}


void AnimatedCharacter::Queue(const MotionClip &motion_clip) {
    if (motion_queue_.size() == 0) {
        Play(motion_clip);
    }
    else {
        motion_queue_.push_back(motion_clip);
    }
}


void AnimatedCharacter::ClearQueue() {
    motion_queue_.clear();
}


void AnimatedCharacter::OverlayClip(const MotionClip &clip, int num_transition_frames) {
    overlay_clip_ = clip;
    overlay_transition_frames_ = num_transition_frames;
    overlay_frame_ = 0;
}


void AnimatedCharacter::AdvanceAnimation(double dt) {
    if (motion_queue_.size() == 0) {
        pose_ = Pose();
    }
    else {
        elapsed_since_last_frame_ += dt;

        double frames_to_advance = fps_ * elapsed_since_last_frame_;
        double whole_frames;
        double frac = modf(frames_to_advance, &whole_frames);
        int nframes = (int)whole_frames;
        elapsed_since_last_frame_ = frac / fps_;

        for (int i=0; i<nframes; i++) {
            // advance the main motion track
            current_frame_++;
            // handle end case
            if (current_frame_ >= motion_queue_[0].size()) {
                // loop back to the first frame
                current_frame_ = 0;
                // if there are more motions in the queue then pop this one and goto the next
                if (motion_queue_.size() > 1) {
                    motion_queue_.erase(motion_queue_.begin());
                }
            }

            // advance the overlay clip if there is one
            if (overlay_clip_.size()) {
                overlay_frame_++;
                // handle end case
                if (overlay_frame_ >= overlay_clip_.size()) {
                    // done playing overlay, reset frame counter and clear the overlay clip
                    overlay_frame_ = 0;
                    overlay_clip_ = MotionClip();
                }
            }

            // update the pose based on new frames
            CalcCurrentPose();

            // add to the translation matrix for the case when relative root motion is used
            accum_translation_matrix_ = accum_translation_matrix_ * pose_.root_relative_translation();
        }
    }
}


void AnimatedCharacter::CalcCurrentPose() {
    if (!overlay_clip_.size()) {
        // no overaly track, motion is entirely from the base track (i.e., the motion queue)
        pose_ = motion_queue_[0][current_frame_];
    }
    else {
        // there is an active overlay track
        if (overlay_frame_ < overlay_transition_frames_) {
            // fade in the overlay
            float alpha = (float)overlay_frame_/(float)overlay_transition_frames_;
            pose_ = motion_queue_[0][current_frame_].Lerp(overlay_clip_[overlay_frame_], alpha);
        }
        else if (overlay_frame_ > overlay_clip_.size() - overlay_transition_frames_) {
            // fade out the overlay
            float alpha = (float)(overlay_clip_.size() - overlay_frame_)/(float)overlay_transition_frames_;
            pose_ = motion_queue_[0][current_frame_].Lerp(overlay_clip_[overlay_frame_], alpha);
        }
        else {
            // overlay is completely faded in, we don't see the base track at all
            pose_ = overlay_clip_[overlay_frame_];
        }
    }
}


Skeleton* AnimatedCharacter::skeleton_ptr() {
    return &skeleton_;
}


void AnimatedCharacter::set_fps(int fps) {
    fps_ = fps;
}


int AnimatedCharacter::fps() {
    return fps_;
}



void AnimatedCharacter::Draw(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix,
                             bool use_absolute_position)
{
    Matrix4 character_root_transform;
    if (use_absolute_position) {
        // set root position based on the absolute position in the mocap data
        character_root_transform = model_matrix * pose_.RootTransform();
    }
    else {
        // set root position based on the relative updates accumulated each frame
        character_root_transform = model_matrix * accum_translation_matrix_ * pose_.root_rotation();
    }

    for (int i=0; i<skeleton_.num_root_bones(); i++) {
        DrawBoneRecursive(skeleton_.root_bone(i), character_root_transform, view_matrix, proj_matrix);
    }
}


void AnimatedCharacter::DrawBoneRecursive(const std::string &bone_name, const Matrix4 &parent_transform,
                                          const Matrix4 &view_matrix, const Matrix4 &proj_matrix)
{
    // Current transformation matrix
    Matrix4 ctm = parent_transform
        * skeleton_.RotAxesSpaceToBoneSpace(bone_name)
        * pose_.JointRotation(bone_name)
        * skeleton_.BoneSpaceToRotAxesSpace(bone_name);

    // main ant color (reddish)
    Color ant_color = Color(1, 0.45, 0);

    // stick figure color (black)
    Color stick_color = Color(0, 0, 0);

    // commonly-used zero-point in 3-space
    Point3 zero = Point3(0, 0, 0);

    // stick-figure component width
    float fig_width = 0.01;

    // Draw different shapes based on part of body
    if (bone_name == "lhipjoint" || bone_name == "rhipjoint") {
	    quick_shapes_.DrawLineSegment(ctm, view_matrix, proj_matrix,
          stick_color, zero, zero
          + skeleton_.BoneDirectionAndLength(bone_name), fig_width);

      quick_shapes_.DrawSphere(ctm * Matrix4::RotationX(M_PI/4)
          * Matrix4::Scale(Vector3(0.15, 0.3, 0.15))
          * Matrix4::Translation(Vector3(0, -0.5, 0)),
          view_matrix, proj_matrix, ant_color);
    }
    if (bone_name == "lfemur" || bone_name == "rfemur") {
	    quick_shapes_.DrawLineSegment(ctm, view_matrix, proj_matrix,
          stick_color, zero, zero
          + skeleton_.BoneDirectionAndLength(bone_name), fig_width);
    }
    if (bone_name == "ltibia" || bone_name == "rtibia") {
	    quick_shapes_.DrawLineSegment(ctm, view_matrix, proj_matrix,
          stick_color, zero, zero
          + skeleton_.BoneDirectionAndLength(bone_name), fig_width);
    }
    if (bone_name == "lfoot" || bone_name == "rfoot") {
      quick_shapes_.DrawLineSegment(ctm, view_matrix, proj_matrix,
          stick_color, zero, zero
          + skeleton_.BoneDirectionAndLength(bone_name), fig_width);
    }
    if (bone_name == "ltoes" || bone_name == "rtoes") {
      quick_shapes_.DrawLineSegment(ctm, view_matrix, proj_matrix,
          stick_color, zero, zero
          + skeleton_.BoneDirectionAndLength(bone_name), fig_width);
    }
    if (bone_name == "lowerback") {
      quick_shapes_.DrawLineSegment(ctm, view_matrix, proj_matrix,
          stick_color, zero, zero
          + skeleton_.BoneDirectionAndLength(bone_name), fig_width);
    }
    if (bone_name == "upperback") {
      quick_shapes_.DrawLineSegment(ctm, view_matrix, proj_matrix,
          stick_color, zero, zero
          + skeleton_.BoneDirectionAndLength(bone_name), fig_width);

      quick_shapes_.DrawSphere(ctm * Matrix4::RotationX(M_PI/3)
          * Matrix4::Scale(Vector3(0.1, 0.12, 0.1))
          * Matrix4::Translation(Vector3(0, 0.4, 0)),
          view_matrix, proj_matrix, ant_color);
    }
    if (bone_name == "thorax") {
      quick_shapes_.DrawLineSegment(ctm, view_matrix, proj_matrix,
          stick_color, zero, zero
          + skeleton_.BoneDirectionAndLength(bone_name), fig_width);

      quick_shapes_.DrawSphere(ctm * Matrix4::RotationX(M_PI/10)
          * Matrix4::Scale(Vector3(0.1, 0.12, 0.1))
          * Matrix4::Translation(Vector3(0, 0.4, 0)),
          view_matrix, proj_matrix, ant_color);
    }
    if (bone_name == "lowerneck" || bone_name == "upperneck") {
	    quick_shapes_.DrawLineSegment(ctm, view_matrix, proj_matrix,
          stick_color, zero, zero
          + skeleton_.BoneDirectionAndLength(bone_name), fig_width);
    }
    if (bone_name == "head") {
      // base of head
      quick_shapes_.DrawSphere(ctm * Matrix4::RotationX(-M_PI/8)
          * Matrix4::Scale(Vector3(0.1, 0.2, 0.1))
          * Matrix4::Translation(Vector3(0, 0.2, 0)),
          view_matrix, proj_matrix, ant_color);

      // left eye
      quick_shapes_.DrawSphere(ctm * Matrix4::Scale(Vector3(0.03, 0.03, 0.02))
          * Matrix4::Translation(Vector3(-2, 4.6, 0)),
            view_matrix, proj_matrix, stick_color);

      // right eye
      quick_shapes_.DrawSphere(ctm * Matrix4::Scale(Vector3(0.03, 0.03, 0.02))
          * Matrix4::Translation(Vector3(2, 4.6, 0)),
          view_matrix, proj_matrix, stick_color);

      // left antenna
      quick_shapes_.DrawCone(ctm * Matrix4::RotationZ(M_PI/6)
          * Matrix4::Scale(Vector3(0.01, 0.08, 0.01))
          * Matrix4::Translation(Vector3(2.4, 3.2, -2.8)),
          view_matrix, proj_matrix, stick_color);
      quick_shapes_.DrawCone(ctm * Matrix4::Translation(Vector3(-0.12, 0.27, 0.1))
          * Matrix4::RotationZ(M_PI/2) * Matrix4::RotationX(M_PI/2)
          * Matrix4::Scale(Vector3(0.01, 0.12, 0.01))
          * Matrix4::Translation(Vector3(0.12, -0.27, -0.1)),
          view_matrix, proj_matrix, stick_color);

      // right antenna
      quick_shapes_.DrawCone(ctm * Matrix4::RotationZ(-M_PI/6)
          * Matrix4::Scale(Vector3(0.01, 0.08, 0.01))
          * Matrix4::Translation(Vector3(-2.4, 3.2, -2.8)),
          view_matrix, proj_matrix, stick_color);
      quick_shapes_.DrawCone(ctm * Matrix4::Translation(Vector3(0.12, 0.27, 0.1))
          * Matrix4::RotationZ(M_PI/2) * Matrix4::RotationX(M_PI/2)
          * Matrix4::Scale(Vector3(0.01, 0.12, 0.01))
          * Matrix4::Translation(Vector3(-0.12, -0.27, -0.1)),
          view_matrix, proj_matrix, stick_color);

      // mouth
      quick_shapes_.DrawSphere(ctm * Matrix4::Scale(Vector3(0.05, 0.03, 0.02))
          * Matrix4::Translation(Vector3(0, -2, 5)),
          view_matrix, proj_matrix, stick_color);
    }
    if (bone_name == "lclavicle" || bone_name == "rclavicle") {
      quick_shapes_.DrawLineSegment(ctm, view_matrix, proj_matrix,
          stick_color, zero, zero
          + skeleton_.BoneDirectionAndLength(bone_name), fig_width);
    }
    if (bone_name == "lhumerus" || bone_name == "rhumerus"
     || bone_name == "lradius" || bone_name == "rradius") {
  	    quick_shapes_.DrawLineSegment(ctm, view_matrix, proj_matrix,
            stick_color, zero, zero
            + skeleton_.BoneDirectionAndLength(bone_name), fig_width);
    }
    if (bone_name == "lwrist" || bone_name == "rwrist") {
  		quick_shapes_.DrawLineSegment(ctm, view_matrix, proj_matrix,
          stick_color, zero, zero
          + skeleton_.BoneDirectionAndLength(bone_name), fig_width);
    }
    if (bone_name == "lhand" || bone_name == "rhand" || bone_name == "lthumb"
     || bone_name == "rthumb" || bone_name == "rfingers"
     || bone_name == "lfingers") {
    		quick_shapes_.DrawLineSegment(ctm, view_matrix, proj_matrix,
            stick_color, zero, zero
            + skeleton_.BoneDirectionAndLength(bone_name), fig_width);
    }

    // Draw the bone's children using the CTM matrix
    Matrix4 child_root_transform = ctm
        * skeleton_.BoneSpaceToChildrenSpace(bone_name);

    for (int i = 0; i < skeleton_.num_children(bone_name); i++) {
        DrawBoneRecursive(skeleton_.child_bone(bone_name, i),
            child_root_transform, view_matrix, proj_matrix);
    }
}
