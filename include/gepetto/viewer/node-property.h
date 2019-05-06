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

#ifndef GEPETTO_VIEWER_NODE_PROPERTY_HH
#define GEPETTO_VIEWER_NODE_PROPERTY_HH

#include <map>
#include <iostream>

#ifndef Q_MOC_RUN
#include <boost/bind.hpp>
#include <boost/function.hpp>
#endif

#include <gepetto/viewer/fwd.h>
#include <gepetto/viewer/config-osg.h>

namespace gepetto {
namespace viewer {

    typedef std::map<std::string, PropertyPtr_t> PropertyMap_t;

    /// \cond
    namespace details {
      template <typename T> struct property_type {};
      template <typename T> struct property_type <const T > : property_type<T> {};
      template <typename T> struct property_type <     T& > : property_type<T> {};
      template <> struct property_type<bool         > { static inline std::string to_string () { return "bool"         ; } };
      template <> struct property_type<int          > { static inline std::string to_string () { return "int"          ; } };
      template <> struct property_type<float        > { static inline std::string to_string () { return "float"        ; } };
      template <> struct property_type<std::string  > { static inline std::string to_string () { return "string"       ; } };
      template <> struct property_type<unsigned long> { static inline std::string to_string () { return "unsigned long"; } };
      template <> struct property_type<osgVector2   > { static inline std::string to_string () { return "osgVector2"   ; } };
      template <> struct property_type<osgVector3   > { static inline std::string to_string () { return "osgVector3"   ; } };
      template <> struct property_type<osgVector4   > { static inline std::string to_string () { return "osgVector4"   ; } };
      template <> struct property_type<Configuration> { static inline std::string to_string () { return "Configuration"; } };
    }
    /// \endcond

    /// Abstract base class for runtime properties of Node.
    class Property {
      public:
        virtual bool set(const bool          & v) { invalidType(v); return false; }
        virtual bool set(const int           & v) { invalidType(v); return false; }
        virtual bool set(const float         & v) { invalidType(v); return false; }
        virtual bool set(const std::string   & v) { invalidType(v); return false; }
        virtual bool set(const unsigned long & v) { invalidType(v); return false; }
        virtual bool set(const osgVector2    & v) { invalidType(v); return false; }
        virtual bool set(const osgVector3    & v) { invalidType(v); return false; }
        virtual bool set(const osgVector4    & v) { invalidType(v); return false; }
        virtual bool set(const Configuration & v) { invalidType(v); return false; }

        virtual bool get(bool          & v) { invalidType(v); return false; }
        virtual bool get(int           & v) { invalidType(v); return false; }
        virtual bool get(float         & v) { invalidType(v); return false; }
        virtual bool get(std::string   & v) { invalidType(v); return false; }
        virtual bool get(unsigned long & v) { invalidType(v); return false; }
        virtual bool get(osgVector2    & v) { invalidType(v); return false; }
        virtual bool get(osgVector3    & v) { invalidType(v); return false; }
        virtual bool get(osgVector4    & v) { invalidType(v); return false; }
        virtual bool get(Configuration & v) { invalidType(v); return false; }

        virtual bool hasReadAccess () const = 0;
        virtual bool hasWriteAccess() const = 0;

        virtual std::string type() = 0;

        const std::string& name () const { return name_; }

      protected:
        Property(const std::string& name) : name_ (name) {}

        const std::string name_;

        template <typename T> inline void invalidType(T) const { throw std::invalid_argument ("Property " + name_ + " is not of type " + details::property_type<T>::to_string()); }
        inline void invalidGet() const { throw std::logic_error ("Cannot read property "  + name_ + "."); }
        inline void invalidSet() const { throw std::logic_error ("Cannot write property " + name_ + "."); }
    };

    template <typename T>
    class PropertyTpl : public Property {
      public:
        typedef boost::function<void(const T&)> Setter_t;
        typedef boost::function<  T (void)> Getter_t;
        typedef shared_ptr<PropertyTpl> Ptr_t;

        static Ptr_t create (const std::string& name, const Getter_t& g, const Setter_t& s) { return Ptr_t(new PropertyTpl(name, g, s)); }
        virtual std::string type() { return details::property_type<T>::to_string(); }

