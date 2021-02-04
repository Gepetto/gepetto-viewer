// Copyright (c) 2018, Joseph Mirabel
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

#include <gepetto/viewer/node-property.h>

#include <sstream>

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <gepetto/gui/dialog/configuration.hh>

namespace gepetto {
namespace viewer {
  struct Initializer {
    Initializer() {
      qRegisterMetaType<osgVector2>("osgVector2");
      qRegisterMetaType<osgVector3>("osgVector3");
      qRegisterMetaType<osgVector4>("osgVector4");
      qRegisterMetaType<Configuration>("Configuration");
    }
  };

  static Initializer initializer;

  template <typename T> inline bool invalidType(const std::string& name, T) {
    throw std::invalid_argument ("Property " + name + " is not of type " + details::property_type<T>::to_string());
  }

  Property::Property(const std::string& name) :
    QObject(NULL),
    name_ (name)
  {
    setObjectName(QString::fromStdString(name));
  }

  template<typename T>
  bool check_if_value_changed(Property& prop, const T& v)
  {
    if(!prop.hasReadAccess()) return true;
    T current;
    if(!prop.get(current)) return true;
    return current != v;
  }

  inline QColor qColor(const osgVector4& v)
  {
    return QColor::fromRgbF((qreal)v[0], (qreal)v[1], (qreal)v[2], (qreal)v[3]);
  }

#define SET_IMPLEMENTATION(v,emit2)                                            \
  if (!check_if_value_changed(*this,v)) return true;                           \
  if (impl_set(v)) { emit valueChanged(v); emit2; return true; }               \
  return false

  bool Property::set(      void             ) { bool b = impl_set( ); if (b) { emit valueChanged( ); } return b; }
  bool Property::set(const bool          & v) { SET_IMPLEMENTATION(v,); }
  bool Property::set(const int           & v) { SET_IMPLEMENTATION(v,); }
  bool Property::set(const float         & v) { SET_IMPLEMENTATION(v, emit valueChanged(static_cast<double>(v))); }
  bool Property::set(const std::string   & v) { SET_IMPLEMENTATION(v, emit valueChanged(QString::fromStdString(v))); }
  bool Property::set(const osgVector2    & v) { SET_IMPLEMENTATION(v,); }
  bool Property::set(const osgVector3    & v) { SET_IMPLEMENTATION(v,); }
  bool Property::set(const osgVector4    & v) { SET_IMPLEMENTATION(v, emit valueChanged(qColor(v))); }
  bool Property::set(const Configuration & v) { SET_IMPLEMENTATION(v,); }

  bool Property::get(void             ) { return impl_get( ); }
  bool Property::get(bool          & v) { return impl_get(v); }
  bool Property::get(int           & v) { return impl_get(v); }
  bool Property::get(float         & v) { return impl_get(v); }
  bool Property::get(std::string   & v) { return impl_get(v); }
  bool Property::get(osgVector2    & v) { return impl_get(v); }
  bool Property::get(osgVector3    & v) { return impl_get(v); }
  bool Property::get(osgVector4    & v) { return impl_get(v); }
  bool Property::get(Configuration & v) { return impl_get(v); }

  bool Property::impl_set(      void             ) { throw std::invalid_argument ("Property " + name_ + " is not of type void."); }
  bool Property::impl_set(const bool          & v) { return invalidType(name_,v); }
  bool Property::impl_set(const int           & v) { return invalidType(name_,v); }
  bool Property::impl_set(const float         & v) { return invalidType(name_,v); }
  bool Property::impl_set(const std::string   & v) { return invalidType(name_,v); }
  bool Property::impl_set(const osgVector2    & v) { return invalidType(name_,v); }
  bool Property::impl_set(const osgVector3    & v) { return invalidType(name_,v); }
  bool Property::impl_set(const osgVector4    & v) { return invalidType(name_,v); }
  bool Property::impl_set(const Configuration & v) { return invalidType(name_,v); }

