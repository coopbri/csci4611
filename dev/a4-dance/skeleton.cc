#include "skeleton.h"
#include "amc_util.h"


Skeleton::Skeleton() : using_degrees_(false) {
}

Skeleton::~Skeleton() {
    
}

int Skeleton::num_root_bones() const {
    return root_bones_.size();
}

std::string Skeleton::root_bone(int i) const {
    return root_bones_[i];
}

int Skeleton::num_children(const std::string &bone_name) const {
    if (children_.find(bone_name) == children_.end()) {
        return 0;
    }
    else {
        return children_.find(bone_name)->second.size();
    }
}

std::string Skeleton::child_bone(const std::string &bone_name, int i) const {
    return children_.find(bone_name)->second[i];
}

bool Skeleton::rx_dof(const std::string &bone_name) const {
    return rx_dof_.find(bone_name)->second;
}

Vector2 Skeleton::rx_limits(const std::string &bone_name) const {
    return rx_limits_.find(bone_name)->second;
}

bool Skeleton::ry_dof(const std::string &bone_name) const {
    return ry_dof_.find(bone_name)->second;
}

Vector2 Skeleton::ry_limits(const std::string &bone_name) const {
    return ry_limits_.find(bone_name)->second;
}

bool Skeleton::rz_dof(const std::string &bone_name) const {
    return rz_dof_.find(bone_name)->second;
}

Vector2 Skeleton::rz_limits(const std::string &bone_name) const {
    return rz_limits_.find(bone_name)->second;
}

int Skeleton::degrees_of_freedom(const std::string &bone_name) const {
    return (int)rx_dof(bone_name) + (int)ry_dof(bone_name) + (int)rz_dof(bone_name);
}


Vector3 Skeleton::BoneDirectionAndLength(const std::string &bone_name) const {
    return lengths_.find(bone_name)->second * directions_.find(bone_name)->second;
}

Matrix4 Skeleton::BoneSpaceToRotAxesSpace(const std::string &bone_name) const {
    return bone_to_rot_axes_.find(bone_name)->second;
}

Matrix4 Skeleton::RotAxesSpaceToBoneSpace(const std::string &bone_name) const {
    return rot_axes_to_bone_.find(bone_name)->second;
}

Matrix4 Skeleton::BoneSpaceToChildrenSpace(const std::string &bone_name) const {
    return bone_to_children_.find(bone_name)->second;
}


const bool ABORT_ON_ERROR=true;
void assume(bool b) {
  if (!b && ABORT_ON_ERROR) {
    std::abort();
  }
}


void Skeleton::LoadFromASF(const std::string &asf_filename) {
    std::ifstream in(asf_filename.c_str());
    SimpleParser parser(&in);
    
    while (parser.Good()) {
        if (parser.Expect("#")) {
            //std::cerr << "Ignoring comment line" << std::endl;
            parser.SwallowLine();
        }
        
        else if (parser.Expect(":version")) {
            parser.SwallowLine();
        }
        
        else if (parser.Expect(":name")) {
            //std::cerr << "Swallowing name" << std::endl;
            parser.SwallowLine();
        }
        
        else if (parser.Expect(":units")) {
            //std::cerr << "Reading units" << std::endl;
            ParseUnits(&parser);
        }
        
        else if (parser.Expect(":documentation")) {
            //std::cerr << "Reading documentation" << std::endl;
            while (parser.Good() && !parser.Peek(":")) {
                parser.SwallowLine();
            }
        }
        
        else if (parser.Expect(":root")) {
            //std::cerr << "Reading root" << std::endl;
            ParseRoot(&parser);
        }
        
        else if (parser.Expect(":bonedata")) {
            //std::cerr << "Reading bonedata" << std::endl;
            ParseBonedata(&parser);
        }
        
        else if (parser.Expect(":hierarchy")) {
            //std::cerr << "Reading hierarchy" << std::endl;
            ParseHierarchy(&parser);
        }
        
        else {
            std::string tok;
            parser.ReadToken(tok);
            if (!parser.Good()) {
                break;
            }
            std::cerr << "Encountered unknown token" << std::endl;
            std::cerr << "'" << tok << "'" << std::endl;
            std::abort();
        }
    } // end while (looping over file)
}


void Skeleton::ParseUnits(SimpleParser *parser) {
    bool cont;
    do {
        cont = false;
        
        if (parser->Expect("mass")) {
            float trash;
            parser->ReadFloat(trash);
            cont = true;
        }
        
        if (parser->Expect("length")) {
            float trash;
            parser->ReadFloat(trash);
            cont = true;
        }
        
        if (parser->Expect("angle")) {
            std::string token;
            parser->ReadToken(token);
            if (token == "deg") {
                using_degrees_ = true;
            }
        }
    } while (cont);
}

