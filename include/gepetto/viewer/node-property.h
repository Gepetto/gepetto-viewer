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
#include <boost/mpl/if.hpp>
#endif

#include <QWidget>

#include <gepetto/viewer/fwd.h>
#include <gepetto/viewer/config-osg.h>

namespace gepetto {
namespace viewer {

    typedef std::map<std::string, PropertyPtr_t> PropertyMap_t;
    class Property;
    template <typename T> class PropertyTpl;
    template <typename T, typename RangeT = T> class RangedPropertyTpl;

    /// \cond
    namespace details {
      template <typename T> struct property_type {};
      template <typename T> struct property_type <const T > : property_type<T> {};
      template <typename T> struct property_type <     T& > : property_type<T> {};
      template <> struct property_type<void         > { static inline std::string to_string () { return "void"         ; } };
      template <> struct property_type<bool         > { static inline std::string to_string () { return "bool"         ; } };
      template <> struct property_type<int          > { static inline std::string to_string () { return "int"          ; } };
      template <> struct property_type<float        > { static inline std::string to_string () { return "float"        ; } };
      template <> struct property_type<std::string  > { static inline std::string to_string () { return "string"       ; } };
      template <> struct property_type<osgVector2   > { static inline std::string to_string () { return "osgVector2"   ; } };
      template <> struct property_type<osgVector3   > { static inline std::string to_string () { return "osgVector3"   ; } };
      template <> struct property_type<osgVector4   > { static inline std::string to_string () { return "osgVector4"   ; } };
      template <> struct property_type<Configuration> { static inline std::string to_string () { return "Configuration"; } };

      template <typename T> QWidget* buildEditor (Property* property) { return NULL; }
      template <> QWidget* buildEditor<bool         > (Property* property);
      template <> QWidget* buildEditor<int          > (Property* property);
      template <> QWidget* buildEditor<float        > (Property* property);
      template <> QWidget* buildEditor<std::string  > (Property* property);
      template <> QWidget* buildEditor<osgVector2   > (Property* property);
      template <> QWidget* buildEditor<osgVector3   > (Property* property);
      template <> QWidget* buildEditor<osgVector4   > (Property* property);
      template <> QWidget* buildEditor<Configuration> (Property* property);
    }
    /// \endcond

    /// Abstract base class for runtime properties of Node.
    class Property : public QObject {
      Q_OBJECT

      public slots:
        bool set(      void             );
        bool set(const bool          & v);
        bool set(const int           & v);
        bool set(const float         & v);
        bool set(const std::string   & v);
        bool set(const osgVector2    & v);
        bool set(const osgVector3    & v);
        bool set(const osgVector4    & v);
        bool set(const Configuration & v);

        // Provide slots to convert from double to float.
        bool set(const double& v);

        // Provide slots to convert from QString to std::string.
        bool set(const QString& v);

        // Provide slots to convert from QColor to osgVector4.
        bool set(const QColor& v);

      public:
        bool get(void             );
        bool get(bool          & v);
        bool get(int           & v);
        bool get(float         & v);
        bool get(std::string   & v);
        bool get(osgVector2    & v);
        bool get(osgVector3    & v);
        bool get(osgVector4    & v);
        bool get(Configuration & v);

        // Provide slots to convert from double to float.
        bool get(double      & v);
        // Provide slots to convert from QString to std::string.
        bool get(QString      & v);
        // Provide slots to convert from QColor to osgVector4.
        bool get(QColor      & v);

      signals:
        void valueChanged(      void             );
        void valueChanged(const bool          & v);
        void valueChanged(const int           & v);
        void valueChanged(const float         & v);
        void valueChanged(const std::string   & v);
        void valueChanged(const osgVector2    & v);
        void valueChanged(const osgVector3    & v);
        void valueChanged(const osgVector4    & v);
        void valueChanged(const Configuration & v);

        // Provide slots to convert from double to float.
        void valueChanged(const double& v);
        // Provide slots to convert from QString to std::string
        void valueChanged(const QString& v);
        // Provide slots to convert from QColor to osgVector4
        void valueChanged(const QColor& v);