  bool Property::impl_get(void             ) { throw std::invalid_argument ("Property " + name_ + " is not of type void."); }
  bool Property::impl_get(bool          & v) { return invalidType(name_,v); }
  bool Property::impl_get(int           & v) { return invalidType(name_,v); }
  bool Property::impl_get(float         & v) { return invalidType(name_,v); }
  bool Property::impl_get(std::string   & v) { return invalidType(name_,v); }
  bool Property::impl_get(osgVector2    & v) { return invalidType(name_,v); }
  bool Property::impl_get(osgVector3    & v) { return invalidType(name_,v); }
  bool Property::impl_get(osgVector4    & v) { return invalidType(name_,v); }
  bool Property::impl_get(Configuration & v) { return invalidType(name_,v); }

  bool Property::set(const double& v) { return set(static_cast<float>(v)); }

  bool Property::get(double      & v)
  {
    float _v = static_cast<float>(v);
    bool res = get (_v);
    v = static_cast<double>(_v);
    return res;
  }

  bool Property::set(const QString& v) { return set(v.toStdString()); }

  bool Property::get(QString      & v)
  {
    std::string _v = v.toStdString();
    bool res = get (_v);
    v = QString::fromStdString(_v);
    return res;
  }

  bool Property::set(const QColor& v)
  {
    return set(osgVector4((float)v.redF(), (float)v.greenF(), (float)v.blueF(),
          (float)v.alphaF()));
  }

  bool Property::get(QColor      & v)
  {
    osgVector4 _v ((float)v.redF(), (float)v.greenF(), (float)v.blueF(),
          (float)v.alphaF());
    bool res = get (_v);
    v = qColor(_v);
    return res;
  }

  QWidget* VoidProperty::guiEditor ()
  {
    QString toolTip (
        "Python:\n"
        "  gui.callVoidProperty(nodeName,\"%1\")");
    QPushButton* button = new QPushButton(objectName());
    button->setToolTip (toolTip.arg(objectName()));
    QObject::connect(button, &QAbstractButton::clicked, [this]() { get(); });
    return button;
  }

  namespace details {
    template <typename Scalar, typename QtScalar, typename QtSpinBox>
    inline void setSpinBoxRange(const Property* prop, QtSpinBox* sb)
    {
      const Range<Scalar>* range = dynamic_cast<const Range<Scalar>*>(prop);
      if (range) {
        if (range->hasMin()) sb->setMinimum(static_cast<QtScalar>(range->min));
        if (range->hasMax()) sb->setMaximum(static_cast<QtScalar>(range->max));
        sb->setSingleStep(static_cast<QtScalar>(range->step));
#if QT_VERSION > QT_VERSION_CHECK(5, 12, 0)
        if (range->adaptiveDecimal) sb->setStepType (QAbstractSpinBox::AdaptiveDecimalStepType);
#endif
      }
    }

    template <> QWidget* buildEditor<bool         > (Property* prop)
    {
      QString toolTip (
          "Python:\n"
          "  gui.getBoolProperty(nodeName,\"%1\")\n"
          "  gui.setBoolProperty(nodeName,\"%1\",boolean)");
      QCheckBox* cb = new QCheckBox;
      cb->setToolTip (toolTip.arg(prop->objectName()));
      bool value;
      /* bool success = */ prop->get(value);
      cb->setChecked(value);
      if (prop->hasWriteAccess()) {
        prop->connect(cb, SIGNAL(toggled(bool)), SLOT(set(bool)), Qt::DirectConnection);
        cb->connect(prop, SIGNAL(valueChanged(bool)), SLOT(setChecked(bool)), Qt::DirectConnection);
      } else
        cb->setEnabled(false);
      return cb;
    }

    template <> QWidget* buildEditor<std::string  > (Property* prop)
    {
      QString toolTip (
          "Python:\n"
          "  gui.getStringProperty(nodeName,\"%1\")\n"
          "  gui.setStringProperty(nodeName,\"%1\",str)");
      QLineEdit* le = new QLineEdit;
      le->setToolTip (toolTip.arg(prop->objectName()));
      std::string value;
      /* bool success = */ prop->get(value);
      le->setText(QString::fromStdString(value));
      if (prop->hasWriteAccess()) {
        prop->connect(le, SIGNAL(textChanged(QString)), SLOT(set(QString)), Qt::DirectConnection);
        le->connect(prop, SIGNAL(valueChanged(QString)), SLOT(setText(QString)), Qt::DirectConnection);
      } else
        le->setReadOnly(true);
      return le;
    }

