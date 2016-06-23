// Copyright (c) 2016, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of gepetto-viewer-corba.
// gepetto-viewer-corba is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// gepetto-viewer-corba is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// gepetto-viewer-corba. If not, see <http://www.gnu.org/licenses/>.

#include <gepetto/viewer/blender-geom-writer.h>

#include <algorithm>
#include <sys/stat.h>

#include <gepetto/viewer/node.h>
#include <gepetto/viewer/group-node.h>
#include <gepetto/viewer/leaf-node-arrow.h>
#include <gepetto/viewer/leaf-node-box.h>
#include <gepetto/viewer/leaf-node-capsule.h>
#include <gepetto/viewer/leaf-node-collada.h>
#include <gepetto/viewer/leaf-node-cone.h>
#include <gepetto/viewer/leaf-node-cylinder.h>
#include <gepetto/viewer/leaf-node-face.h>
#include <gepetto/viewer/leaf-node-ground.h>
#include <gepetto/viewer/leaf-node-light.h>
#include <gepetto/viewer/leaf-node-line.h>
#include <gepetto/viewer/leaf-node-sphere.h>
#include <gepetto/viewer/leaf-node-xyzaxis.h>
//#include <gepetto/viewer/node-rod.h>

namespace graphics {
  namespace {
    const char indent[] = "  ";
    const char end = '\n';
    const char group[] = "##";
    const char node[] = "#";

    template <typename Vector>
      std::ostream& writeVectorAsList (std::ostream& os, const Vector& v)
    {
      os << "( ";
      for (int i = 0; i < Vector::num_components; ++i)
        os << v[i] << ", ";
      os << ")";
      return os;
    }

    template <>
      std::ostream& writeVectorAsList<osg::Quat> (std::ostream& os, const osg::Quat& q)
    {
      os << "( " << q.w() << ", " << q.x() << ", " << q.y() << ", " << q.z() << ", )";
      return os;
    }
  }

  BlenderGeomWriterVisitor::BlenderGeomWriterVisitor (const std::string& filename)
    : filename_ (filename)
  {
    bool isNew = !openFile();
    const std::string comment = (isNew ? "" : "# ");
    out() << comment << "import bpy" << end << end
      << comment << "## Start convenient functions" << end
      << comment << "taggedObjects = list()" << end
      << comment << "def tagObjects ():" << end
      << comment << "  global taggedObjects" << end
      << comment << "  taggedObjects = list ()" << end
      << comment << "  for obj in bpy.data.objects:" << end
      << comment << "    taggedObjects.append (obj.name)" << end
      << comment << "" << end
      << comment << "def getNonTaggedObjects ():" << end
      << comment << "  global taggedObjects" << end
      << comment << "  return [obj for obj in bpy.data.objects if obj.name not in taggedObjects]" << end
      << comment << "" << end
      << comment << "def setParent (children, parent):" << end
      << comment << "  for child in children:" << end
      << comment << "    child.parent = parent" << end << end
      << comment << "## End of convenient functions" << end;
  }

  bool BlenderGeomWriterVisitor::openFile () {
    struct stat buffer;   
    bool ret = (stat (filename_.c_str(), &buffer) == 0);
    file_.open (filename_.c_str(),
        std::ofstream::out | std::ofstream::app);
    if (!file_.is_open ())
      throw std::ios_base::failure ("Unable to open file " + filename_);
    return ret;
  }

