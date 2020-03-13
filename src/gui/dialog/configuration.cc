// Copyright (c) 2015-2018, LAAS-CNRS
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// gepetto-viewer. If not, see <http://www.gnu.org/licenses/>.

#include <gepetto/gui/dialog/configuration.hh>

#include <QtGlobal>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <gepetto/viewer/node-property.h>

namespace gepetto {
  namespace gui {
    using viewer::Configuration;

    void setSpinBoxRange(const viewer::Property* prop, QDoubleSpinBox* sb)
    {
      const viewer::Range<float>* range = dynamic_cast<const viewer::Range<float>*>(prop);
      if (range) {
        if (range->hasMin()) sb->setMinimum(static_cast<double>(range->min));
        if (range->hasMax()) sb->setMaximum(static_cast<double>(range->max));
        sb->setSingleStep(static_cast<double>(range->step));
#if QT_VERSION > QT_VERSION_CHECK(5, 12, 0)
        if (range->adaptiveDecimal) sb->setStepType (QAbstractSpinBox::AdaptiveDecimalStepType);
#endif
      }
    }

    void getEulerFromQuat(osg::Quat q, double& heading, double& attitude, double& bank)
    {
      double limit = 0.499999;

      double sqx = q.x()*q.x();
      double sqy = q.y()*q.y();
      double sqz = q.z()*q.z();

      double t = q.x()*q.y() + q.z()*q.w();

      if (t>limit) { // gimbal lock ?
        heading = 2 * atan2(q.x(),q.w());
        attitude = osg::PI_2;
        bank = 0;
      } else if (t<-limit) {
        heading = -2 * atan2(q.x(),q.w());
        attitude = - osg::PI_2;
        bank = 0;
      } else {
        heading = atan2(2*q.y()*q.w()-2*q.x()*q.z() , 1 - 2*sqy - 2*sqz);
        attitude = asin(2*t);
        bank = atan2(2*q.x()*q.w()-2*q.y()*q.z() , 1 - 2*sqx - 2*sqz);
      }
    }

    void getQuatFromEuler(double heading, double attitude, double bank, osg::Quat& q)
    {
      double c1 = cos(heading/2);
      double s1 = sin(heading/2);
      double c2 = cos(attitude/2);
      double s2 = sin(attitude/2);
      double c3 = cos(bank/2);
      double s3 = sin(bank/2);
      double c1c2 = c1*c2;
      double s1s2 = s1*s2;

      double w =c1c2*c3 - s1s2*s3;
      double x =c1c2*s3 + s1s2*c3;
      double y =s1*c2*c3 + c1*s2*s3;
      double z =c1*s2*c3 - s1*c2*s3;

      q[0] = x; q[1] = y; q[2] = z; q[3] = w;
    }

    template <typename Vector>
    inline void getValues (Vector& v, const QVector<QDoubleSpinBox*> spinBoxes)
    {
      int i = 0;
      foreach (QDoubleSpinBox* sb, spinBoxes) {
        v[i] = (float)sb->value ();
        ++i;
      }
    }

    inline void getValues (Configuration& cfg, const QVector<QDoubleSpinBox*> spinBoxes)
    {
      const QDoubleSpinBox *x = spinBoxes[0], *y = spinBoxes[1], *z = spinBoxes[2],
        *roll = spinBoxes[3], *pitch = spinBoxes[4], *yaw = spinBoxes[5];

      cfg.position[0] = (float)x->value();
      cfg.position[1] = (float)y->value();
      cfg.position[2] = (float)z->value();
      getQuatFromEuler (pitch->value(), roll->value(), yaw->value(), cfg.quat);
    }

    template <typename Vector>
    inline void setValues (const Vector& v, QVector<QDoubleSpinBox*> spinBoxes)
    {
      int i = 0;
      foreach (QDoubleSpinBox* sb, spinBoxes) {
        sb->setValue (v[i]);
        ++i;
      }
    }

    inline void setValues (const Configuration& cfg, QVector<QDoubleSpinBox*> spinBoxes)
    {
      QDoubleSpinBox *x = spinBoxes[0], *y = spinBoxes[1], *z = spinBoxes[2],
        *roll = spinBoxes[3], *pitch = spinBoxes[4], *yaw = spinBoxes[5];

      double _r,_p,_y;
      getEulerFromQuat (cfg.quat,_p,_y,_r);

      x->setValue(cfg.position[0]);
      y->setValue(cfg.position[1]);
      z->setValue(cfg.position[2]);

      roll ->setValue(_r);
      pitch->setValue(_p);
      yaw  ->setValue(_y);
    }

    static const QString sep(", ");

    template <typename Vector>
    QString valuesToString (const QVector<QDoubleSpinBox*> spinBoxes)
    {
      QString text = "[ ";
      foreach (const QDoubleSpinBox* sb, spinBoxes)
        text += QString::number (sb->value()) + sep;
      text += " ]";
      return text;
    }