    template <> QWidget* buildEditor<float> (Property* prop)
    {
      QString toolTip (
          "Python:\n"
          "  gui.getFloatProperty(nodeName,\"%1\")\n"
          "  gui.setFloatProperty(nodeName,\"%1\",float)");
      QDoubleSpinBox* dsb = new QDoubleSpinBox;
      dsb->setObjectName(prop->objectName());
      dsb->setToolTip (toolTip.arg(prop->objectName()));
      float value;
      /* bool success = */ prop->get(value);
      dsb->setValue(value);
      if (prop->hasWriteAccess()) {
        prop->connect(dsb, SIGNAL(valueChanged(double)), SLOT(set(double)), Qt::DirectConnection);
        dsb->connect(prop, SIGNAL(valueChanged(double)), SLOT(setValue(double)), Qt::DirectConnection);
      } else
        dsb->setEnabled(false);
      setSpinBoxRange<float, double>(prop, dsb);
      dsb->setDecimals(15);
      return dsb;
    }

    template <> QWidget* buildEditor<int          > (Property* prop)
    {
      QString toolTip (
          "Python:\n"
          "  gui.getIntProperty(nodeName,\"%1\")\n"
          "  gui.setIntProperty(nodeName,\"%1\",int)");
      QSpinBox* dsb = new QSpinBox;
      dsb->setToolTip (toolTip.arg(prop->objectName()));
      int value;
      /* bool success = */ prop->get(value);
      dsb->setValue(value);
      if (prop->hasWriteAccess()) {
        prop->connect(dsb, SIGNAL(valueChanged(int)), SLOT(set(int)), Qt::DirectConnection);
        dsb->connect(prop, SIGNAL(valueChanged(int)), SLOT(setValue(int)), Qt::DirectConnection);
      } else
        dsb->setEnabled(false);
      setSpinBoxRange<int, int>(prop, dsb);
      return dsb;
    }

    template<typename VectorType> struct traits { };
    template<> struct traits <osgVector2> { typedef gui::Vector2Dialog Dialog_t; };
    template<> struct traits <osgVector3> { typedef gui::Vector3Dialog Dialog_t; };
    template<> struct traits <osgVector4> { typedef gui::Vector4Dialog Dialog_t; };

    template<typename VectorType>
    QWidget* buildVectorNEditor (const QString& name, int N, Property* prop)
    {
      typedef typename traits<VectorType>::Dialog_t Dialog_t;
        QString toolTip (
          "Python:\n"
          "  gui.getVector3Property(nodeName,\"%1\")\n"
          "  gui.setVector3Property(nodeName,\"%1\",int)");
      QPushButton* button = new QPushButton("Set value");
      button->setToolTip (toolTip.arg(name));

      /// Vector3 dialog should be opened in a different place
      Dialog_t* cfgDialog = new Dialog_t(prop);
      cfgDialog->setValueFromProperty(prop);
      switch (N) {
        case 2:
          prop->connect (cfgDialog, SIGNAL(valueChanged (osgVector2)),
              SLOT(set(osgVector2)), Qt::DirectConnection);
          cfgDialog->connect (prop, SIGNAL(valueChanged (osgVector2)),
              SLOT(set(osgVector2)), Qt::DirectConnection);
          break;
        case 3:
          prop->connect (cfgDialog, SIGNAL(valueChanged (osgVector3)),
              SLOT(set(osgVector3)), Qt::DirectConnection);
          cfgDialog->connect (prop, SIGNAL(valueChanged (osgVector3)),
              SLOT(set(osgVector3)), Qt::DirectConnection);
          break;
        case 4:
          prop->connect (cfgDialog, SIGNAL(valueChanged (osgVector4)),
              SLOT(set(osgVector4)), Qt::DirectConnection);
          cfgDialog->connect (prop, SIGNAL(valueChanged (osgVector4)),
              SLOT(set(osgVector4)), Qt::DirectConnection);
          break;
        default:
          break;
      }
      cfgDialog->setProperty("propertyName", name);
      cfgDialog->connect(button, SIGNAL(clicked()), SLOT(show()));

      return button;
    }

