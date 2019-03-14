// Copyright (c) 2015, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of gepetto-viewer.
// gepetto-viewer is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// gepetto-viewer is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// gepetto-viewer. If not, see <http://www.gnu.org/licenses/>.

#include <gepetto/viewer/transform-writer.h>
#include <gepetto/viewer/node.h>

namespace gepetto {
namespace viewer {
  namespace {
    const char indent[] = "  ";
  }

  void TransformWriter::newFrame ()
  {
    writeNewFrame ();
    frameCount_++;
  }

  void BasicTransformWriter::writeNewFrame ()
  {
    file_ << "FRAME=" << frameCount_ << "\n";
  }

  void BasicTransformWriter::writeTransform (const char* objName,
      const osgVector3& vec, const osgQuat& quat)
  {
    file_ << objName << "=" <<
      vec[0] << ", " <<
      vec[1] << ", " <<
      vec[2] << ", " <<
      quat.w() << ", " <<
      quat.x() << ", " <<
      quat.y() << ", " <<
      quat.z() << "\n";
  }

  void YamlTransformWriter::writeNewFrame ()
  {
    file_ << "frame_" << frameCount_ << ":\n";
  }

  void YamlTransformWriter::writeTransform (const char* objName,
      const osgVector3& vec, const osgQuat& quat)
  {
    file_ << indent << objName << ": ["
      << vec[0] << ", " << vec[1] << ", " << vec[2] << ", " <<
      quat.w() << ", " << quat.x() << ", " << quat.y() << ", " <<
      quat.z() << "]\n";
  }

  void TransformWriterVisitor::apply (Node& node)
  {
    const Configuration& cfg = node.getGlobalTransform ();
    writer_->writeTransform (node.getID ().c_str(), cfg.position, cfg.quat);
    traverse (node);
  }

  void TransformWriterVisitor::captureFrame (Node& node)
  {
    writer_->openFile ();
    writer_->newFrame ();
    apply (node);
    writer_->closeFile ();
  }

} // namespace viewer
} // namespace gepetto
