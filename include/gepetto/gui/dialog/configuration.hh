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

#ifndef GEPETTO_GUI_CONFIGURATION_DIALOG_HH
#define GEPETTO_GUI_CONFIGURATION_DIALOG_HH

#include <QDialog>

#include <gepetto/viewer/config-osg.h>

class QDoubleSpinBox;
class QLineEdit;

namespace gepetto {
  namespace gui {
    class VectorNDialog : public QDialog
    {
      Q_OBJECT

      public:
        VectorNDialog (viewer::Property* prop, int N,
            const QString& name, QWidget *parent = 0);

        void setVector2FromProperty ();
        void setVector3FromProperty ();
        void setVector4FromProperty ();

      signals:
        void valueChanged (const osgVector2& config);
        void valueChanged (const osgVector3& config);
        void valueChanged (const osgVector4& config);

      private slots:
        void updateValue2 ();
        void updateValue3 ();
        void updateValue4 ();

      protected:
        void showEvent (QShowEvent* event);

      private:
        void setPyValue ();

        viewer::Property* prop;
        QVector<QDoubleSpinBox*> spinBoxes;
        QLineEdit* pyValue;
    };

    class ConfigurationDialog : public QDialog
    {
      Q_OBJECT

      public:
        typedef viewer::Configuration Configuration;

        ConfigurationDialog (viewer::Property* prop,
            const QString& name, QWidget *parent = 0);

      signals:
        void configurationChanged (const Configuration& config);

      private slots:
        void updateConfig ();

      protected:
        void showEvent (QShowEvent* event);

      private:
        void setConfigFromProperty ();
        void setPyValue ();

        viewer::Property* prop;
        Configuration cfg;

        QDoubleSpinBox *x,*y,*z,
                       *roll,*pitch,*yaw;
        QLineEdit* pyValue;
    };
  } // namespace gui
} // namespace gepetto

#endif // GEPETTO_GUI_CONFIGURATION_DIALOG_HH