      public:
        virtual bool hasReadAccess () const = 0;
        virtual bool hasWriteAccess() const = 0;

        virtual std::string type() = 0;

        const std::string& name () const { return name_; }

        /// \return NULL is this property is not editable, otherwise it returns
        ///         a valid new QWidget.
        virtual QWidget* guiEditor ()
        {
          return NULL;
        }

      protected:
        virtual bool impl_set(      void             );
        virtual bool impl_set(const bool          & v);
        virtual bool impl_set(const int           & v);
        virtual bool impl_set(const float         & v);
        virtual bool impl_set(const std::string   & v);
        virtual bool impl_set(const osgVector2    & v);
        virtual bool impl_set(const osgVector3    & v);
        virtual bool impl_set(const osgVector4    & v);
        virtual bool impl_set(const Configuration & v);

        virtual bool impl_get(void             );
        virtual bool impl_get(bool          & v);
        virtual bool impl_get(int           & v);
        virtual bool impl_get(float         & v);
        virtual bool impl_get(std::string   & v);
        virtual bool impl_get(osgVector2    & v);
        virtual bool impl_get(osgVector3    & v);
        virtual bool impl_get(osgVector4    & v);
        virtual bool impl_get(Configuration & v);

      protected:
        Property(const std::string& name);

        virtual ~Property() {}

        const std::string name_;

        inline void invalidGet() const { throw std::logic_error ("Cannot read property "  + name_ + "."); }
        inline void invalidSet() const { throw std::logic_error ("Cannot write property " + name_ + "."); }
    };

    class VoidProperty : public Property {
      public:
        typedef boost::function<void (void)> Function_t;
        typedef shared_ptr<VoidProperty> Ptr_t;

        static Ptr_t create (const std::string& name, const Function_t& f) { return Ptr_t(new VoidProperty(name, f)); }
        virtual std::string type() { return details::property_type<void>::to_string(); }

        template <typename Obj, typename ReturnType>
        static inline Function_t memberFunction(Obj* obj, ReturnType (Obj::*mem_func)()      ) { return boost::bind(mem_func, obj); }
        template <typename Obj, typename ReturnType>
        static inline Function_t memberFunction(Obj* obj, ReturnType (Obj::*mem_func)() const) { return boost::bind(mem_func, obj); }

        VoidProperty(const std::string& name, const Function_t& f)
          : Property(name), function_(f) {}

        virtual ~VoidProperty() {}

        bool hasReadAccess  () const { return (bool)function_; }
        bool hasWriteAccess () const { return hasReadAccess(); }

        const Function_t& function () const { return function_; }
        void function (const Function_t& f) { function_ = f; }

        QWidget* guiEditor ();

      protected:
        bool impl_get(void) { if (!hasReadAccess ()) { invalidGet(); return false; } function_(); return true; }
        bool impl_set(void) { return get(); }

      private:
        Function_t function_;
    };

    template <typename T>
    class PropertyTpl : public Property {
      public:
        typedef boost::function<void(const T&)> Setter_t;
        typedef boost::function<  T (void)> Getter_t;
        typedef shared_ptr<PropertyTpl> Ptr_t;

        static Ptr_t create (const std::string& name, const Getter_t& g, const Setter_t& s) { return Ptr_t(new PropertyTpl(name, g, s)); }
        static Ptr_t create (const std::string& name, const Getter_t& g) { return Ptr_t(new PropertyTpl(name, g, Setter_t())); }
        static Ptr_t create (const std::string& name, const Setter_t& s) { return Ptr_t(new PropertyTpl(name, Getter_t(), s)); }

        virtual std::string type() { return details::property_type<T>::to_string(); }

