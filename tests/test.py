import pinocchio as se3
import pinocchio.utils
import gepetto 

import numpy as np
import matrix as eigenpy
viewer=gepetto.RobotViewer()
viewer.start()
rob=viewer.addNewRobot()
rob.setUrdfPackageDirectory("/home/xeul/SRC/Pinocchio/nao_robot-master/")
rob.setUrdfFile("/home/xeul/SRC/Pinocchio/nao_robot-master/nao_description/urdf/naoV50_generated_urdf/nao.urdf")

rob.setDebugCollisionOnOff()
rob.setDebugCollisionOnOff()
jointid=rob.getVisuJointID("base_link_fixedjoint")
q=se3.SE3()
rob.setConfiguration(jointid,q)
