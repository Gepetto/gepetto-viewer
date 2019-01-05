/// \mainpage
/// \section gepetto_viewer_sec_intro Introduction
///
/// This package implements a library built on Open Scene Graph to display
/// various geometric primitives.
///
/// The library is able to parse urdf files and to create objects with
/// each link of the robot.
/// Basic geometric primitives can also be built (boxes, cylinders,
/// capsules,...).
/// See graphics::Node inheritance diagram for a list of handled 3D objects.
///
/// As in OpenSceneGraph, objects are organized in trees. This package does
/// not implement a kinematic chain. Each object must be placed separately
/// in its parent frame.
///
/// The main entry points are:
/// \li class graphics::Node:           \copybrief graphics::Node
/// \li class graphics::WindowManager:  \copybrief graphics::WindowManager
/// \li class graphics::WindowsManager: \copybrief graphics::WindowsManager
