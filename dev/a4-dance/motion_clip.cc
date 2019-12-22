#include "motion_clip.h"

#include <fstream>

#include "amc_util.h"
#include "simple_parser.h"

MotionClip::MotionClip() {
}

MotionClip::~MotionClip() {
}

int MotionClip::size() const {
    return frames_.size();
}

Pose MotionClip::pose(int frame_num) const {
    return frames_[frame_num];
}

Pose& MotionClip::pose(int frame_num) {
    return frames_[frame_num];
}

void MotionClip::TrimFront(int num_frames) {
    frames_.erase(frames_.begin(), frames_.begin() + num_frames);
}

void MotionClip::TrimBack(int num_frames) {
    frames_.erase(frames_.end()-num_frames-1, frames_.end()-1);
}

void MotionClip::PrependPose(const Pose &pose) {
    frames_.insert(frames_.begin(), pose);
}

void MotionClip::AppendPose(const Pose &pose) {
    frames_.push_back(pose);
}


void MotionClip::PrependClip(const MotionClip &clip, int num_blend_frames) {
    for (int i=0; i < clip.size()-num_blend_frames; i++) {
        frames_.insert(frames_.begin(), clip[i]);
    }
    for (int i=0; i<num_blend_frames; i++) {
        int clip_index = clip.size() - num_blend_frames + i;
        int orig_index = clip_index;
        float alpha = (float)i/(float)(num_blend_frames-1);  // 0.0 to 1.0
        Pose blended = clip[clip_index].Lerp(frames_[orig_index], alpha);
        frames_[orig_index] = blended;
    }
}

void MotionClip::AppendClip(const MotionClip &clip, int num_blend_frames) {
    for (int i=0; i<num_blend_frames; i++) {
        int clip_index = i;
        int orig_index = frames_.size() - num_blend_frames + i;
        float alpha = (float)i/(float)(num_blend_frames-1);  // 0.0 to 1.0
        Pose blended = frames_[orig_index].Lerp(clip[clip_index], alpha);
        frames_[orig_index] = blended;
    }
    for (int i=num_blend_frames; i < clip.size(); i++) {
        frames_.push_back(clip[i]);
    }
}


void MotionClip::MakeLoop(int num_blend_frames) {
    MotionClip tmp_clip;
    for (int i=0; i<num_blend_frames; i++) {
        tmp_clip.PrependPose(frames_.back());
        frames_.pop_back();
    }
    for (int i=0; i<tmp_clip.size(); i++) {
        float alpha = (float)i/(float)(tmp_clip.size()-1);
        frames_[i] = tmp_clip[i].Lerp(frames_[i], alpha);
    }
}

void MotionClip::CalcRelativeTranslations() {
    Point3 prev_pos = frames_[0].root_position();
    for (int i=0; i<frames_.size(); i++) {
        Point3 cur_pos = frames_[i].root_position();
        frames_[i].set_root_relative_translation(cur_pos - prev_pos);
        prev_pos = cur_pos;
    }
}


Pose MotionClip::operator[](const int frame_num) const {
    return frames_[frame_num];
}

Pose& MotionClip::operator[](const int frame_num) {
    return frames_[frame_num];
}


void MotionClip::LoadFromAMC(const std::string &filename, const Skeleton &skeleton) {
    std::ifstream ifs(filename.c_str());
    SimpleParser parser(&ifs);
    
    parser.SwallowLine();
    parser.SwallowLine();
    parser.SwallowLine();
    
    while (parser.Good()) {
        Pose frame;
        int frame_num;
        parser.ReadInt(frame_num);
        if (!parser.Good()) {
            return;
        }
        frame.set_frame_number(frame_num);
        
        while (!parser.UpcomingInt()) {
            std::string bone;
            parser.ReadToken(bone);
            if (!parser.Good()) {
                return;
            }
            if (bone == "root") {
                Point3 position;
                parser.ReadFloat(position[0]);
                parser.ReadFloat(position[1]);
                parser.ReadFloat(position[2]);
                position = amc2meter(position);
                frame.set_root_position(position);
                
                Vector3 orientation;
                parser.ReadFloat(orientation[0]);
                parser.ReadFloat(orientation[1]);
                parser.ReadFloat(orientation[2]);
                frame.set_root_orientation(orientation);                
            }
            else {
                float rx=0, ry=0, rz=0;
                
                if (skeleton.rx_dof(bone)) {
                    parser.ReadFloat(rx);
                }
                if (skeleton.ry_dof(bone)) {
                    parser.ReadFloat(ry);
                }
                if (skeleton.rz_dof(bone)) {
                    parser.ReadFloat(rz);
                }
                frame.set_joint_angles(bone, Vector3(rx, ry, rz));
            }
        }
        frames_.push_back(frame);
    }
}


