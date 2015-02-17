//
//  urdf-parser.cpp
//  gepetto-viewer
//
//  Created by J.Valentin in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//
#include <gepetto/viewer/urdf-parser.h>
#include <urdf_parser/urdf_parser.h>

namespace graphics {




Robot* urdfParser::parse (const std::string& robotName,
    const std::string& urdf_file_path,
    const std::string& meshDataRootDir  )
    {


        boost::shared_ptr< urdf::ModelInterface > model =
        urdf::parseURDFFile( urdf_file_path );
        // Test that file has correctly been parsed
        if (!model)
        {
            throw std::runtime_error (std::string ("Failed to parse ") +
            urdf_file_path);
        }
        Robot*rob = new Robot();
        rob->setUrdfPackageRootDirectory(meshDataRootDir);
        std::vector< boost::shared_ptr < urdf::Link > > links;
        //std::vector< boost::shared_ptr < urdf::Joint > > joints;
        model->getLinks(links);
        //model->getJoints(joints);
        std::string link_name;


        for (unsigned int i = 0 ; i < links.size() ; i++)
        {
            link_name = links[i]->name;
            rob->parse(links[i]);

        }
        return rob  ;
    }
    }
