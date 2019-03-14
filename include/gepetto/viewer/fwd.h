//
//  node.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_FWD_HH
#define GEPETTO_VIEWER_FWD_HH

#include <gepetto/viewer/macros.h>

namespace gepetto {
namespace viewer {
    
    class Configuration;
    DEF_CLASS_SMART_PTR(Node)
    DEF_CLASS_SMART_PTR(GroupNode)

    DEF_CLASS_SMART_PTR(Property)

    DEF_CLASS_SMART_PTR(WindowManager)
    DEF_CLASS_SMART_PTR(RoadmapViewer)
    
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_FWD_HH */