void Skeleton::ParseRoot(SimpleParser *parser) {
    bool cont;
    do {
        cont = false;
        
        if (parser->Expect("order")) {
            cont = true;
            if (!parser->Expect("TX TY TZ RX RY RZ")) {
                std::cerr << "'order' not in order expected" << std::endl;
                std::abort();
            }
        }
        
        if (parser->Expect("axis")) {
            cont = true;
            if (!parser->Expect("XYZ")) {
                std::cerr << "'axis' not in order expected" << std::endl;
                std::abort();
            }
        }
        
        if (parser->Expect("position")) {
            cont = true;
            parser->ReadFloat(root_position_[0]);
            parser->ReadFloat(root_position_[1]);
            parser->ReadFloat(root_position_[2]);
            root_position_ = amc2meter(root_position_);
        }
        
        if (parser->Expect("orientation")) {
            cont = true;
            parser->ReadFloat(root_orientation_[0]);
            parser->ReadFloat(root_orientation_[1]);
            parser->ReadFloat(root_orientation_[2]);
        }
        
    } while (cont);
}


void Skeleton::ParseBonedata(SimpleParser *parser) {
    while (parser->Expect("begin")) {
        int id = -1;
        std::string name;
        Vector3 direction;
        float length = 0.0;
        Matrix4 bone_to_rot_axes;
        Matrix4 rot_axes_to_bone;
        bool rx_dof = false;
        bool ry_dof = false;
        bool rz_dof = false;
        Vector2 rx_limits;
        Vector2 ry_limits;
        Vector2 rz_limits;
        
        while (!parser->Expect("end")) {
            
            if (parser->Expect("id")) {
                parser->ReadInt(id);
            }
            
            if (parser->Expect("name")) {
                parser->ReadToken(name);
            }
            
            if (parser->Expect("direction")) {
                parser->ReadFloat(direction[0]);
                parser->ReadFloat(direction[1]);
                parser->ReadFloat(direction[2]);
            }
            
            if (parser->Expect("length")) {
                parser->ReadFloat(length);
                length = amc2meter(length);
            }
            
            if (parser->Expect("axis")) {
                float ax, ay, az;
                std::string axis_type;
                parser->ReadFloat(ax);
                parser->ReadFloat(ay);
                parser->ReadFloat(az);
                if (using_degrees_) {
                    ax = GfxMath::ToRadians(ax);
                    ay = GfxMath::ToRadians(ay);
                    az = GfxMath::ToRadians(az);
                }
                parser->ReadToken(axis_type);
                if (axis_type == "XYZ") {
                    rot_axes_to_bone = Matrix4::RotationZ(az) *
                        Matrix4::RotationY(ay) *
                        Matrix4::RotationX(ax);
                    bone_to_rot_axes = rot_axes_to_bone.Inverse();
                }
                else {
                    std::cerr << "'axis type' not in order expected" << std::endl;
                    std::abort();
                }
            }
            
            if (parser->Expect("dof")) {
                rx_dof = parser->Expect("rx");
                ry_dof = parser->Expect("ry");
                rz_dof = parser->Expect("rz");
            }
            
            
            if (parser->Expect("limits")) {
                int ndof = (int)rx_dof + (int)ry_dof + (int)rz_dof;
                for (int dof=0; dof < ndof; dof++) {
                    assume(parser->Expect("("));
                    float min, max;
                    parser->ReadFloat(min);
                    parser->ReadFloat(max);
                    assume(parser->Expect(")"));
                    
                    if (dof == 0) {
                        if (rx_dof)      rx_limits = Vector2(min, max);
                        else if (ry_dof) ry_limits = Vector2(min, max);
                        else if (rz_dof) rz_limits = Vector2(min, max);
                        else abort();
                    }
                    else if (dof == 1) {
                        if (rx_dof && ry_dof)      ry_limits = Vector2(min, max);
                        else if (rx_dof && rz_dof) rz_limits = Vector2(min, max);
                        else if (ry_dof && rz_dof) rz_limits = Vector2(min, max);
                        else abort();
                    }
                    else if (dof == 2) {
                        if (rx_dof && ry_dof && rz_dof) rz_limits = Vector2(min, max);
                        else abort();
                    }
                    else {
                        std::cerr << "bad dof limits" << std::endl;
                        abort();
                    }
                }
            }
        } // read "end" token
        
        // save the bone to the skeleton data structure
        ids_[name] = id;
        directions_[name] = direction;
        lengths_[name] = length;
        rx_dof_[name] = rx_dof;
        ry_dof_[name] = ry_dof;
        rz_dof_[name] = rz_dof;
        rx_limits_[name] = rx_limits;
        ry_limits_[name] = ry_limits;
        rz_limits_[name] = rz_limits;
        bone_to_rot_axes_[name] = bone_to_rot_axes;
        rot_axes_to_bone_[name] = rot_axes_to_bone;
        bone_to_children_[name] = Matrix4::Translation(length * direction);
    }
}

void Skeleton::ParseHierarchy(SimpleParser *parser) {
    if (!parser->Expect("begin")) {
        std::cerr << "Reading hierarchy, expected 'begin', not found" << std::endl;
        std::abort();
    }
    while (!parser->Expect("end")) {
        std::string line;
        std::string parent;
        parser->ReadToken(parent);
        parser->ReadLine(line);
        
        std::stringstream ss(line);
        std::string child;
        ss >> child;
        while (ss) {
            if (parent == "root") {
                root_bones_.push_back(child);
            }
            else {
                children_[parent].push_back(child);
            }
            ss >> child;
        }
    }
}