  void BlenderGeomWriterVisitor::apply (Node& node)
  {
    std::cerr << "This function should not be called. Type of node " << node.getID() << " may be unknown" << std::endl;
    standardApply(node);
  }
  void BlenderGeomWriterVisitor::apply (GroupNode& node)
  {
    for (std::size_t i = 0; i < groupStack_.size() + 1; ++i)
      out() << group;
    out() << " Group " << node.getID() << end;

    out() << "bpy.ops.object.empty_add()" << end
      << "bpy.context.object.name = \"" << node.getID() << '"' << end;
    // Set parent of this group
    if (!groupStack_.empty())
      out() << "bpy.context.object.parent = bpy.data.objects[\""
        << groupStack_.top() << "\"]" << end;

    groupStack_.push(node.getID());
    NodeVisitor::apply(static_cast<Node&>(node));
    groupStack_.pop();

    for (std::size_t i = 0; i < groupStack_.size() + 1; ++i)
      out() << group;
    out() << end;
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeArrow& node)
  {
    unimplemented("LeafNodeArrow", node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeBox& node)
  {
    out()
      << "bpy.ops.mesh.primitive_cube_add ()" << end
      << "bpy.context.object.dimensions = ";
    writeVectorAsList(out(), node.getHalfAxis()*2) << end;
    standardApply(static_cast<Node&>(node));
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeCapsule& node)
  {
    unimplemented("LeafNodeCapsule", node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeCollada& node)
  {
    out() << "tagObjects()" << end;

    const std::string& mesh = node.meshFilePath();

    std::string ext = mesh.substr(mesh.find_last_of(".")+1,mesh.size());
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if(ext == "obj") {
      out() << "bpy.ops.import_scene.obj (filepath=\"" << mesh << "\")" << end;
    } else if (ext == "dae") {
      out() << "bpy.ops.wm.collada_import (filepath=\"" << mesh << "\")" << end;
    } else if (ext == "stl") {
      out() << "bpy.ops.import_mesh.stl (filepath=\"" << mesh << "\")" << end;
    } else {
      std::cout << "Extension of file " << mesh << " with name " << node.getID()
        << " is not known." << end
        << "You can load the file manually and add it to the empty object called " << node.getID() << end
        << "To fix the issue, upate" __FILE__ ":" << __LINE__ << std::endl;
      out() << "# Here goes the content of file " << mesh << end;
      out() << "# To fix it, upate" __FILE__ ":" << __LINE__ << end;
    }

    out ()
      << "imported_objects = getNonTaggedObjects ()" << end
      << "print(imported_objects)" << end
      << "bpy.ops.object.empty_add ()" << end
      << "setParent (imported_objects, bpy.context.object)" << end;
    standardApply(node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeCone& node)
  {
    out() << "bpy.ops.mesh.primitive_cone_add (radius1="
      << node.getRadius() << ", depth=" << node.getHeight() << ')' << end;
    standardApply(node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeCylinder& node)
  {
    out() << "bpy.ops.mesh.primitive_cylinder_add (radius="
      << node.getRadius() << ", depth=" << node.getHeight() << ')' << end;
    standardApply(node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeFace& node)
  {
    unimplemented("LeafNodeFace", node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeGround& node)
  {
    unimplemented("LeafNodeGround", node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeLight& node)
  {
    unimplemented("LeafNodeLight", node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeLine& node)
  {
    unimplemented("LeafNodeLine", node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeSphere& node)
  {
    out () << "bpy.ops.mesh.primitive_ico_sphere_add (size="
      << node.getRadius() << ")" << end;
    standardApply(node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeXYZAxis& node)
  {
    unimplemented("LeafNodeXYZAxis", node);
  }

  void BlenderGeomWriterVisitor::standardApply (Node& node)
  {
    const std::string& id = node.getID ();
    // Set name
    out()
      << "bpy.context.object.name = \"" << id << "__shape\"" << end
      << "bpy.context.object.location = ";
    writeVectorAsList(out(), node.getStaticPosition()) << end;
    out()
      << "bpy.context.object.rotation_mode = 'QUATERNION'" << end
      << "bpy.context.object.rotation_quaternion = ";
    writeVectorAsList(out(), node.getStaticRotation()) << end;
    out()
      << "bpy.ops.object.empty_add ()" << end
      << "bpy.context.object.name = \"" << id << '"' << end
      << "bpy.data.objects[\"" << id << "__shape\"].parent = "
      << "bpy.data.objects[\"" << id << "\"]" << end;
    // Set parent group
    if (!groupStack_.empty())
      out() << "bpy.context.object.parent = bpy.data.objects[\""
        << groupStack_.top() << "\"]" << end;

    NodeVisitor::apply (node);
  }

  void BlenderGeomWriterVisitor::unimplemented(const char* type, Node& node)
  {
    std::cout << type << " is not implemented. " << node.getID()
      << " not added" << std::endl;
    out() << "bpy.ops.object.empty_add()" << end;
    standardApply(node);
  }
} // namespace graphics