    template <>
    QString valuesToString<Configuration> (const QVector<QDoubleSpinBox*> spinBoxes)
    {
      const QDoubleSpinBox *x = spinBoxes[0], *y = spinBoxes[1], *z = spinBoxes[2],
        *roll = spinBoxes[3], *pitch = spinBoxes[4], *yaw = spinBoxes[5];

      osg::Quat quat;
      getQuatFromEuler (pitch->value(), roll->value(), yaw->value(), quat);

      return "[ "
          + QString::number (x->value()) + sep
          + QString::number (y->value()) + sep
          + QString::number (z->value()) + sep
          + QString::number (quat.x()) + sep
          + QString::number (quat.y()) + sep
          + QString::number (quat.z()) + sep
          + QString::number (quat.w()) + " ]";
    }

#define DEFINE_PROPERTY_EDITOR(Name,Type)                                      \
    void Name::updateValue ()                                                  \
    {                                                                          \
      Type cfg;                                                                \
      getValues(cfg, spinBoxes);                                               \
      setPyValue ();                                                           \
      emit valueChanged (cfg);                                                 \
    }                                                                          \
                                                                               \
    void Name::setValueFromProperty (viewer::Property* prop)                   \
    {                                                                          \
      Type cfg;                                                                \
      prop->get (cfg);                                                         \
      setValues(cfg, spinBoxes);                                               \
      setPyValue();                                                            \
    }                                                                          \
                                                                               \
    void Name::set (const Type& v)                                             \
    {                                                                          \
      foreach (QDoubleSpinBox* sb, spinBoxes)                                  \
        sb->blockSignals(true);                                                \
      setValues(v, spinBoxes);                                                 \
      foreach (QDoubleSpinBox* sb, spinBoxes)                                  \
        sb->blockSignals(false);                                               \
    }                                                                          \
                                                                               \
    void Name::setPyValue ()                                                   \
    {                                                                          \
      pyValue->setText (valuesToString<Type>(spinBoxes));                      \
    }

    DEFINE_PROPERTY_EDITOR(Vector2Dialog, osgVector2)
    DEFINE_PROPERTY_EDITOR(Vector3Dialog, osgVector3)
    DEFINE_PROPERTY_EDITOR(Vector4Dialog, osgVector4)
    DEFINE_PROPERTY_EDITOR(ConfigurationDialog, Configuration)

    template<typename T>
    QString rowLabel(int i) { return "X" + QString::number(i); }

    template<>
    QString rowLabel<Configuration>(int i)
    {
      switch(i)
      {
        case 0: return "X";
        case 1: return "Y";
        case 2: return "Z";
        case 3: return "Roll";
        case 4: return "Pitch";
        case 5: return "Yaw";
      }
      abort();
    }

#define DEFINE_PROPERTY_EDITOR_CONSTRUCTOR(Name,Type,N)                        \
    Name::Name (viewer::Property* property, QWidget *parent)                   \
      : QDialog (parent)                                                       \
      , pyValue (new QLineEdit)                                                \
    {                                                                          \
      setModal (false);                                                        \
      pyValue->setReadOnly (true);                                             \
                                                                               \
      QFormLayout *layout = new QFormLayout;                                   \
      layout->addRow(new QLabel (property->objectName()));                     \
                                                                               \
      spinBoxes.reserve(N);                                                    \
      for (int i = 0; i < N; ++i) {                                            \
        QDoubleSpinBox* sb (new QDoubleSpinBox);                               \
        spinBoxes.append(sb);                                                  \
        setSpinBoxRange(property, sb);                                         \
        connect(sb, SIGNAL(valueChanged(double)), SLOT(updateValue()));        \
                                                                               \
        layout->addRow(rowLabel<Type>(i), sb);                                 \
      }                                                                        \
      layout->addRow("value", pyValue);                                        \
                                                                               \
      QDialogButtonBox *buttonBox = new QDialogButtonBox (                     \
          QDialogButtonBox::Close, Qt::Horizontal);                            \
      connect (buttonBox->button (QDialogButtonBox::Close),                    \
          SIGNAL(clicked(bool)), SLOT(accept()));                              \
                                                                               \
      layout->addRow(buttonBox);                                               \
                                                                               \
      setLayout (layout);                                                      \
    }

    DEFINE_PROPERTY_EDITOR_CONSTRUCTOR(Vector2Dialog, osgVector2, 2)
    DEFINE_PROPERTY_EDITOR_CONSTRUCTOR(Vector3Dialog, osgVector3, 3)
    DEFINE_PROPERTY_EDITOR_CONSTRUCTOR(Vector4Dialog, osgVector4, 4)
    DEFINE_PROPERTY_EDITOR_CONSTRUCTOR(ConfigurationDialog, Configuration, 6)
  } // namespace gui
} // namespace gepetto
