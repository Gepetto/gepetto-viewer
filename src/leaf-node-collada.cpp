//
//  leaf-node-collada.cpp
//  gepetto-viewer
//
//  Created by Anthony Couret, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <sys/stat.h>
#include <fstream>
#include <clocale>
#include <ios>
#include <osg/LightModel>
#include <osg/Texture2D>
#include <osg/Version>
#include <osgDB/FileNameUtils>
#include <gepetto/viewer/leaf-node-collada.h>

namespace gepetto {
namespace viewer {
  inline bool fileExists (const char* fn)
  { struct stat buffer; return (stat (fn, &buffer) == 0); }

  std::string getCachedFileName (const std::string& meshfile)
  {
    static const std::string exts[3] = { ".osgb", ".osg2", ".osg" };
    for (int i = 0; i < 3; ++i) {
      std::string cached = meshfile + exts[i];
      if (fileExists(cached.c_str())) return cached;
    }
    return std::string();
  }

  bool getBackfaceDrawing (LeafNodeCollada* node) {
    return (bool)(node->getColladaPtr()->getOrCreateStateSet()
        ->getMode(GL_CULL_FACE) & osg::StateAttribute::ON);
  }
  void setBackfaceDrawing (LeafNodeCollada* node, bool on) {
    osg::StateSet* ss = node->getColladaPtr()->getOrCreateStateSet();

    ss->setMode(GL_CULL_FACE,
          (on ?  osg::StateAttribute::ON : osg::StateAttribute::OFF));

    if (on) {
      osg::LightModel* ltModel = new osg::LightModel;
      ltModel->setTwoSided(on);
      ss->setAttribute(ltModel);
      ss->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
    } else {
      ss->removeAttribute(osg::StateAttribute::LIGHTMODEL);
      ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
    }
  }

  class LightSourceRemoving : public osg::NodeVisitor
  {
    public:
      LightSourceRemoving () : NodeVisitor (TRAVERSE_ALL_CHILDREN) {}

      void apply(osg::LightSource& node)
      {
        osg::Group* group = new osg::Group (node);
        for (unsigned int i = 0; i < node.getNumParents(); ++i)
          node.getParent(i)->replaceChild (&node, group);
      }
  };

#if OSG_VERSION_LESS_THAN(3,3,3)
  struct ObjectCache {
    typedef std::map<std::string, osg::NodeRefPtr> Map_t;
    Map_t map_;
    bool get (const std::string& name, osg::NodeRefPtr& node) const
    {
      Map_t::const_iterator it = map_.find (name);
      if (it != map_.end()) {
        node = it->second;
        return true;
      }
      return false;
    } 
    void add (const std::string& name, osg::NodeRefPtr& node)
    {
      map_.insert (std::make_pair(name,node));
    }
    void erase (const std::string& name)
    {
      map_.erase(name);
    }
  };

  ObjectCache object_cache;
#else
  osg::ref_ptr<osgDB::ObjectCache> object_cache (new osgDB::ObjectCache);
#endif

  /* Declaration of private function members */

