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

#include <gepetto/viewer/group-node.h>
#include <gepetto/viewer/node.h>

#include <QDebug>
#include <QFormLayout>
#include <QtGlobal>
#include <gepetto/gui/bodytreewidget.hh>
#include <gepetto/gui/dialog/configuration.hh>

#include "gepetto/gui/mainwindow.hh"
#include "gepetto/gui/windows-manager.hh"

namespace gepetto {
namespace gui {
using viewer::PropertyPtr_t;
using viewer::ScopedLock;

BodyTreeItem::BodyTreeItem(QObject* parent, NodePtr_t node)
    : QObject(parent),
      QStandardItem(QString::fromStdString(
          node->getID().substr(node->getID().find_last_of("/") + 1))),
      node_(node) {
  setEditable(false);
}

void BodyTreeItem::initialize() {
  connect(this, SIGNAL(requestInitialize()), SLOT(doInitialize()));
  emit requestInitialize();
}

void BodyTreeItem::doInitialize() {
  propertyEditors_ = node_->guiEditor();
  BodyTreeWidget* bt = MainWindow::instance()->bodyTree();
  if (propertyEditors_->thread() != bt->thread())
    propertyEditors_->moveToThread(bt->thread());
  disconnect(SIGNAL(requestInitialize()));
}

BodyTreeItem::~BodyTreeItem() {
  if (propertyEditors_->parent() != NULL) delete propertyEditors_;
}

QStandardItem* BodyTreeItem::clone() const {
  return new BodyTreeItem(QObject::parent(), node_);
}

NodePtr_t BodyTreeItem::node() const { return node_; }

void BodyTreeItem::populateContextMenu(QMenu* contextMenu) {
  /// Landmark
  QMenu* lm = contextMenu->addMenu(tr("Landmark"));
  QAction* alm = lm->addAction(tr("Add"));
  QAction* dlm = lm->addAction(tr("Remove"));
  connect(alm, SIGNAL(triggered()), SLOT(addLandmark()));
  connect(dlm, SIGNAL(triggered()), SLOT(deleteLandmark()));
  /// Remove
  QAction* remove = contextMenu->addAction(tr("Remove"));
  connect(remove, SIGNAL(triggered()), SLOT(remove()));
  QAction* removeAll = contextMenu->addAction(tr("Remove all"));
  connect(removeAll, SIGNAL(triggered()), SLOT(removeAll()));
  if (!parentGroup_.empty()) {
    QAction* rfg = contextMenu->addAction(tr("Remove from group"));
    connect(rfg, SIGNAL(triggered()), SLOT(removeFromGroup()));
  }
}

void BodyTreeItem::setParentGroup(const std::string& parent) {
  parentGroup_ = parent;
}

void BodyTreeItem::setViewingMode(QString mode) {
  MainWindow::instance()->osg()->setWireFrameMode(node_->getID(),
                                                  mode.toLocal8Bit().data());
}

void BodyTreeItem::setVisibilityMode(QString mode) {
  MainWindow::instance()->osg()->setVisibility(node_->getID(),
                                               mode.toLocal8Bit().data());
}

void BodyTreeItem::removeFromGroup() {
  if (parentGroup_.empty()) return;
  MainWindow::instance()->osg()->removeFromGroup(node_->getID(), parentGroup_);
  QStandardItem::parent()->removeRow(row());
}

void BodyTreeItem::removeAll() {
  MainWindow* main = MainWindow::instance();
  main->osg()->deleteNode(node_->getID(), true);
}

void BodyTreeItem::remove() {
  MainWindow* main = MainWindow::instance();
  main->osg()->deleteNode(node_->getID(), false);
}

void BodyTreeItem::addLandmark() {
  MainWindow::instance()->osg()->addLandmark(node_->getID(), 0.05f);
}

void BodyTreeItem::deleteLandmark() {
  MainWindow::instance()->osg()->deleteLandmark(node_->getID());
}
}  // namespace gui
}  // namespace gepetto