    template <> QWidget* buildEditor<osgVector2   > (Property* prop)
    {
      if (!prop->hasWriteAccess()) return NULL;
      return buildVectorNEditor<osgVector2> (prop->objectName(), 2, prop);
    }

    template <> QWidget* buildEditor<osgVector3   > (Property* prop)
    {
      if (!prop->hasWriteAccess()) return NULL;
      return buildVectorNEditor<osgVector3> (prop->objectName(), 3, prop);
    }

    QWidget* buildColorEditor (QString& name, Property* prop)
    {
      QString toolTip (
          "Python:\n"
          "  gui.getColorProperty(nodeName,\"%1\")\n"
          "  gui.setColorProperty(nodeName,\"%1\",int)");
      QColor color;
      /* bool success = */ prop->get(color);

      QPushButton* button = new QPushButton("Select color");
      button->setToolTip (toolTip.arg(name));
      // Set icon for current color value

      /// Color dialog should be opened in a different place
      QColorDialog* colorDialog = new QColorDialog(color, button);
      colorDialog->setOption(QColorDialog::ShowAlphaChannel, true);
      colorDialog->setOption(QColorDialog::NoButtons, true);

      colorDialog->setProperty("propertyName", name);
      colorDialog->connect(button, SIGNAL(clicked()), SLOT(open()));

      prop->connect (colorDialog, SIGNAL(currentColorChanged(QColor)), SLOT(set(QColor)), Qt::DirectConnection);
#if __cplusplus >= 201103L and (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
      QObject::connect (prop,
          QOverload<const QColor &>::of(&Property::valueChanged),
            colorDialog, &QColorDialog::setCurrentColor,
          //SLOT(set(QColor)),
          Qt::DirectConnection);
#endif

      return button;
    }

    template <> QWidget* buildEditor<osgVector4   > (Property* prop)
    {
      if (!prop->hasWriteAccess()) return NULL;

      QString name (prop->objectName());
      if (name.contains ("color", Qt::CaseInsensitive))
        return buildColorEditor(name, prop);

      return buildVectorNEditor<osgVector4> (prop->objectName(), 4, prop);
    }

    template <> QWidget* buildEditor<Configuration> (Property* prop)
    {
      // We could use buildVectorNEditor. The only bad point is that tooltip
      // which will say setVector3Property instead of setConfigurationProperty...
      if (!prop->hasWriteAccess()) return NULL;

      QString name (QString::fromStdString(prop->name()));
      QPushButton* button = new QPushButton("Set transform");

      /// Color dialog should be opened in a different place
      gui::ConfigurationDialog* cfgDialog = new gui::ConfigurationDialog(prop);
      cfgDialog->setValueFromProperty(prop);
      prop->connect (cfgDialog, SIGNAL(valueChanged (Configuration)),
          SLOT(set(Configuration)), Qt::DirectConnection);
      cfgDialog->connect (prop, SIGNAL(valueChanged (Configuration)),
          SLOT(set(Configuration)), Qt::DirectConnection);

      cfgDialog->setProperty("propertyName", name);
      cfgDialog->connect(button, SIGNAL(clicked()), SLOT(show()));

      return button;
    }
  } // namespace details

  int MetaEnum::from_string (const std::string& s)
  {
    for (std::size_t i = 0; i < names.size(); ++i)
      if (s == names[i]) return values[i];
    throw std::invalid_argument("Unknown name of enum (" + type + "): " + s);
  }
  std::string MetaEnum::to_string (const int& v)
  {
    for (std::size_t i = 0; i < names.size(); ++i)
      if (v == values[i]) return names[i];
    throw std::invalid_argument("Unknown enum value (" + type + ")");
  }