        template <typename Obj>
        static inline Getter_t getterFromMemberFunction(Obj* obj, const T& (Obj::*mem_func)() const) { return boost::bind(mem_func, obj); }
        template <typename Obj>
        static inline Getter_t getterFromMemberFunction(Obj* obj, T (Obj::*mem_func)() const) { return boost::bind(mem_func, obj); }
        template <typename Obj>
        static inline Setter_t setterFromMemberFunction(Obj* obj, void (Obj::*mem_func)(const T&)) { return boost::bind(mem_func, obj, _1); }
        template <typename Obj>
        static inline Setter_t setterFromMemberFunction(Obj* obj, void (Obj::*mem_func)(T)) { return boost::bind(mem_func, obj, _1); }

        template <typename Obj, typename RetType>
        static Ptr_t create (const std::string& name, Obj* obj,
            RetType (Obj::*mem_get)() const,
            void (Obj::*mem_set)(const T&))
        {
          return create (name, Getter_t(boost::bind(mem_get, obj)),
              Setter_t(boost::bind(mem_set, obj, _1)));
        }
        template <typename Obj, typename RetType>
        static Ptr_t create (const std::string& name, Obj* obj,
            RetType (Obj::*mem_get)() const,
            void (Obj::*mem_set)(T))
        {
          return create (name, Getter_t(boost::bind(mem_get, obj)),
              Setter_t(boost::bind(mem_set, obj, _1)));
        }
        template <typename Obj, typename RetType>
        static Ptr_t createRO (const std::string& name, Obj* obj,
            RetType (Obj::*mem_get)() const)
        {
          return create (name, Getter_t(boost::bind(mem_get, obj)));
        }
        template <typename Obj>
        static Ptr_t createWO (const std::string& name, Obj* obj,
            void (Obj::*mem_set)(const T&))
        {
          return create (name, Setter_t(boost::bind(mem_set, obj, _1)));
        }
        template <typename Obj, typename RetType>
        static Ptr_t createWO (const std::string& name, Obj* obj,
            void (Obj::*mem_set)(T))
        {
          return create (name, Setter_t(boost::bind(mem_set, obj, _1)));
        }


        PropertyTpl(const std::string& name, const Getter_t& g, const Setter_t& s)
          : Property(name), getter_(g), setter_(s) {}

        virtual ~PropertyTpl() {}

        bool hasReadAccess  () const { return (bool)getter_; }
        bool hasWriteAccess () const { return (bool)setter_; }

        const Getter_t& getter () const { return getter_; }
        void getter (const Getter_t& g) { getter_ = g; }

        const Setter_t& setter () const { return setter_; }
        void setter (const Setter_t& s) { setter_ = s; }

        virtual QWidget* guiEditor ()
        {
          return details::buildEditor<T>(this);
        }

      protected:
        virtual bool impl_set(const T& value) { if (!hasWriteAccess()) { invalidSet(); return false; } setter_(value)   ; return true; }
        virtual bool impl_get(      T& value) { if (!hasReadAccess ()) { invalidGet(); return false; } value = getter_(); return true; }

      private:
        Getter_t getter_;
        Setter_t setter_;
    };

    template<typename Scalar>
    struct Range {
      Scalar min, max, step;
      bool adaptiveDecimal;

      Range () :
#if __cplusplus >= 201103L
        min(std::numeric_limits<Scalar>::lowest()),
#else
        min(std::numeric_limits<Scalar>::quiet_NaN()),
#endif
        max(std::numeric_limits<Scalar>::max()),
        step (static_cast<Scalar>(1)),
        adaptiveDecimal (false)
      {}

#if __cplusplus >= 201103L
      inline bool hasMin () const { return min > std::numeric_limits<Scalar>::lowest(); }
#else
      inline bool hasMin () const { return min == min; }
#endif
      inline bool hasMax () const { return max < std::numeric_limits<Scalar>::max(); }
      inline bool hasRange() const { return hasMin() && hasMax(); }

      void setRange(const Scalar& minimum, const Scalar& maximum)
      { min = minimum; max = maximum; }
      void setRange(const Scalar& minimum, const Scalar& maximum, const Scalar& _step)
      { min = minimum; max = maximum; step = _step;}
    };

