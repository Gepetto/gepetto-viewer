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

#include "gepetto/gui/tree-item.hh"

#include <QtGlobal>
#include <QDebug>
#include <QLineEdit>
#include <QColorDialog>
#include <QFormLayout>

#include <gepetto/viewer/group-node.h>

#include "gepetto/gui/mainwindow.hh"
#include "gepetto/gui/windows-manager.hh"
#include <gepetto/gui/bodytreewidget.hh>
#include <gepetto/gui/dialog/configuration.hh>

namespace gepetto {
  namespace gui {
    using viewer::ScopedLock;
    using viewer::PropertyPtr_t;

    template <typename Scalar, typename QtScalar, typename QtSpinBox>
    void setSpinBoxRange(const PropertyPtr_t& prop, QtSpinBox* sb)
    {
      viewer::Range<Scalar>* range = dynamic_cast<viewer::Range<Scalar>*>(prop.get());
      if (range) {
        if (range->hasMin()) sb->setMinimum(static_cast<QtScalar>(range->min));
        if (range->hasMax()) sb->setMaximum(static_cast<QtScalar>(range->max));
        sb->setSingleStep(static_cast<QtScalar>(range->step));
#if QT_VERSION > QT_VERSION_CHECK(5, 12, 0)
        if (range->adaptiveDecimal) sb->setStepType (QAbstractSpinBox::AdaptiveDecimalStepType);
#endif
      }
    }

    QWidget* voidPropertyEditor (BodyTreeItem* bti, const PropertyPtr_t prop)
    {
      QString toolTip (
          "Python:\n"
          "  gui.callVoidProperty(nodeName,\"%1\")");
      QPushButton* button = new QPushButton(prop->name().c_str());
      button->setToolTip (toolTip.arg(prop->name().c_str()));
      bti->connect(button, SIGNAL(clicked()), SLOT(callVoidProperty()));
      return button;
    }

    QWidget* boolPropertyEditor (BodyTreeItem* bti, const PropertyPtr_t prop)
    {
      QString toolTip (
          "Python:\n"
          "  gui.getBoolProperty(nodeName,\"%1\")\n"
          "  gui.setBoolProperty(nodeName,\"%1\",boolean)");
      QCheckBox* cb = new QCheckBox;
      cb->setToolTip (toolTip.arg(prop->name().c_str()));
      bool value;
      /* bool success = */ prop->get(value);
      cb->setChecked(value);
      if (prop->hasWriteAccess())
        bti->connect(cb, SIGNAL(toggled(bool)), SLOT(setBoolProperty(bool)));
      else
        cb->setEnabled(false);
      return cb;
    }

    QWidget* enumPropertyEditor (BodyTreeItem* bti, const PropertyPtr_t prop)
    {
      const viewer::EnumProperty::Ptr_t enumProp = viewer::dynamic_pointer_cast<viewer::EnumProperty> (prop);
      const viewer::MetaEnum* enumMeta = enumProp->metaEnum();

      QString toolTip (
          "Python:\n"
          "As an integer:\n"
          "  gui.getIntProperty(nodeName,\"%1\")\n"
          "  gui.setIntProperty(nodeName,\"%1\",int)\n"
          "or as a string:\n"
          "  gui.getStringProperty(nodeName,\"%1\")\n"
          "  gui.setStringProperty(nodeName,\"%1\",string)");

      QComboBox* cb = new QComboBox;
      cb->setToolTip (toolTip.arg(prop->name().c_str()));
      int value;
      /* bool success = */ enumProp->get(value);
      std::size_t indexSelected = 0;
      for (std::size_t i = 0; i < enumMeta->values.size(); ++i)
      {
        cb->addItem(enumMeta->names[i].c_str(), enumMeta->values[i]);
        if (value == enumMeta->values[i]) indexSelected = i;
      }
      cb->setCurrentIndex((int)indexSelected);
      if (prop->hasWriteAccess())
        bti->connect(cb, SIGNAL(currentIndexChanged(int)), SLOT(setIntProperty(int)));
      else
        cb->setEnabled(false);
      return cb;
    }

    QWidget* stringPropertyEditor (BodyTreeItem* bti, const PropertyPtr_t prop)
    {
      QString toolTip (
          "Python:\n"
          "  gui.getStringProperty(nodeName,\"%1\")\n"
          "  gui.setStringProperty(nodeName,\"%1\",str)");
      QLineEdit* le = new QLineEdit;
      le->setToolTip (toolTip.arg(prop->name().c_str()));
      std::string value;
      /* bool success = */ prop->get(value);
      le->setText(QString::fromStdString(value));
      if (prop->hasWriteAccess())
        bti->connect(le, SIGNAL(textChanged(QString)), SLOT(setStringProperty(QString)));
      else
        le->setReadOnly(true);
      return le;
    }

