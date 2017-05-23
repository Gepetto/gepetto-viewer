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

#include <osgDB/WriteFile>

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
    const char varMat[] = "material";
    const char warning[] = "# Warning: ";
    const std::string varShape = "currentShape";

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

    std::ostream& writeRGB(std::ostream& os, const osgVector4& rgba)
    {
      return os << "( " << rgba.r() << ", " << rgba.g() << ", " << rgba.b() << ", )";
    }

    void writeMaterial(std::ostream& os, const std::string& name, const osg::Material* mat,
        bool diffuse = true, bool ambient = false, bool specular = false) {
      const osg::Material::Face face = osg::Material::FRONT;
      os << varMat << " = bpy.data.materials.new(\"" << name << "\")" << end;
      if (diffuse)
        writeRGB (os << varMat << ".diffuse_color = ", mat->getDiffuse(face)) << end
          << varMat << ".diffuse_intensity = " << mat->getDiffuse(face).a() << end;
      if (ambient)
        writeRGB (os << varMat << ".ambient_color = ", mat->getAmbient(face)) << end
          << varMat << ".ambient_intensity = " << mat->getAmbient(face).a() << end;
      if (specular)
        writeRGB (os << varMat << ".specular_color = ", mat->getSpecular(face)) << end
          << varMat << ".specular_intensity = " << mat->getSpecular(face).a() << end;
    }

    void writeMaterial(std::ostream& os, const std::string& name, const osgVector4& rgbaDiffuse)
    {
      os << varMat << " = bpy.data.materials.new(\"" << name << "\")" << end;
      writeRGB (os << varMat << ".diffuse_color = ", rgbaDiffuse) << end
        << varMat << ".diffuse_intensity = " << rgbaDiffuse.a() << end;
    }

    template <typename NodeType> void setColor(std::ostream& os, NodeType& node, const std::string& var) {
      writeMaterial(os, node.getID() + "__mat", node.getColor());
      os << var << ".materials.append(" << varMat << ')' << end;
    }

    void setColor(std::ostream& os, LeafNodeCollada& node) {
      osg::Material* mat_ptr = dynamic_cast<osg::Material*> (
          node.getOsgNode()->getStateSet()->getAttribute(osg::StateAttribute::MATERIAL)); 
      if (mat_ptr != NULL) { // Color was set by URDF
        writeMaterial(os, node.getID() + "__mat", mat_ptr, true, false, false);
        os << "setMaterial (imported_objects, " << varMat << ')' << end;
      }
    }


    void loadMeshFile(std::ostream& os, const std::string& nodeName, const std::string fn)
    {
      os << "tagObjects()" << end;

      std::string ext = fn.substr(fn.find_last_of(".")+1,fn.size());
      std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
      if(ext == "obj") {
        os << "bpy.ops.import_scene.obj (filepath=\"" << fn << "\")" << end;
      } else if (ext == "dae") {
        os << "bpy.ops.wm.collada_import (filepath=\"" << fn << "\")" << end;
      } else if (ext == "stl") {
        os << "bpy.ops.import_mesh.stl (filepath=\"" << fn << "\")" << end;
      } else {
        std::cout << "Extension of file " << fn << " with name " << nodeName
          << " is not known." << end
          << "You can load the file manually and add it to the empty object called " << nodeName << end
          << "To fix the issue, upate" __FILE__ ":" << __LINE__ << std::endl;
        os << "# Here goes the content of file " << fn << end;
        os << "# To fix it, upate" __FILE__ ":" << __LINE__ << end;
      }

      os
        << "imported_objects = getNonTaggedObjects ()" << end
        << "print(imported_objects)" << end
        << varShape << " = makeEmpty(\"" << nodeName << "__shape\")" << end
        << "setLocQuatSca(" << varShape << ')' << end
        << "setParent (imported_objects, " << varShape << ')' << end;
    }
  }

  BlenderGeomWriterVisitor::BlenderGeomWriterVisitor (const std::string& filename)
    : NodeVisitor(false)
    , filename_ (filename)
    , nodeCount_ (0)
    , groupDepth_ (0)
  {
    bool isNew = !openFile();
    const std::string comment = (isNew ? "" : "# ");
    out() << comment << "import bpy" << end << end
      << comment << "## Start convenient functions" << end
      << comment << "def checkConf():" << end
      << comment << "  if bpy.app.version[0:2] != (2, 75):" << end
      << comment << "    print(\"Using blender version \" + str(bpy.app.version))" << end
      << comment << "    print(\"Developed under version 2.75.0.\")" << end
      << comment << "    return False" << end
      << comment << "  if bpy.context.scene.render.engine != 'CYCLES':" << end
      << comment << "    print(\"Cycles renderer is prefered\")" << end
      << comment << "    return False" << end
      << comment << "  return True" << end
      << comment << end
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
      << comment << "" << end
      << comment << "def setMaterial (children, mat):" << end
      << comment << "  for child in children:" << end
      << comment << "    child.data.materials.append(mat)" << end << end
      << comment << "" << end
      << comment << "def makeEmpty(objname):" << end
      << comment << "  bpy.ops.object.empty_add()" << end
      << comment << "  bpy.context.object.name = objname" << end
      << comment << "  return bpy.context.object" << end
      << comment << "" << end
      << comment << "def setLocQuatSca(obj, loc = (0,0,0), quat=(1,0,0,0), sca=(1,1,1)):" << end
      << comment << "  obj.location = loc" << end
      << comment << "  obj.rotation_mode = 'QUATERNION'" << end
      << comment << "  obj.rotation_quaternion = quat" << end
      << comment << "  obj.scale = sca" << end
      << comment << "" << end
      << comment << "def makePolyLine(objname, curvename, cList):" << end
      << comment << "  curvedata = bpy.data.curves.new(name=curvename, type='CURVE')" << end
      << comment << "  curvedata.dimensions = '3D'" << end
      << comment << "  curvedata.fill_mode = 'FULL'" << end
      << comment << "  curvedata.bevel_depth = 0.01" << end
      << comment << "  curvedata.bevel_resolution = 10" << end
      << comment << "" << end
      << comment << "  objectdata = bpy.data.objects.new(objname, curvedata)" << end
      << comment << "  objectdata.location = (0,0,0) #object origin" << end
      << comment << "  bpy.context.scene.objects.link(objectdata)" << end
      << comment << "  w = 1.0" << end
      << comment << "  polyline = curvedata.splines.new('POLY')" << end
      << comment << "  polyline.points.add(len(cList)-1)" << end
      << comment << "  for num in range(len(cList)):" << end
      << comment << "    x, y, z = cList[num]" << end
      << comment << "    polyline.points[num].co = (x, y, z, w)" << end
      << comment << "  return objectdata, curvedata" << end
      << comment << "" << end << end
      << comment << "## End of convenient functions" << end
      << comment << "checkConf()" << end << end
      << comment << "obj_stack = []" << end;
  }

  bool BlenderGeomWriterVisitor::openFile ()
  {
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
    for (std::size_t i = 0; i < groupDepth_ + 1; ++i)
      out() << group;
    out() << " Group " << node.getID() << end;

    out() << "bpy.ops.object.empty_add()" << end
      << "bpy.context.object.name = \"" << node.getID() << '"' << end;
    // Set parent of this group
    out() << "if obj_stack: bpy.context.object.parent = obj_stack[-1]" << end;

    ++groupDepth_;
    out() << "obj_stack.append(bpy.context.object)" << end;
    NodeVisitor::apply(static_cast<Node&>(node));
    --groupDepth_;
    out() << "obj_stack.pop()" << end;

    for (std::size_t i = 0; i < groupDepth_ + 1; ++i)
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
      << varShape << " = bpy.context.object" << end
      << varShape << ".dimensions = ";
    writeVectorAsList(out(), node.getHalfAxis()*2) << end;

    setColor(out(), node, varShape + ".data");

    standardApply(node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeCapsule& node)
  {
    unimplemented("LeafNodeCapsule", node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeCollada& node)
  {
    loadMeshFile (out(), node.getID(), node.meshFilePath());

    setColor(out(), node);

    standardApply(node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeCone& node)
  {
    out() << "bpy.ops.mesh.primitive_cone_add (radius1="
      << node.getRadius() << ", depth=" << node.getHeight() << ')' << end
      << varShape << " = bpy.context.object" << end;
    setColor(out(), node, varShape + ".data");
    standardApply(node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeCylinder& node)
  {
    out() << "bpy.ops.mesh.primitive_cylinder_add (radius="
      << node.getRadius() << ", depth=" << node.getHeight() << ')' << end
      << varShape << " = bpy.context.object" << end;
    setColor(out(), node, varShape + ".data");
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
    if (node.getMode() != GL_LINE_STRIP) {
      const char varCurve[] = "currentCurve";

      ::osg::Vec3ArrayRefPtr points = node.getPoints();
      out() << "points = [";
      for (std::size_t i = 0; i < points->size(); ++i)
        writeVectorAsList(out(), points->at(i)) << ',';
      out() << ']' << end;

      out()
        << varShape << ", " << varCurve
        << " = makePolyLine(\"" << node.getID() <<  "__shape\", \"" << node.getID() <<  "__curve\", points)" << end;

      setColor(out(), node, varCurve);
    } else {
      std::stringstream ss; ss << filename_ << "_curve" << nodeCount_ << ".dae";
      std::string fn = ss.str();
      osg::ref_ptr <osgDB::Options> os = new osgDB::Options;
      os->setOptionString ("NoExtras");
      osgDB::writeNodeFile (*node.asGroup(), fn, os.get());

      loadMeshFile (out(), node.getID(), fn);
      //setColor(out(), node);
    }
    standardApply(node);
  }
  void BlenderGeomWriterVisitor::apply (LeafNodeSphere& node)
  {
    out () << "bpy.ops.mesh.primitive_ico_sphere_add (size="
      << node.getRadius() << ")" << end
      << varShape << " = bpy.context.object" << end;
    setColor(out(), node, varShape + ".data");
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
      << varShape << ".name = \"" << id << "__shape\"" << end
      << "setLocQuatSca(" << varShape << ", ";
    writeVectorAsList(out() << "loc = " , node.getStaticPosition()) << ", ";
    writeVectorAsList(out() << "quat = ", node.getStaticRotation()) << ", ";
    writeVectorAsList(out() << "sca = " , node.getScale()) << ')' << end;
    out()
      << "obj = makeEmpty(\"" << id << "\")" << end
      << "setLocQuatSca(obj)" << end
      << varShape << ".parent = obj" << end;
    // Set parent group
    out() << "if obj_stack: obj.parent = obj_stack[-1]" << end;

    ++nodeCount_;
    NodeVisitor::apply (node);
  }

  void BlenderGeomWriterVisitor::unimplemented(const char* type, Node& node)
  {
    std::cout << type << " is not implemented. " << node.getID()
      << " not added" << std::endl;
    out() << varShape << " = makeEmpty()" << end;
    standardApply(node);
  }
} // namespace graphics