    template <typename T, typename RangeT>
    class RangedPropertyTpl : public PropertyTpl<T>, public Range<RangeT> {
      public:
        typedef boost::function<void(const T&)> Setter_t;
        typedef boost::function<  T (void)> Getter_t;
        typedef shared_ptr<RangedPropertyTpl> Ptr_t;

        static Ptr_t create (const std::string& name, const Getter_t& g, const Setter_t& s) { return Ptr_t(new RangedPropertyTpl(name, g, s)); }
        static Ptr_t create (const std::string& name, const Getter_t& g) { return Ptr_t(new RangedPropertyTpl(name, g, Setter_t())); }
        static Ptr_t create (const std::string& name, const Setter_t& s) { return Ptr_t(new RangedPropertyTpl(name, Getter_t(), s)); }

        template <typename Obj, typename RetType>
        static Ptr_t create (const std::string& name, Obj* obj,
            RetType (Obj::*mem_get)() const,
            void (Obj::*mem_set)(const T&))
        {
          return create (name, Getter_t(boost::bind(mem_get, obj)),
              Setter_t(boost::bind(mem_set, obj, _1)));
        }
        template <typename Obj, typename RetType>
        static Ptr_t create (const std::string& name, Obj* obj,
            RetType (Obj::*mem_get)() const,
            void (Obj::*mem_set)(T))
        {
          return create (name, Getter_t(boost::bind(mem_get, obj)),
              Setter_t(boost::bind(mem_set, obj, _1)));
        }
        template <typename Obj, typename RetType>
        static Ptr_t createRO (const std::string& name, Obj* obj,
            RetType (Obj::*mem_get)() const)
        {
          return create (name, Getter_t(boost::bind(mem_get, obj)));
        }
        template <typename Obj>
        static Ptr_t createWO (const std::string& name, Obj* obj,
            void (Obj::*mem_set)(const T&))
        {
          return create (name, Setter_t(boost::bind(mem_set, obj, _1)));
        }
        template <typename Obj, typename RetType>
        static Ptr_t createWO (const std::string& name, Obj* obj,
            void (Obj::*mem_set)(T))
        {
          return create (name, Setter_t(boost::bind(mem_set, obj, _1)));
        }


        RangedPropertyTpl(const std::string& name, const Getter_t& g, const Setter_t& s)
          : PropertyTpl<T>(name, g, s) {}

        virtual ~RangedPropertyTpl() {}
    };

    typedef PropertyTpl<bool         > BoolProperty;
    typedef PropertyTpl<int          > IntProperty;
    typedef PropertyTpl<float        > FloatProperty;
    typedef PropertyTpl<std::string  > StringProperty;
    typedef PropertyTpl<osgVector2   > Vector2Property;
    typedef PropertyTpl<osgVector3   > Vector3Property;
    typedef PropertyTpl<osgVector4   > Vector4Property;
    typedef PropertyTpl<Configuration> ConfigurationProperty;

    typedef RangedPropertyTpl<int          > RangedIntProperty;
    typedef RangedPropertyTpl<float        > RangedFloatProperty;
    typedef RangedPropertyTpl<osgVector2   , float> RangedVector2Property;
    typedef RangedPropertyTpl<osgVector3   , float> RangedVector3Property;
    typedef RangedPropertyTpl<osgVector4   , float> RangedVector4Property;
    typedef RangedPropertyTpl<Configuration, float> RangedConfigurationProperty;

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

        virtual QWidget* guiEditor ();

      protected:
        /// Set the enum property.
        /// It also checks that \c value is a valid enum.
        bool impl_set(const int& value);

        /// Set the enum property from a string.
        /// It also checks that \c value is a valid enum.
        bool impl_set(const std::string& value);

        /// Get the enum property as an integer
        bool impl_get(int   & v);

        /// Get the enum property as a string
        bool impl_get(std::string   & v);

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

        QWidget* guiEditor ();
    };

} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_NODE_PROPERTY_HH */