    QWidget* floatPropertyEditor (BodyTreeItem* bti, const PropertyPtr_t prop)
    {
      QString toolTip (
          "Python:\n"
          "  gui.getFloatProperty(nodeName,\"%1\")\n"
          "  gui.setFloatProperty(nodeName,\"%1\",float)");
      QDoubleSpinBox* dsb = new QDoubleSpinBox;
      dsb->setToolTip (toolTip.arg(prop->name().c_str()));
      float value;
      /* bool success = */ prop->get(value);
      dsb->setValue(value);
      if (prop->hasWriteAccess())
        bti->connect(dsb, SIGNAL(valueChanged(double)), SLOT(setFloatProperty(double)));
      else
        dsb->setEnabled(false);
      setSpinBoxRange<float, double>(prop, dsb);
      return dsb;
    }

    QWidget* intPropertyEditor (BodyTreeItem* bti, const PropertyPtr_t prop,
        bool isSigned)
    {
      QString toolTip (
          "Python:\n"
          "  gui.getIntProperty(nodeName,\"%1\")\n"
          "  gui.setIntProperty(nodeName,\"%1\",int)");
      QSpinBox* dsb = new QSpinBox;
      dsb->setToolTip (toolTip.arg(prop->name().c_str()));
      int value;
      if (isSigned) {
        /* bool success = */ prop->get(value);
      } else {
        unsigned long v;
        /* bool success = */ prop->get(v);
        value = (int)v;
      }
      dsb->setValue(value);
      if (prop->hasWriteAccess()) {
        if (isSigned)
          bti->connect(dsb, SIGNAL(valueChanged(int)), SLOT(setIntProperty(int)));
        else
          bti->connect(dsb, SIGNAL(valueChanged(int)), SLOT(setUIntProperty(int)));
      } else
        dsb->setEnabled(false);
      setSpinBoxRange<int, int>(prop, dsb);
      setSpinBoxRange<unsigned long, int>(prop, dsb);
      return dsb;
    }

    QWidget* colorPropertyEditor (BodyTreeItem* bti, const PropertyPtr_t prop)
    {
      if (!prop->hasWriteAccess()) return NULL;
      QString toolTip (
          "Python:\n"
          "  gui.getColorProperty(nodeName,\"%1\")\n"
          "  gui.setColorProperty(nodeName,\"%1\",int)");
      osgVector4 value;
      /* bool success = */ prop->get(value);
      QColor color;
      color.setRgbF((qreal)value[0],(qreal)value[1],(qreal)value[2],(qreal)value[3]);

      QPushButton* button = new QPushButton("Select color");
      button->setToolTip (toolTip.arg(prop->name().c_str()));
      // Set icon for current color value

      /// Color dialog should be opened in a different place
      QColorDialog* colorDialog = new QColorDialog(color, MainWindow::instance());
      colorDialog->setOption(QColorDialog::ShowAlphaChannel, true);

      colorDialog->setProperty("propertyName", QString::fromStdString(prop->name()));
      colorDialog->connect(button, SIGNAL(clicked()), SLOT(open()));
      bti->connect (colorDialog, SIGNAL(colorSelected(QColor)), SLOT(setColorProperty(QColor)));

      return button;
    }

    QWidget* vector3PropertyEditor (BodyTreeItem* bti, const PropertyPtr_t prop)
    {
      if (!prop->hasWriteAccess()) return NULL;

      QString toolTip (
          "Python:\n"
          "  gui.getVector3Property(nodeName,\"%1\")\n"
          "  gui.setVector3Property(nodeName,\"%1\",int)");
      QPushButton* button = new QPushButton("Set vector 3");
      button->setToolTip (toolTip.arg(prop->name().c_str()));

      /// Vector3 dialog should be opened in a different place
      Vector3Dialog* cfgDialog = new Vector3Dialog(prop,
          bti->text(), MainWindow::instance());

      cfgDialog->setProperty("propertyName", QString::fromStdString(prop->name()));
      cfgDialog->connect(button, SIGNAL(clicked()), SLOT(show()));
      bti->connect (cfgDialog, SIGNAL(valueChanged (const osgVector3&)),
          SLOT(setVector3Property(const osgVector3&)));

      return button;
    }

