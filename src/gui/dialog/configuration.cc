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

    VectorNDialog::VectorNDialog (viewer::Property* property, int N,
        const QString& name, QWidget *parent)
      : QDialog (parent)
      , prop (property)
      , pyValue (new QLineEdit)
    {
      for (int i = 0; i < N; ++i) {
        spinBoxes.append(new QDoubleSpinBox);
        setSpinBoxRange(property, spinBoxes.back());
      }

      setModal (false);

      pyValue->setReadOnly (true);

      const char* slotUpdateValue;
      switch (N) {
        case 2: slotUpdateValue = SLOT(updateValue2()); break;
        case 3: slotUpdateValue = SLOT(updateValue3()); break;
        case 4: slotUpdateValue = SLOT(updateValue4()); break;
        default: throw std::invalid_argument("Vector size is invalid");
      }
      foreach (QDoubleSpinBox* sb, spinBoxes)
        connect(sb, SIGNAL(valueChanged(double)), slotUpdateValue);

      QDialogButtonBox *buttonBox = new QDialogButtonBox (QDialogButtonBox::Close, Qt::Horizontal);
      connect (buttonBox->button (QDialogButtonBox::Close), SIGNAL(clicked(bool)), SLOT(accept()));

      QFormLayout *layout = new QFormLayout;
      layout->addRow(new QLabel (name));
      for (int i = 0; i < N; ++i)
        layout->addRow("X" + QString::number(i), spinBoxes[i]);
      layout->addRow("value", pyValue);
      layout->addRow(buttonBox);

      setLayout (layout);
    }

    void VectorNDialog::showEvent (QShowEvent* event)
    {
      //setValueFromProperty();
      QDialog::showEvent (event);
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

    void VectorNDialog::updateValue2 ()
    {
      osgVector2 cfg;
      getValues(cfg, spinBoxes);
      setPyValue ();
      emit valueChanged (cfg);
    }

    void VectorNDialog::updateValue3 ()
    {
      osgVector3 cfg;
      getValues(cfg, spinBoxes);
      setPyValue ();
      emit valueChanged (cfg);
    }

    void VectorNDialog::updateValue4 ()
    {
      osgVector4 cfg;
      getValues(cfg, spinBoxes);
      setPyValue ();
      emit valueChanged (cfg);
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

    void VectorNDialog::setVector2FromProperty ()
    {
      osgVector2 cfg;
      prop->get (cfg);
      setValues(cfg, spinBoxes);
    }

    void VectorNDialog::setVector3FromProperty ()
    {
      osgVector3 cfg;
      prop->get (cfg);
      setValues(cfg, spinBoxes);
    }

    void VectorNDialog::setVector4FromProperty ()
    {
      osgVector4 cfg;
      prop->get (cfg);
      setValues(cfg, spinBoxes);
    }

    void VectorNDialog::setPyValue ()
    {
      static const QString sep(", ");
      QString text = "[ ";
      foreach (QDoubleSpinBox* sb, spinBoxes)
        text += QString::number (sb->value()) + sep;
      text += " ]";
      pyValue->setText (text);
    }

    ConfigurationDialog::ConfigurationDialog (
        viewer::Property* property, const QString& name, QWidget *parent)
      : QDialog (parent)
      , prop (property)
      , x     (new QDoubleSpinBox)
      , y     (new QDoubleSpinBox)
      , z     (new QDoubleSpinBox)
      , roll  (new QDoubleSpinBox)
      , pitch (new QDoubleSpinBox)
      , yaw   (new QDoubleSpinBox)
      , pyValue (new QLineEdit)
    {
      setSpinBoxRange(property, x    );
      setSpinBoxRange(property, y    );
      setSpinBoxRange(property, z    );
      setSpinBoxRange(property, roll );
      setSpinBoxRange(property, pitch);
      setSpinBoxRange(property, yaw  );

      setModal (false);

      setConfigFromProperty ();

      pyValue->setReadOnly (true);
      setPyValue ();

      connect(x    , SIGNAL(valueChanged(double)), SLOT(updateConfig()));
      connect(y    , SIGNAL(valueChanged(double)), SLOT(updateConfig()));
      connect(z    , SIGNAL(valueChanged(double)), SLOT(updateConfig()));
      connect(roll , SIGNAL(valueChanged(double)), SLOT(updateConfig()));
      connect(pitch, SIGNAL(valueChanged(double)), SLOT(updateConfig()));
      connect(yaw  , SIGNAL(valueChanged(double)), SLOT(updateConfig()));

      QDialogButtonBox *buttonBox = new QDialogButtonBox (QDialogButtonBox::Close, Qt::Horizontal);
      connect (buttonBox->button (QDialogButtonBox::Close), SIGNAL(clicked(bool)), SLOT(accept()));

      QFormLayout *layout = new QFormLayout;
      layout->addRow(new QLabel (name));
      layout->addRow("X", x);
      layout->addRow("Y", y);
      layout->addRow("Z", z);
      layout->addRow("roll" , roll );
      layout->addRow("pitch", pitch);
      layout->addRow("yaw"  , yaw  );
      layout->addRow("value", pyValue);
      layout->addRow(buttonBox);

      setLayout (layout);
    }

    void ConfigurationDialog::showEvent (QShowEvent* event)
    {
      setConfigFromProperty();
      QDialog::showEvent (event);
    }

    void ConfigurationDialog::updateConfig ()
    {
      cfg.position[0] = (float)x->value();
      cfg.position[1] = (float)y->value();
      cfg.position[2] = (float)z->value();
      getQuatFromEuler (pitch->value(), yaw->value(), roll->value(), cfg.quat);

      setPyValue ();

      emit configurationChanged (cfg);
    }

    void ConfigurationDialog::setConfigFromProperty ()
    {
      prop->get (cfg);
      double _r,_p,_y;
      getEulerFromQuat (cfg.quat,_p,_y,_r);

      x->setValue(cfg.position[0]); x->setRange(-1000,1000); x->setSingleStep(0.01); x->setDecimals(4);
      y->setValue(cfg.position[1]); y->setRange(-1000,1000); y->setSingleStep(0.01); y->setDecimals(4);
      z->setValue(cfg.position[2]); z->setRange(-1000,1000); z->setSingleStep(0.01); z->setDecimals(4);

      roll ->setValue(_r); roll ->setRange(-osg::PI, osg::PI); roll ->setSingleStep(0.01); roll ->setDecimals(4);
      pitch->setValue(_p); pitch->setRange(-osg::PI, osg::PI); pitch->setSingleStep(0.01); pitch->setDecimals(4);
      yaw  ->setValue(_y); yaw  ->setRange(-osg::PI, osg::PI); yaw  ->setSingleStep(0.01); yaw  ->setDecimals(4);
    }

    void ConfigurationDialog::setPyValue ()
    {
      static const QString sep(", ");
      pyValue->setText ("[ "
          + QString::number (cfg.position[0]) + sep
          + QString::number (cfg.position[1]) + sep
          + QString::number (cfg.position[2]) + sep
          + QString::number (cfg.quat.x()) + sep
          + QString::number (cfg.quat.y()) + sep
          + QString::number (cfg.quat.z()) + sep
          + QString::number (cfg.quat.w()) + " ]");
    }
  } // namespace gui
} // namespace gepetto