  MetaEnum* visibilityModeEnum ()
  {
    static MetaEnum vm;
    if (vm.type.size() == 0) {
      vm.type = "VisibilityMode";
      vm.names .push_back ("ON"           ); vm.values.push_back (VISIBILITY_ON );
      vm.names .push_back ("ALWAYS_ON_TOP"); vm.values.push_back (ALWAYS_ON_TOP );
      vm.names .push_back ("OFF"          ); vm.values.push_back (VISIBILITY_OFF);
    }
    return &vm;
  }
  MetaEnum* wireFrameModeEnum  ()
  {
    static MetaEnum wm;
    if (wm.type.size() == 0) {
      wm.type = "VisibilityMode";
      wm.names .push_back ("FILL"              ); wm.values.push_back (FILL              );
      wm.names .push_back ("WIREFRAME"         ); wm.values.push_back (WIREFRAME         );
      wm.names .push_back ("FILL_AND_WIREFRAME"); wm.values.push_back (FILL_AND_WIREFRAME);
    }
    return &wm;
  }
  MetaEnum* lightingModeEnum  ()
  {
    static MetaEnum lm;
    if (lm.type.size() == 0) {
      lm.type = "LightingMode";
      lm.names .push_back ("ON" ); lm.values.push_back (LIGHT_INFLUENCE_ON );
      lm.names .push_back ("OFF"); lm.values.push_back (LIGHT_INFLUENCE_OFF);
    }
    return &lm;
  }
  MetaEnum* glImmediateModeEnum ()
  {
    static MetaEnum lm;
    if (lm.type.size() == 0) {
      lm.type = "GLImmediateMode";
      lm.names.push_back ("GL_LINES"         ); lm.values.push_back (GL_LINES         );
      lm.names.push_back ("GL_POINTS"        ); lm.values.push_back (GL_POINTS        );
      lm.names.push_back ("GL_LINE_STRIP"    ); lm.values.push_back (GL_LINE_STRIP    );
      lm.names.push_back ("GL_LINE_LOOP"     ); lm.values.push_back (GL_LINE_LOOP     );
      lm.names.push_back ("GL_POLYGON"       ); lm.values.push_back (GL_POLYGON       );
      lm.names.push_back ("GL_QUADS"         ); lm.values.push_back (GL_QUADS         );
      lm.names.push_back ("GL_QUAD_STRIP"    ); lm.values.push_back (GL_QUAD_STRIP    );
      lm.names.push_back ("GL_TRIANGLE_STRIP"); lm.values.push_back (GL_TRIANGLE_STRIP);
      lm.names.push_back ("GL_TRIANGLES"     ); lm.values.push_back (GL_TRIANGLES     );
      lm.names.push_back ("GL_TRIANGLE_FAN"  ); lm.values.push_back (GL_TRIANGLE_FAN  );
    }
    return &lm;
  }

  bool EnumProperty::impl_set(const int& value)
  {
    const MetaEnum& me = *metaEnum();
    for (std::size_t i = 0; i < me.values.size(); ++i)
      if (me.values[i] == value)
        return IntProperty::impl_set(value);
    std::ostringstream oss;
    oss << "Invalid value " << value << " for enum " << me.type << ". "
      "Possible values are ";
    for (std::size_t i = 0; i < me.values.size(); ++i)
      oss << me.names[i] << " (" << me.values[i] << "), ";
    throw std::invalid_argument(oss.str());
    return false;
  }

  bool EnumProperty::impl_set(const std::string& value)
  {
    const MetaEnum& me = *metaEnum();
    for (std::size_t i = 0; i < me.names.size(); ++i)
      if (me.names[i] == value)
        return IntProperty::impl_set(me.values[i]);
    std::ostringstream oss;
    oss << "Invalid value " << value << " for enum " << me.type << ". "
      "Possible values are ";
    for (std::size_t i = 0; i < me.values.size(); ++i)
      oss << me.names[i] << " (" << me.values[i] << "), ";
    throw std::invalid_argument(oss.str());
    return false;
  }