    QWidget* configurationPropertyEditor (BodyTreeItem* bti, const PropertyPtr_t prop)
    {
      if (!prop->hasWriteAccess()) return NULL;

      QPushButton* button = new QPushButton("Set transform");

      /// Color dialog should be opened in a different place
      ConfigurationDialog* cfgDialog = new ConfigurationDialog(prop,
          bti->text(), MainWindow::instance());

      cfgDialog->setProperty("propertyName", QString::fromStdString(prop->name()));
      cfgDialog->connect(button, SIGNAL(clicked()), SLOT(show()));
      bti->connect (cfgDialog, SIGNAL(configurationChanged (const Configuration&)),
          SLOT(setConfigurationProperty(const Configuration&)));

      return button;
    }

    BodyTreeItem::BodyTreeItem(QObject *parent, NodePtr_t node) :
      QObject (parent),
      QStandardItem (QString::fromStdString (node->getID().substr(node->getID().find_last_of("/") + 1))),
      node_ (node)
    {
      setEditable(false);
    }

    void BodyTreeItem::initialize ()
    {
      connect(this, SIGNAL(requestInitialize()), SLOT(doInitialize()));
      emit requestInitialize();
    }

    void BodyTreeItem::doInitialize ()
    {
      propertyEditors_ = new QWidget();
      BodyTreeWidget* bt = MainWindow::instance()->bodyTree();
      if (propertyEditors_->thread() != bt->thread())
        propertyEditors_->moveToThread(bt->thread());
      QFormLayout* l = new QFormLayout(propertyEditors_);

      l->addRow("Node name:", new QLabel (node_->getID().c_str()));

      const viewer::PropertyMap_t& props = node_->properties();
      for (viewer::PropertyMap_t::const_iterator _prop = props.begin();
           _prop != props.end(); ++_prop)
      {
        const PropertyPtr_t prop = _prop->second;
        if (!prop->hasReadAccess()) continue;

        QString name = _prop->first.c_str();
        QWidget* field = NULL;
        if (prop->type() == "enum") {
          field = enumPropertyEditor(this, prop);
        } else if (prop->type() == "void") {
          field = voidPropertyEditor(this, prop);
        } else if (prop->type() == "bool") {
          field = boolPropertyEditor(this, prop);
        } else if (prop->type() == "string") {
          field = stringPropertyEditor(this, prop);
        } else if (prop->type() == "float") {
          field = floatPropertyEditor(this, prop);
        } else if (prop->type() == "int") {
          field = intPropertyEditor(this, prop, true);
        } else if (prop->type() == "unsigned long") {
          field = intPropertyEditor(this, prop, false);
        } else if (prop->type() == "Configuration") {
          field = configurationPropertyEditor(this, prop);
        } else if (prop->type() == "osgVector3") {
          field = vector3PropertyEditor (this, prop);
        } else if (prop->type() == "osgVector4") {
          if (name.contains ("color", Qt::CaseInsensitive)) {
            field = colorPropertyEditor (this, prop);
          } else {
            field = NULL;
          }
        }
        if (field != NULL) {
          QLabel* label (new QLabel (name + ':'));
          label->setToolTip (field->toolTip());
          field->setProperty("propertyName", name);
          l->addRow(label, field);
        } else {
          qDebug() << "Unhandled property" << name << "of type" << prop->type().c_str() << ".";
        }
      }
      disconnect(SIGNAL(requestInitialize()));
    }

    template <typename T>
    void BodyTreeItem::setProperty (const QObject* sender, const T& value) const
    {
      if (sender != NULL) {
        QVariant nameVariant = sender->property("propertyName");
        if (nameVariant.isValid()) {
          std::string name = nameVariant.toString().toStdString();
          ScopedLock lock (MainWindow::instance()->osg()->osgFrameMutex());
          try {
            node_->setProperty<T>(name, value);
          } catch (const std::exception& e) {
            MainWindow::instance()->logError (e.what());
          }
        } else {
          qDebug() << "Sender has no property propertyName" << sender;
        }
      }
    }

    void BodyTreeItem::callVoidProperty () const
    {
      QObject* sender = QObject::sender();
      if (sender != NULL) {
        QVariant nameVariant = sender->property("propertyName");
        if (nameVariant.isValid()) {
          std::string name = nameVariant.toString().toStdString();
          ScopedLock lock (MainWindow::instance()->osg()->osgFrameMutex());
          if (!node_->hasProperty(name)) {
            qDebug() << "Node does not have property " << nameVariant;
            return;
          }
          PropertyPtr_t voidProp = node_->properties().find(name)->second;
          try {
            voidProp->get();
          } catch (const std::exception& e) {
            MainWindow::instance()->logError (e.what());
          }
        } else {
          qDebug() << "Sender has no property propertyName" << sender;
        }
      }
    }