  void LeafNodeCollada::init()
  {
    group_ptr_ = new osg::Group;
    group_ptr_->setName ("groupForMaterial");

#if OSG_VERSION_LESS_THAN(3,3,3)
    if (!collada_ptr_)
      object_cache.get(collada_file_path_, collada_ptr_);
#endif

    if (!collada_ptr_) {
      // Setup cache
      const osg::ref_ptr<osgDB::Options> options = new osgDB::Options();
#if OSG_VERSION_GREATER_OR_EQUAL(3,3,3)
      options->setObjectCache (object_cache);
#endif
      options->setObjectCacheHint(osgDB::Options::CACHE_ALL);

      if (!fileExists(collada_file_path_.c_str()))
        throw std::invalid_argument(std::string("File ") + collada_file_path_ + std::string(" not found."));

      std::string osgname = getCachedFileName(collada_file_path_);
      if (!osgname.empty()) {
        std::cout << "Using " << osgname << std::endl;
        collada_ptr_ = osgDB::readNodeFile(osgname, options);
      } else {
        // get the extension of the meshs file
        std::string ext = osgDB::getLowerCaseFileExtension(collada_file_path_);
        if(ext == "obj"){
          options->setOptionString("noRotation");
          collada_ptr_ = osgDB::readNodeFile(collada_file_path_,options);
        }
        else if (ext == "dae") {
          float scale = 1.f;
          options->setPluginData("DAE-AssetUnitMeter", &scale);
          if (*localeconv()->decimal_point != '.') {
            std::cerr << "Warning: your locale convention uses '"
              << localeconv()->decimal_point << "' as decimal separator while DAE "
              "expects '.'.\nSet LC_NUMERIC to a locale convetion using '.' as "
              "decimal separator (e.g. export LC_NUMERIC=\"en_US.utf-8\")."
              << std::endl;
          }

          collada_ptr_ = osgDB::readNodeFile(collada_file_path_,options);

          // FIXME: Fixes https://github.com/Gepetto/gepetto-viewer/issues/95
          // The bug: Assimp seems to ignore the DAE up_axis tag. Because this
          // cannot be fixed in assimp without a huge impact, we make GV
          // compatible with assimp.
          //
          // The fix: OSG DAE plugin rotates the actual model with a root
          // PositionAttitudeTransform, when the DAE up axis is not Z.
          // We simply reset the attitude.
          osg::PositionAttitudeTransform* pat =
            dynamic_cast<osg::PositionAttitudeTransform*> (collada_ptr_.get());
          if (pat != NULL) {
            std::cout << "Reset up_axis to Z_UP." << std::endl;
            pat->setAttitude (osgQuat());
          }

          bool error = false;
          if (!collada_ptr_) {
            std::cout << "File: " << collada_file_path_ << " could not be loaded\n";
            error = true;
          } else if (strncasecmp(collada_ptr_->getName().c_str(), "empty", 5) == 0) {
            std::cout << "File: " << collada_file_path_ << " could not be loaded:\n"
              << collada_ptr_->getName() << '\n';
            error = true;
          }
          if (error) {
            std::cout << "You may try to convert the file with the following command:\n"
              "osgconv " << collada_file_path_ << ' ' << collada_file_path_ << ".osgb" << std::endl;
          }
          // Apply scale
          if (scale != 1.) {
            osg::ref_ptr<osg::MatrixTransform> xform = new osg::MatrixTransform;
            xform->setDataVariance( osg::Object::STATIC );
            xform->setMatrix( osg::Matrix::scale( scale, scale, scale ) );
            xform->addChild( collada_ptr_ );
            collada_ptr_ = xform;
          }
        } else
          collada_ptr_ = osgDB::readNodeFile(collada_file_path_,options);
      }
      if (!collada_ptr_)
        throw std::invalid_argument(std::string("File ") + collada_file_path_ + std::string(" found but could not be opened. Check that a plugin exist."));
#if OSG_VERSION_LESS_THAN(3,3,3)
      object_cache.add(collada_file_path_, collada_ptr_);
#endif

      /* Allow transparency */
      collada_ptr_->getOrCreateStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);
      collada_ptr_->setDataVariance(osg::Object::STATIC);
    }
    collada_ptr_->setName ("meshfile");

    /* Create PositionAttitudeTransform */
    group_ptr_->addChild(collada_ptr_);
    this->asQueue()->addChild(group_ptr_);

    addProperty(StringProperty::create("Mesh file",
          StringProperty::getterFromMemberFunction(this, &LeafNodeCollada::meshFilePath),
          StringProperty::Setter_t()));
    addProperty(BoolProperty::create("BackfaceDrawing",
          EnumProperty::Getter_t(boost::bind(getBackfaceDrawing, this)),
          EnumProperty::Setter_t(boost::bind(setBackfaceDrawing, this, _1))));
    addProperty(StringProperty::create("Texture file",
          StringProperty::getterFromMemberFunction(this, &LeafNodeCollada::textureFilePath),
          StringProperty::Setter_t()));
    addProperty(Vector4Property::create("Color",
          Vector4Property::getterFromMemberFunction(this, &LeafNodeCollada::getColor),
          Vector4Property::setterFromMemberFunction(this, &LeafNodeCollada::setColor)));
    addProperty(VoidProperty::create("RemoveLightSources",
          VoidProperty::memberFunction(this, &LeafNodeCollada::removeLightSources)));
  }
    
