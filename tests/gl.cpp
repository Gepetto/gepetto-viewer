#include <stdio.h>

#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <osg/GLExtensions>
#include <osgViewer/Viewer>

const int OSG_WIDTH = 1024;
const int OSG_HEIGHT = 960;

class TestSupportOperation : public osg::GraphicsOperation {
 public:
  TestSupportOperation()
      : osg::Referenced(true),
        osg::GraphicsOperation("TestSupportOperation", false),
        m_supported(true),
        m_errorMsg(),
        m_version(1.3) {}

  virtual void operator()(osg::GraphicsContext* gc) {
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);
    osg::GLExtensions* gl2ext = gc->getState()->get<osg::GLExtensions>();

    if (gl2ext) {
      if (!gl2ext->isGlslSupported) {
        m_supported = false;
        m_errorMsg = "ERROR: GLSL not supported by OpenGL driver.";
      } else
        m_version = gl2ext->glVersion;
    } else {
      m_supported = false;
      m_errorMsg = "ERROR: GLSL not supported.";
    }
  }

  OpenThreads::Mutex m_mutex;
  bool m_supported;
  std::string m_errorMsg;
  float m_version;
};

int main(int, char**) {
#ifdef _WIN32
  ::SetProcessDPIAware();
#endif

  osgViewer::Viewer viewer;
  viewer.setUpViewInWindow(100, 100, OSG_WIDTH, OSG_HEIGHT);

  // openGL version:
  osg::ref_ptr<TestSupportOperation> so = new TestSupportOperation;
  viewer.setRealizeOperation(so.get());
  viewer.realize();

  if (so->m_supported)
    std::cout << "GLVersion=" << so->m_version << std::endl;
  else
    std::cout << so->m_errorMsg << std::endl;

  return viewer.run();
}