    void BodyTreeItem::setBoolProperty (bool value) const
    {
      setProperty (QObject::sender(), value);
    }

    void BodyTreeItem::setIntProperty (int value) const
    {
      QObject* sender = QObject::sender();
      QComboBox* cb = qobject_cast<QComboBox*>(sender);
      if (cb != NULL) {
        // Enum property
        int enumValue = cb->itemData (value).toInt();
        setProperty (sender, enumValue);
      } else {
        setProperty (sender, value);
      }
    }

    void BodyTreeItem::setUIntProperty (int value) const
    {
      setProperty (QObject::sender(), (unsigned long)value);
    }

    void BodyTreeItem::setStringProperty (const QString& value) const
    {
      setProperty (QObject::sender(), value.toStdString());
    }

    void BodyTreeItem::setFloatProperty (const double& value) const
    {
      setProperty (QObject::sender(), float(value));
    }

    void BodyTreeItem::setVector3Property (const osgVector3& value) const
    {
      setProperty (QObject::sender(), value);
    }

    void BodyTreeItem::setColorProperty (const QColor& value) const
    {
      osgVector4 c (
          (float)value.redF(),
          (float)value.greenF(),
          (float)value.blueF(),
          (float)value.alphaF());
      setProperty (QObject::sender(), c);
    }

    void BodyTreeItem::setConfigurationProperty (const Configuration& value) const
    {
      setProperty (QObject::sender(), value);
    }

    BodyTreeItem::~BodyTreeItem()
    {
      if (propertyEditors_->parent() != NULL)
        delete propertyEditors_;
    }

    QStandardItem* BodyTreeItem::clone() const
    {
      return new BodyTreeItem (QObject::parent(), node_);
    }

    NodePtr_t BodyTreeItem::node() const
    {
      return node_;
    }

    void BodyTreeItem::populateContextMenu(QMenu *contextMenu)
    {
      /// Landmark
      QMenu* lm = contextMenu->addMenu (tr("Landmark"));
      QAction* alm = lm->addAction (tr("Add"));
      QAction* dlm = lm->addAction (tr("Remove"));
      connect (alm, SIGNAL (triggered()), SLOT (addLandmark()));
      connect (dlm, SIGNAL (triggered()), SLOT (deleteLandmark()));
      /// Remove
      QAction* remove = contextMenu->addAction (tr("Remove"));
      connect (remove, SIGNAL (triggered()), SLOT (remove()));
      QAction* removeAll = contextMenu->addAction (tr("Remove all"));
      connect (removeAll, SIGNAL (triggered()), SLOT (removeAll()));
      if (!parentGroup_.empty()) {
        QAction* rfg = contextMenu->addAction (tr("Remove from group"));
        connect (rfg, SIGNAL (triggered()), SLOT (removeFromGroup ()));
      }
    }

    void BodyTreeItem::setParentGroup(const std::string &parent)
    {
      parentGroup_ = parent;
    }

    void BodyTreeItem::setViewingMode(QString mode)
    {
      MainWindow::instance()->osg()->setWireFrameMode (node_->getID(),
          mode.toLocal8Bit().data());
    }

    void BodyTreeItem::setVisibilityMode(QString mode)
    {
      MainWindow::instance()->osg()->setVisibility (node_->getID(),
          mode.toLocal8Bit().data());
    }

    void BodyTreeItem::removeFromGroup()
    {
      if (parentGroup_.empty()) return;
      MainWindow::instance()->osg()->removeFromGroup (node_->getID(), parentGroup_);
      QStandardItem::parent()->removeRow(row());
    }

    void BodyTreeItem::removeAll()
    {
      MainWindow* main = MainWindow::instance();
      main->osg()->deleteNode(node_->getID(), true);
    }

    void BodyTreeItem::remove()
    {
      MainWindow* main = MainWindow::instance();
      main->osg()->deleteNode(node_->getID(), false);
    }

    void BodyTreeItem::addLandmark()
    {
      MainWindow::instance()->osg()->addLandmark(node_->getID(), 0.05f);
    }

    void BodyTreeItem::deleteLandmark()
    {
      MainWindow::instance()->osg()->deleteLandmark(node_->getID());
    }
  } // namespace gui
} // namespace gepetto
