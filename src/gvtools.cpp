// Copyright (c) 2019, Joseph Mirabel
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

#include <osg/ArgumentParser>
#include <osg/LOD>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/Simplifier>

int main (int argc, char** argv)
{
  osg::ArgumentParser arguments(&argc, argv);
  osg::ApplicationUsage* au (arguments.getApplicationUsage());
  au->setCommandLineUsage(arguments.getApplicationName()+" [options]");

  au->addCommandLineOption("-i <file> or --input <file>", "Read file");
  au->addCommandLineOption("-s or --simplify", "Add to LOD node a simplified version of the input. Value must be ratio,mindist,maxdist");
  au->addCommandLineOption("-o <file> or --output <file>", "Write current LOD node to file.");

  osg::ApplicationUsage::Type help (arguments.readHelpType());

  std::string inputFilename, outputFilename;
  std::vector<float> ratios, minDists, maxDists;

  bool usage = (help != osg::ApplicationUsage::NO_HELP);
  if (!arguments.read("-i", inputFilename)
      && !arguments.read("--input", inputFilename)) {
    usage = true;
  }
  if (!arguments.read("-o", outputFilename)
      && !arguments.read("--output", outputFilename)) {
    usage = true;
  }
  std::string str;
  float ratio, minDist, maxDist;
  while (arguments.read("-s", str)
      || arguments.read("--simplify", str)) {
    if( sscanf( str.c_str(), "%f,%f,%f",
          &ratio, &minDist, &maxDist ) != 3 )
    {
      std::cerr << "Simplifier argument format incorrect." << std::endl;
      return 1;
    }
    ratios  .push_back(ratio  );
    minDists.push_back(minDist);
    maxDists.push_back(maxDist);
  }
  if (ratios.size() == 0) usage = true;

  arguments.reportRemainingOptionsAsUnrecognized(osg::ArgumentParser::BENIGN);
  if (arguments.errors(osg::ArgumentParser::CRITICAL)) {
    arguments.writeErrorMessages(std::cout);
  } else if (arguments.errors(osg::ArgumentParser::BENIGN)) {
    arguments.writeErrorMessages(std::cout);
  }

  if (usage)  {
    au->write(std::cout, help, 80, true);
    return 1;
  }

  osg::ref_ptr<osg::Node> input = osgDB::readNodeFile(inputFilename);
  osg::ref_ptr<osg::LOD> lod = new osg::LOD;
  osgUtil::Simplifier simplifier;

  for (std::size_t i = 0; i < ratios.size(); ++i) {
    std::cout << "Simplifying with " << ratios[i]
      << ", " << minDists[i]
      << ", " << maxDists[i] << std::endl;

    ratio = ratios[i];
    osg::ref_ptr<osg::Node> simplified;
    if (ratio == 1) {
      simplified = input;
    } else {
      simplifier.setSampleRatio(ratio);
      simplified = input->clone(osg::CopyOp::DEEP_COPY_ALL)->asNode();
      simplified->accept (simplifier);
    }
    lod->addChild (simplified, minDists[i], maxDists[i]);
  }

  osgDB::writeNodeFile (*lod, outputFilename);
  return 0;
}
