// Copyright (c) 2017, LAAS-CNRS
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of hpp-core.
// hpp-core is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// hpp-core is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// hpp-core. If not, see <http://www.gnu.org/licenses/>.

#ifndef SCENEVIEWER_NODE_PROPERTY_HH
#define SCENEVIEWER_NODE_PROPERTY_HH

#include <map>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <gepetto/viewer/config-osg.h>

namespace graphics {
    
    DEF_CLASS_SMART_PTR(Property)
    typedef std::map<std::string, PropertyPtr_t> PropertyMap_t;

    class Property {
      public:
        virtual bool set(const int              & /*value*/) { invalidType("int"          ); return false; }
        virtual bool set(const float            & /*value*/) { invalidType("float"        ); return false; }
        virtual bool set(const std::string      & /*value*/) { invalidType("string"       ); return false; }
        virtual bool set(const unsigned long    & /*value*/) { invalidType("unsigned long"); return false; }
        virtual bool set(const osgVector2       & /*value*/) { invalidType("osgVector2"   ); return false; }
        virtual bool set(const osgVector3       & /*value*/) { invalidType("osgVector3"   ); return false; }
        virtual bool set(const osgVector4       & /*value*/) { invalidType("osgVector4"   ); return false; }

        virtual bool get(int              & /*value*/) { invalidType("int"          ); return false; }
        virtual bool get(float            & /*value*/) { invalidType("float"        ); return false; }
        virtual bool get(std::string      & /*value*/) { invalidType("string"       ); return false; }
        virtual bool get(unsigned long    & /*value*/) { invalidType("unsigned long"); return false; }
        virtual bool get(osgVector2       & /*value*/) { invalidType("osgVector2"   ); return false; }
        virtual bool get(osgVector3       & /*value*/) { invalidType("osgVector3"   ); return false; }
        virtual bool get(osgVector4       & /*value*/) { invalidType("osgVector4"   ); return false; }

        virtual bool hasReadAccess () const = 0;
        virtual bool hasWriteAccess() const = 0;

        const std::string& name () const { return name_; }

      protected:
        Property(const std::string& name) : name_ (name) {}

        const std::string name_;

        inline void invalidType(const std::string& t) const { throw std::invalid_argument ("Property " + name_ + " is not of type " + t); }
        inline void invalidGet() const { throw std::logic_error ("Cannot read property "  + name_ + "."); }
        inline void invalidSet() const { throw std::logic_error ("Cannot write property " + name_ + "."); }
    };

    template <typename T>
    class PropertyTpl : public Property {
      public:
        typedef boost::function<void(const T&)> Setter_t;
        typedef boost::function<const T&(void)> Getter_t;
        typedef boost::shared_ptr<PropertyTpl> Ptr_t;

        static Ptr_t create (const std::string& name, const Getter_t& g, const Setter_t& s) { return Ptr_t(new PropertyTpl(name, g, s)); }

        template <typename Obj>
        static Getter_t getterFromMemberFunction(Obj* obj, const T& (mem_func)(Obj*)) { return boost::bind(mem_func, obj); }
        template <typename Obj>
        static Setter_t setterFromMemberFunction(Obj* obj, void (Obj::*mem_func)(const T&)) { return boost::bind(mem_func, obj, _1); }

        PropertyTpl(const std::string& name, const Getter_t& g, const Setter_t& s)
          : Property(name), getter_(g), setter_(s) {}

        bool set(const T& value) { if (!hasWriteAccess()) { invalidSet(); return false; } setter_(value)   ; return true; }
        bool get(      T& value) { if (!hasReadAccess ()) { invalidGet(); return false; } value = getter_(); return true; }

        bool hasReadAccess  () const { return (bool)getter_; }
        bool hasWriteAccess () const { return (bool)setter_; }

      private:
        const Getter_t getter_;
        const Setter_t setter_;
    };

    typedef PropertyTpl<int          > IntProperty;
    typedef PropertyTpl<float        > FloatProperty;
    typedef PropertyTpl<std::string  > StringProperty;
    typedef PropertyTpl<unsigned long> UIntProperty;
    typedef PropertyTpl<osgVector2   > Vector2Property;
    typedef PropertyTpl<osgVector3   > Vector3Property;
    typedef PropertyTpl<osgVector4   > Vector4Property;

    
} /* namespace graphics */

#endif /* SCENEVIEWER_NODE_PROPERTY_HH */
