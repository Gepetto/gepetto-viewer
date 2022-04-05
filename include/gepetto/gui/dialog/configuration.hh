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

#include <gepetto/viewer/config-osg.h>

#include <QDialog>

class QDoubleSpinBox;
class QLineEdit;

namespace gepetto {
namespace gui {

typedef viewer::Configuration Configuration;
/*
 * README The MOC tool from Qt does not expand macros. Thus, the code in comment
below
 * does not work because the classes are not mocced.
 * The remaining of the file, after this comment, correspond to the macro
 * expansion at the time of writing.

#define DECLARE_PROPERTY_EDITOR(Name, Type)                  \
  class Name : public QDialog {                              \
    Q_OBJECT                                                 \
                                                             \
   public:                                                   \
    Name(viewer::Property* prop, QWidget* parent = 0);       \
                                                             \
    void setValueFromProperty(viewer::Property* prop);       \
                                                             \
   signals:                                                  \
    void valueChanged(const Type& config);                   \
                                                             \
   public slots:                                             \
    void set(const Type& v);                                 \
                                                             \
   private slots:                                            \
    void updateValue();                                      \
                                                             \
   private:                                                  \
    void setPyValue();                                       \
                                                             \
    QVector<QDoubleSpinBox*> spinBoxes;                      \
    QLineEdit* pyValue;                                      \
  }

DECLARE_PROPERTY_EDITOR(Vector2Dialog, osgVector2);
DECLARE_PROPERTY_EDITOR(Vector3Dialog, osgVector3);
DECLARE_PROPERTY_EDITOR(Vector4Dialog, osgVector4);
DECLARE_PROPERTY_EDITOR(ConfigurationDialog, Configuration);

#undef DECLARE_PROPERTY_EDITOR
*/

class Vector2Dialog : public QDialog {
  Q_OBJECT
 public:
  Vector2Dialog(viewer::Property* prop, QWidget* parent = 0);
  void setValueFromProperty(viewer::Property* prop);
 signals:
  void valueChanged(const osgVector2& config);
 public slots:
  void set(const osgVector2& v);
 private slots:
  void updateValue();

 private:
  void setPyValue();
  QVector<QDoubleSpinBox*> spinBoxes;
  QLineEdit* pyValue;
};
class Vector3Dialog : public QDialog {
  Q_OBJECT
 public:
  Vector3Dialog(viewer::Property* prop, QWidget* parent = 0);
  void setValueFromProperty(viewer::Property* prop);
 signals:
  void valueChanged(const osgVector3& config);
 public slots:
  void set(const osgVector3& v);
 private slots:
  void updateValue();

 private:
  void setPyValue();
  QVector<QDoubleSpinBox*> spinBoxes;
  QLineEdit* pyValue;
};
class Vector4Dialog : public QDialog {
  Q_OBJECT
 public:
  Vector4Dialog(viewer::Property* prop, QWidget* parent = 0);
  void setValueFromProperty(viewer::Property* prop);
 signals:
  void valueChanged(const osgVector4& config);
 public slots:
  void set(const osgVector4& v);
 private slots:
  void updateValue();

 private:
  void setPyValue();
  QVector<QDoubleSpinBox*> spinBoxes;
  QLineEdit* pyValue;
};
class ConfigurationDialog : public QDialog {
  Q_OBJECT
 public:
  ConfigurationDialog(viewer::Property* prop, QWidget* parent = 0);
  void setValueFromProperty(viewer::Property* prop);
 signals:
  void valueChanged(const Configuration& config);
 public slots:
  void set(const Configuration& v);
 private slots:
  void updateValue();

 private:
  void setPyValue();
  QVector<QDoubleSpinBox*> spinBoxes;
  QLineEdit* pyValue;
};

}  // namespace gui
}  // namespace gepetto

#endif  // GEPETTO_GUI_CONFIGURATION_DIALOG_HH
