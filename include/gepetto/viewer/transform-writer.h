//
//  transform-writer.h
//  gepetto-viewer
//
//  Created by Joseph Mirabel in November 2015.
//  Copyright (c) 2015 LAAS-CNRS. All rights reserved.
//

#ifndef SCENEVIEWER_TRANSFORM_WRITER_VISITOR_HH
#define SCENEVIEWER_TRANSFORM_WRITER_VISITOR_HH

#include <iostream>
#include <fstream>
#include <gepetto/viewer/config-osg.h>
#include <gepetto/viewer/node-visitor.h>

namespace graphics {

    DEF_CLASS_SMART_PTR(TransformWriter)
    DEF_CLASS_SMART_PTR(BasicTransformWriter)
    DEF_CLASS_SMART_PTR(YamlTransformWriter)
    DEF_CLASS_SMART_PTR(TransformWriterVisitor)

    class TransformWriter : public osg::Referenced
    {
      public:
        // TODO: set the buffer of the filebuf manually so as to increase buffer
        // size.
        TransformWriter (const std::string filename)
          : filename_ (filename)
          , frameCount_ (0)
        {}

        virtual ~TransformWriter () {}

        void newFrame ();
        virtual void writeTransform (const char* objName,
            const osgVector3& vec, const osgQuat& quat) = 0;

        std::ofstream& out () { return file_; }

        void openFile () {
          file_.open (filename_.c_str(),
              std::ofstream::out | std::ofstream::app);
          if (!file_.is_open ())
            throw std::ios_base::failure ("Unable to open file " + filename_);
        }
        void closeFile () {
          file_.close ();
        }

      protected:
        virtual void writeNewFrame () = 0;

        std::ofstream file_;
        std::string filename_;
        unsigned int frameCount_;
    };

    class BasicTransformWriter : public TransformWriter
    {
      public:
        BasicTransformWriter (const std::string filename)
          : TransformWriter (filename)
        {}

        ~BasicTransformWriter () {}

        void writeTransform (const char* objName,
            const osgVector3& vec, const osgQuat& quat);

      protected:
        void writeNewFrame ();
    };

    class YamlTransformWriter : public TransformWriter
    {
      public:
        YamlTransformWriter (const std::string filename)
          : TransformWriter (filename)
        {}

        ~YamlTransformWriter () {}

        void writeTransform (const char* objName,
            const osgVector3& vec, const osgQuat& quat);

      protected:
        void writeNewFrame ();
    };

    class TransformWriterVisitor :  public NodeVisitor
    {
      public:
        TransformWriterVisitor (TransformWriter* writer)
          : NodeVisitor (false)
          , writer_ (writer)
        {}

        void captureFrame (Node& node);

        template <typename Iterator>
        void captureFrame (Iterator begin, Iterator end);

        virtual void apply (Node& node);

        osg::ref_ptr <TransformWriter> writer_;
    }; /* class TransformWriterVisitor */


    template <typename Iterator>
      void TransformWriterVisitor::captureFrame (Iterator begin, Iterator end)
    {
      writer_->openFile ();
      writer_->newFrame ();
      for (Iterator it = begin; it != end; ++it) apply (**it);
      writer_->closeFile ();
    }
} /* namespace graphics */

#endif /* SCENEVIEWER_TRANSFORM_WRITER_VISITOR_HH */
