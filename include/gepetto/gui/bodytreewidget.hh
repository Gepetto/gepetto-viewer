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

#ifndef GEPETTO_GUI_BODYTREEWIDGET_HH
#define GEPETTO_GUI_BODYTREEWIDGET_HH

#include <gepetto/gui/fwd.hh>

#include <QWidget>
#include <QTreeView>
#include <QToolBox>
#include <QStandardItemModel>
#include <QVector3D>

#include <gepetto/viewer/group-node.h>

namespace gepetto {
  namespace gui {
    /// Contains the list of all the bodies in the scene.
    class BodyTreeWidget : public QWidget
    {
      Q_OBJECT

    public:
      explicit BodyTreeWidget (QWidget* parent = NULL)
        : QWidget (parent)
      {}

      /// Init the widget.
      /// \param view tree view to display.
      /// \param propertyArea menu in the window
      void init(QTreeView *view, QWidget *propertyArea);

      virtual ~BodyTreeWidget () {}

      /// Get the body tree view.
      QTreeView* view ();

      QStandardItemModel* model()
      {
        return model_;
      }

      void emitBodySelected (SelectionEvent* event);

    signals:
      void bodySelected (SelectionEvent* event);

    public slots:
      /// \ingroup plugin_python
      /// \{

      /// Triggered when an item is selected in the body tree view.
      /// \param bodyName name of the body
      void selectBodyByName (const QString bodyName);

      /// Triggered when an item is selected in the body tree view.
      /// \param bodyName name of the body
      void selectBodyByName (const std::string& bodyName);

      /// Get selected bodies
      QList<BodyTreeItem*> selectedBodies() const;

      /// \}

    protected slots:
      /// Display the context menu for one item.
      /// \param pos position of the item
      void customContextMenu (const QPoint& pos);

      void currentChanged (const QModelIndex &current,
                           const QModelIndex &previous);

    private:
      /// Handle a selection event
      ///
      /// Does not re-emit a selection event when the body tree selection
      /// is updated.
      void handleSelectionEvent (const SelectionEvent* event);

      void updatePropertyArea (BodyTreeItem* item);

      QTreeView* view_;
      QStandardItemModel* model_;
      WindowsManagerPtr_t osg_;
      QWidget* propertyArea_;
    };
  }
}

#endif // GEPETTO_GUI_BODYTREEWIDGET_HH
