import pinocchio as se3
import pinocchio.utils
import gepetto 
import time

import numpy as np
import matrix as eigenpy
viewer=gepetto.RobotViewer()
viewer.start()
rob=viewer.addNewRobot()
rob.setUrdfPackageDirectory("/home/xeul/SRC/Pinocchio/nao_robot-master/")
rob.setUrdfFile("/home/xeul/SRC/Pinocchio/nao_robot-master/nao_description/urdf/naoV50_generated_urdf/nao.urdf")

rob.setDebugCollisionOnOff()
rob.setDebugCollisionOnOff()
#problem to get synchronisly the joint while previous steps not finished
time.sleep(1)
#jointid=-1 if model not finished loading
jointid=rob.getVisuJointID("LFinger12")

q=se3.SE3()
q.rotation=se3.utils.rpyToMatrix([1,0,0])

rob.setConfiguration(jointid,q)
