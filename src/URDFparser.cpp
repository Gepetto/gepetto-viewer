//
//  URDFparser.cpp
//  SceneViewer
//
//  Created by Anthony Couret, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <Graphics/URDFparser.h>

namespace Graphics {
GroupNodePtr_t URDFparser::parse(const std::string& robotName, const std::string& urdf_file_path, const std::string& package_path)
{
    boost::shared_ptr< urdf::ModelInterface > model = urdf::parseURDFFile( urdf_file_path );
    GroupNodePtr_t robot = GroupNode::create(robotName);
    std::vector< boost::shared_ptr < urdf::Link > > links;
    model->getLinks(links);

    std::string link_name;
    std::string mesh_path;
    ::boost::shared_ptr< ::urdf::Mesh > mesh_shared_ptr;
    for (unsigned int i = 0 ; i < links.size() ; i++) {
        if ( links[i]->visual != NULL && links[i]->visual->geometry != NULL && links[i]->visual->geometry->type == urdf::Geometry::MESH ) {
            mesh_shared_ptr = ::boost::static_pointer_cast< ::urdf::Mesh > ( links[i]->visual->geometry );
            link_name = links[i]->name;
            std::cout << link_name << std::endl;
            if ( mesh_shared_ptr != 0 ) {
                mesh_path =  mesh_shared_ptr->filename.substr(10, mesh_shared_ptr->filename.size());
                LeafNodeColladaPtr_t link = LeafNodeCollada::create( robotName + "/" + link_name, package_path + mesh_path);
                // Set staticTransform = transform from link to visual
                osgVector3 static_pos(links[i]->visual->origin.position.x, links[i]->visual->origin.position.y, links[i]->visual->origin.position.z);
                osgQuat static_quat(links[i]->visual->origin.rotation.x, links[i]->visual->origin.rotation.y, links[i]->visual->origin.rotation.z, links[i]->visual->origin.rotation.w);
                link->setStaticTransform(static_pos,static_quat);

                // add links to robot node
                robot->addChild(link);
            }
        }
    }

    return robot;
}
}