  LeafNodeCollada::LeafNodeCollada(const std::string& name, const std::string& collada_file_path) :
    Node(name), collada_file_path_(collada_file_path)
  {
    init();
  }
    
  LeafNodeCollada::LeafNodeCollada(const std::string& name, const std::string& collada_file_path, const osgVector4& color) :
    Node(name), collada_file_path_(collada_file_path)
  {
    init();
    setColor(color);
  }

  LeafNodeCollada::LeafNodeCollada(const std::string& name,
      const ::osg::NodeRefPtr& node, const std::string& collada_file_path) :
    Node(name), collada_file_path_(collada_file_path), collada_ptr_ (node)
  {
    init();
  }
  
  LeafNodeCollada::LeafNodeCollada(const LeafNodeCollada& other) :
    Node(other.getID()), collada_file_path_(other.collada_file_path_)
  {
    init();
  }
    
  void LeafNodeCollada::initWeakPtr(LeafNodeColladaWeakPtr other_weak_ptr)
  {
    weak_ptr_ = other_weak_ptr;
  }
    
  /* End of declaration of private function members */
    
  /* Declaration of protected function members */
    
  LeafNodeColladaPtr_t LeafNodeCollada::create(const std::string& name, const std::string& collada_file_path)
  {
    std::ifstream infile(collada_file_path.c_str ());
    if (!infile.good()) {
      throw std::ios_base::failure (collada_file_path +
                                    std::string (" does not exist."));
    }
    LeafNodeColladaPtr_t shared_ptr(new LeafNodeCollada
                                    (name, collada_file_path));
    
    // Add reference to itself
    shared_ptr->initWeakPtr(shared_ptr);
    
    return shared_ptr;
  }

  LeafNodeColladaPtr_t LeafNodeCollada::create(const std::string& name, ::osg::NodeRefPtr mesh, const std::string& collada_file_path)
  {
    LeafNodeColladaPtr_t shared_ptr(new LeafNodeCollada
                                    (name, mesh, collada_file_path));
    
    // Add reference to itself
    shared_ptr->initWeakPtr(shared_ptr);
    
    return shared_ptr;
  }

    LeafNodeColladaPtr_t LeafNodeCollada::create(const std::string& name, const std::string& collada_file_path, const osgVector4& color)
  {
    std::ifstream infile(collada_file_path.c_str ());
    if (!infile.good()) {
      throw std::ios_base::failure (collada_file_path +
                                    std::string (" does not exist."));
    }
    LeafNodeColladaPtr_t shared_ptr(new LeafNodeCollada
                                    (name, collada_file_path, color));
    
    // Add reference to itself
    shared_ptr->initWeakPtr(shared_ptr);
    
    return shared_ptr;
  }
    
  LeafNodeColladaPtr_t LeafNodeCollada::createCopy(LeafNodeColladaPtr_t other)
  {
    LeafNodeColladaPtr_t shared_ptr(new LeafNodeCollada(*other));
        
    // Add reference to itself
    shared_ptr->initWeakPtr(shared_ptr);
        
    return shared_ptr;
  }

  ::osg::NodeRefPtr LeafNodeCollada::getColladaPtr()
  {
    return collada_ptr_.get();
  }

  /* End of declaration of protected function members */
    
  /* Declaration of public function members */
    
  LeafNodeColladaPtr_t LeafNodeCollada::clone(void) const
  {
    return LeafNodeCollada::createCopy(weak_ptr_.lock());
  }
    
  LeafNodeColladaPtr_t LeafNodeCollada::self(void) const
  {
    return weak_ptr_.lock();
  }
    
  void LeafNodeCollada::setColor(const osgVector4& color)
  {
    osg::ref_ptr<osg::Material> mat_ptr (new osg::Material); 
    osgVector4 ambient (color.r() * 0.5f, color.g() * 0.5f, color.b() * 0.5f, color.a());

    mat_ptr->setDiffuse (osg::Material::FRONT_AND_BACK,color); 
    mat_ptr->setAmbient (osg::Material::FRONT_AND_BACK,ambient); 

    group_ptr_->getOrCreateStateSet()->setAttribute(mat_ptr.get());    
    setTransparentRenderingBin (color[3] < Node::TransparencyRenderingBinThreshold);
    setDirty();
  }
    
