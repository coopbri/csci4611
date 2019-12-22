#ifndef SKELETON_H_
#define SKELETON_H_

#include <mingfx.h>
using namespace mingfx;

#include "simple_parser.h"


/** This data structure describes a rigid body skeleton used to record and play
 back motion capture data.  Each skelton has a root node, which may have one or
 more child bones.  Each bone, may have zero or more child bones.
 
 */
class Skeleton {
public:
    
    /// Creates an empty skeleton
    Skeleton();
    virtual ~Skeleton();
    
    /// Loads a skeleton from the Acclaim ASF file format.
    void LoadFromASF(const std::string &asf_filename);

    /// The number of bones attached to the root node
    int num_root_bones() const;
    
    /// The name of the i-th bone attached to the root node
    std::string root_bone(int i) const;

    /// The number of children of the named bone
    int num_children(const std::string &bone_name) const;
    
    /// The name of the i-th child of the named bone
    std::string child_bone(const std::string &bone_name, int i) const;
    
    
    /** The joint angle rotations that produce the animation are applied around
     a set of x,y,z axes that make sense for the joint (e.g., a joint with
     only 1 degree-of-freedom might setup a coordinate system where the X-axis
     is aligned along the hinge of that joint).  The following transform
     rotates the bone's local coordinate system into the joint angle coordinate
     system.  This must be done before the joint angles can be applied.  Then,
     after the joint angles are applied, we must rotate back before applying
     the to_parent transform.
     */
    Matrix4 BoneSpaceToRotAxesSpace(const std::string &bone_name) const;

    /** The inverse of BoneSpaceToRotAxesSpace().  Use this matrix to rotate 
     back to the bone's local coordinate system after applying joint angle 
     rotations. */
    Matrix4 RotAxesSpaceToBoneSpace(const std::string &bone_name) const;
    
    /** This matrix transforms from the bone's local coordinate system to the
     coordinate system of its children, which is the same as translating from
     the bone's origin by the BoneDirectionAndLength() */
    Matrix4 BoneSpaceToChildrenSpace(const std::string &bone_name) const;
    
    /** Returns the direction and length of the bone.  If you translate from the
     bone's origin along this vector, you will reach the point that should be
     used as the origin for the bone's children. */
    Vector3 BoneDirectionAndLength(const std::string &bone_name) const;
    
    
    
    /* Most users will not need to access the routines below this line because
       all of these properties are taken into account when calculating the
       matrices above. */

    /// True if the joint used to rotate bone_name can rotate around the X axis.
    bool rx_dof(const std::string &bone_name) const;

    /// Returns the valid range of X-axis joint angles for the joint used to rotate bone_name.
    Vector2 rx_limits(const std::string &bone_name) const;
    
    /// True if the joint used to rotate bone_name can rotate around the Y axis.
    bool ry_dof(const std::string &bone_name) const;

    /// Returns the valid range of Y-axis joint angles for the joint used to rotate bone_name.
    Vector2 ry_limits(const std::string &bone_name) const;
    
    /// True if the joint used to rotate bone_name can rotate around the Z axis.
    bool rz_dof(const std::string &bone_name) const;
    
    /// Returns the valid range of Z-axis joint angles for the joint used to rotate bone_name.
    Vector2 rz_limits(const std::string &bone_name) const;
    
    /// Returns 0,1,2 or 3 for the number of degrees of freedom of the joint
    /// used to rotate the named bone.
    int degrees_of_freedom(const std::string &bone_name) const;
    
    
    /// Typically zero, and we can ignore this.
    Vector3 skeleton_root_position() const;

    /// Typically zero, and we can ignore this.
    Vector3 skeleton_root_orientation() const;
    
    
private:
    bool using_degrees_;

    std::vector<std::string> root_bones_;
    Vector3 root_position_;
    Vector3 root_orientation_;

    std::map<std::string, int> ids_;
    
    std::map<std::string, std::vector<std::string> > children_;
    std::map<std::string, Vector3> directions_;

    std::map<std::string, float> lengths_;
    
    std::map<std::string, bool> rx_dof_;
    std::map<std::string, Vector2> rx_limits_;
    
    std::map<std::string, bool> ry_dof_;
    std::map<std::string, Vector2> ry_limits_;
    
    std::map<std::string, bool> rz_dof_;
    std::map<std::string, Vector2> rz_limits_;
    
    std::map<std::string, Matrix4> bone_to_rot_axes_;
    std::map<std::string, Matrix4> rot_axes_to_bone_;
    std::map<std::string, Matrix4> bone_to_children_;
    
    
    // Helpers for parsing skeleton data from file
    void ParseUnits(SimpleParser *parser);
    void ParseRoot(SimpleParser *parser);
    void ParseBonedata(SimpleParser *parser);
    void ParseBone(SimpleParser *parser, bool dg);
    void ParseHierarchy(SimpleParser *parser);
};

#endif