        template <typename Obj>
        static inline Getter_t getterFromMemberFunction(Obj* obj, const T& (Obj::*mem_func)() const) { return boost::bind(mem_func, obj); }
        template <typename Obj>
        static inline Getter_t getterFromMemberFunction(Obj* obj, T (Obj::*mem_func)() const) { return boost::bind(mem_func, obj); }
        template <typename Obj>
        static inline Setter_t setterFromMemberFunction(Obj* obj, void (Obj::*mem_func)(const T&)) { return boost::bind(mem_func, obj, _1); }
        template <typename Obj>
        static inline Setter_t setterFromMemberFunction(Obj* obj, void (Obj::*mem_func)(T)) { return boost::bind(mem_func, obj, _1); }

        PropertyTpl(const std::string& name, const Getter_t& g, const Setter_t& s)
          : Property(name), getter_(g), setter_(s) {}

        bool set(const T& value) { if (!hasWriteAccess()) { invalidSet(); return false; } setter_(value)   ; return true; }
        bool get(      T& value) { if (!hasReadAccess ()) { invalidGet(); return false; } value = getter_(); return true; }

        bool hasReadAccess  () const { return (bool)getter_; }
        bool hasWriteAccess () const { return (bool)setter_; }

        const Getter_t& getter () const { return getter_; }
        void getter (const Getter_t& g) { getter_ = g; }

        const Setter_t& setter () const { return setter_; }
        void setter (const Setter_t& s) { setter_ = s; }

      private:
        Getter_t getter_;
        Setter_t setter_;
    };

    typedef PropertyTpl<bool         > BoolProperty;
    typedef PropertyTpl<int          > IntProperty;
    typedef PropertyTpl<float        > FloatProperty;
    typedef PropertyTpl<std::string  > StringProperty;
    typedef PropertyTpl<unsigned long> UIntProperty;
    typedef PropertyTpl<osgVector2   > Vector2Property;
    typedef PropertyTpl<osgVector3   > Vector3Property;
    typedef PropertyTpl<osgVector4   > Vector4Property;
    typedef PropertyTpl<Configuration> ConfigurationProperty;

    /// Conversion between integer and enum name at runtime.
    struct MetaEnum {
      std::string type;
      std::vector<std::string> names;
      std::vector<        int> values;

      int from_string (const std::string& s);
      std::string to_string (const int& v);
    };

    MetaEnum* visibilityModeEnum ();
    MetaEnum* wireFrameModeEnum  ();
    MetaEnum* lightingModeEnum  ();
    MetaEnum* glImmediateModeEnum  ();

    class EnumProperty : public IntProperty {
      public:
        using IntProperty::Getter_t;
        using IntProperty::Setter_t;
        typedef shared_ptr<EnumProperty> Ptr_t;

        static Ptr_t create (const std::string& name, const MetaEnum* type, const Getter_t& g, const Setter_t& s) { return Ptr_t(new EnumProperty(name, type, g, s)); }
        virtual std::string type() { return "enum"; }
        const MetaEnum* metaEnum () const { return metaEnum_; }

        EnumProperty(const std::string& name, const MetaEnum* type, const Getter_t& g, const Setter_t& s)
          : IntProperty(name, g, s), metaEnum_ (type) {}

        /// Set the enum property.
        /// It also checks that \c value is a valid enum.
        bool set(const int& value);

      private:
        const MetaEnum* metaEnum_;
    };

    class Properties
    {
      protected:
        PropertyMap_t properties_;

        const PropertyPtr_t& property(const std::string& name) const;

        /// Called when a property is modified.
        virtual void setDirty (bool dirty=true) = 0;

      public:

        template <typename T>
        bool getProperty(const std::string& name, T& value) const
        {
          return property(name)->get(value);
        }

        template <typename T>
        bool setProperty(const std::string& name, const T& value)
        {
          bool res = property(name)->set(value);
          if (res) this->setDirty();
          return res;
        }

        bool hasProperty(const std::string& name) const;

        const PropertyMap_t& properties () const
        {
          return properties_;
        }

        void addProperty(const PropertyPtr_t& prop);

        void addProperty(const std::string& name, const PropertyPtr_t& prop);
    };

} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_NODE_PROPERTY_HH */