  osgVector4 LeafNodeCollada::getColor() const
  {
    osg::StateSet* ss = group_ptr_->getStateSet();
    if (ss) {
      osg::Material *mat = dynamic_cast<osg::Material*>
        (ss->getAttribute(osg::StateAttribute::MATERIAL));
      if (mat) return mat->getDiffuse(osg::Material::FRONT_AND_BACK);
    }
    return osgVector4();
  }

  void LeafNodeCollada::setAlpha(const float& alpha)
  {
    // TODO this overload is probably not necessary.
    osg::StateSet* ss = group_ptr_->getStateSet();
    if (ss)
      {
	alpha_ = alpha;
	osg::Material *mat;
	if (ss->getAttribute(osg::StateAttribute::MATERIAL))
	  mat = dynamic_cast<osg::Material*>(ss->getAttribute(osg::StateAttribute::MATERIAL));
	else
	  {
	    mat = new osg::Material;
	    ss->setAttribute(mat, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	  }
	mat->setTransparency(osg::Material::FRONT_AND_BACK, alpha);
	if (alpha == 0)
	  ss->setRenderingHint(osg::StateSet::DEFAULT_BIN);
	else
          ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        setDirty();
      }
  }
 
  void LeafNodeCollada::setTexture(const std::string& image_path)
  {
    texture_file_path_ = image_path;
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setDataVariance(osg::Object::STATIC); 
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(image_path);
    if (!image)
    {
      std::cout << " couldn't find texture, quiting." << std::endl;
      return;
    } 
    texture->setImage(image);
    collada_ptr_->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
    setDirty();
  }

  const std::string& LeafNodeCollada::meshFilePath () const
  {
    return collada_file_path_;
  }

  const std::string& LeafNodeCollada::textureFilePath () const
  {
    return texture_file_path_;
  }

  /*void LeafNodeCollada::setColor(osg::NodeRefPtr osgNode_ptr,const osgVector4& color)
  {
    osg::Vec4ArrayRefPtr colorArray = new osg::Vec4Array();
    colorArray->push_back(color);
    osg::GeodeRefPtr geode_ptr = osgNode_ptr->asGeode();
    if (geode_ptr) {
      for (unsigned int i = 0 ; i < geode_ptr->getNumDrawables() ; i++) {
      osg::GeometryRefPtr geom_ptr = geode_ptr->getDrawable(i)->asGeometry();
        if (geom_ptr) {
          geom_ptr->setColorArray(colorArray.get());
          geom_ptr->setColorBinding(osg::Geometry::BIND_OVERALL); 
        }
      }
    }
    else {
      osg::GroupRefPtr group_ptr = osgNode_ptr->asGroup();
      if (group_ptr) {
        for (unsigned int i = 0 ; i < group_ptr->getNumChildren() ; i++) {
          setColor(group_ptr->getChild(i),color);
        }
      }
    }
  }*/

  osg::ref_ptr<osg::Node> LeafNodeCollada::getOsgNode() const
  {
    return collada_ptr_;
  }

  void LeafNodeCollada::removeLightSources ()
  {
    LightSourceRemoving lsRemoving;
    collada_ptr_->traverse (lsRemoving);
  }

  LeafNodeCollada::~LeafNodeCollada()
  {
    /* Proper deletion of all tree scene */
    this->asQueue()->removeChild(collada_ptr_);

    if (collada_ptr_->referenceCount() == 2) {
      // If the object is only referenced by this node and the cache,
      // remove it from the cache.
#if OSG_VERSION_LESS_THAN(3,3,3)
      object_cache.erase(collada_file_path_);
#else
      object_cache->removeFromObjectCache(collada_file_path_);
#endif
    }

    collada_ptr_ = NULL;
        
    weak_ptr_.reset();
  }
    
  /* End of declaration of public function members */
    
} /* namespace viewer */

} /* namespace gepetto */
