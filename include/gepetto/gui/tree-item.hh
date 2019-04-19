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

#ifndef GEPETTO_GUI_TREEITEM_HH
#define GEPETTO_GUI_TREEITEM_HH

#include <gepetto/gui/fwd.hh>

#include <QStandardItem>
#include <QItemDelegate>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QMenu>
#include <QPushButton>
#include <QSignalMapper>

#include <gepetto/viewer/node.h>

namespace gepetto {
  namespace gui {
    class BodyTreeItem : public QObject, public QStandardItem
    {
      Q_OBJECT

      public:
        BodyTreeItem (QObject* parent, NodePtr_t node);
        void initialize();

        virtual QStandardItem* clone () const;

        virtual int type() {
          return QStandardItem::UserType+1;
        }

        NodePtr_t node () const;

        void populateContextMenu (QMenu* menu);

        void setParentGroup (const std::string& parent);

        QWidget* propertyEditors () const
        {
          return propertyEditors_;
        }

        virtual ~BodyTreeItem();

      public:
        void attachToWindow (unsigned int windowID);

        public slots:
          void setViewingMode (QString mode);
        void setVisibilityMode (QString mode);
        void removeFromGroup ();
        void remove ();
        void removeAll ();
        void addLandmark ();
        void deleteLandmark ();
        QString text () const { return QStandardItem::text(); }

    signals:
        void requestInitialize();
    private slots:
        void doInitialize();

        void setBoolProperty (bool value) const;
        void setIntProperty (int value) const;
        void setUIntProperty (int value) const;
        void setStringProperty (const QString& value) const;
        void setFloatProperty (const double& value) const;
        void setVector3Property (const osgVector3& value) const;
        void setColorProperty (const QColor& value) const;
        void setConfigurationProperty (const Configuration& value) const;

      private:
        template <typename T> void setProperty(const QObject* sender, const T& value) const;

        NodePtr_t node_;
        std::string parentGroup_;

        QWidget* propertyEditors_;

        friend class VisibilityItem;
    };
  } // namespace gui
} // namespace gepetto

#endif // GEPETTO_GUI_TREEITEM_HH