  bool EnumProperty::impl_get(int& v)
  {
    return IntProperty::impl_get (v);
  }

  bool EnumProperty::impl_get(std::string& str)
  {
    int value;
    if (!IntProperty::impl_get(value)) return false;
    const MetaEnum& me = *metaEnum();
    for (std::size_t i = 0; i < me.names.size(); ++i)
      if (me.values[i] == value) {
        str = me.names[i];
        return true;
      }
    return false;
  }

  QWidget* EnumProperty::guiEditor ()
  {
    QString toolTip (
        "Python:\n"
        "As an integer:\n"
        "  gui.getIntProperty(nodeName,\"%1\")\n"
        "  gui.setIntProperty(nodeName,\"%1\",int)\n"
        "or as a string:\n"
        "  gui.getStringProperty(nodeName,\"%1\")\n"
        "  gui.setStringProperty(nodeName,\"%1\",string)");

    QComboBox* cb = new QComboBox;
    cb->setToolTip (toolTip.arg(name().c_str()));
    std::string value;
    /* bool success = */ get(value);
    for (unsigned i = 0; i < metaEnum_->values.size(); ++i) {
      cb->addItem(metaEnum_->names[i].c_str(), metaEnum_->values[i]);
      if (value == metaEnum_->names[i])
        cb->setCurrentIndex(i);
    }
    if (hasWriteAccess()) {
      connect(cb, SIGNAL(currentTextChanged(QString)), SLOT(set(QString)), Qt::DirectConnection);
      // On Qt4, the combo box will not be updated.
#if (QT_VERSION > QT_VERSION_CHECK(5, 0, 0))
      cb->connect(this, SIGNAL(valueChanged(QString)), SLOT(setCurrentText(QString)), Qt::DirectConnection);
#endif
    } else
      cb->setEnabled(false);
    return cb;
  }
 
  Property* Properties::property(const std::string& name) const
  {
    PropertyMap_t::const_iterator _prop = properties_.find(name);
    if (_prop == properties_.end())
      throw std::invalid_argument("Unknown property " + name);
    return _prop->second.p;
  }

  bool Properties::hasProperty(const std::string& name) const
  {
    PropertyMap_t::const_iterator _prop = properties_.find(name);
    return (_prop != properties_.end());
  }

  void Properties::addProperty(const PropertyPtr_t& prop)
  {
    addProperty(prop->name(), prop);
  }

  void Properties::addProperty(const std::string& name, const PropertyPtr_t& prop)
  {
    Wrapper wrapped (prop);
    std::pair<PropertyMap_t::iterator, bool> res =
      properties_.insert(std::make_pair(name, Wrapper(prop)));
    if (!res.second)
      res.first->second = prop;
  }

  void Properties::addProperty(Property* prop)
  {
    addProperty(prop->name(), prop);
  }

  void Properties::addProperty(const std::string& name, Property* prop)
  {
    Wrapper wrapped (prop);
    std::pair<PropertyMap_t::iterator, bool> res =
      properties_.insert(std::make_pair(name, Wrapper(prop)));
    if (!res.second)
      res.first->second = prop;
  }

  void addPropertyEditor(QFormLayout* l, const std::string& _name,
      Property* prop)
  {
    QString name (QString::fromStdString(_name));
    QWidget* field = prop->guiEditor();
    if (field != NULL) {
      QLabel* label (new QLabel (name + ':'));
      label->setToolTip (field->toolTip());
      field->setProperty("propertyName", name);
      l->addRow(label, field);
    } else {
      qDebug() << "Unhandled property" << name << "of type" << prop->type().c_str() << ".";
    }
  }

  QWidget* Properties::guiEditor ()
  {
    QWidget* editor = new QWidget;
    QFormLayout* l = new QFormLayout(editor);

    for (PropertyMap_t::const_iterator _prop = properties_.begin();
        _prop != properties_.end(); ++_prop)
    {
      if (!_prop->second->hasReadAccess()) continue;
      addPropertyEditor (l, _prop->first,_prop->second.p);
    }
    return editor;
  }
} /* namespace viewer */

} /* namespace gepetto */
