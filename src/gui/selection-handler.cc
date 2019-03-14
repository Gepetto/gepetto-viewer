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

#include <QHBoxLayout>
#include <QLabel>
#include <QVector3D>
#include <QAction>
#include <QDebug>

#include <gepetto/gui/bodytreewidget.hh>
#include <gepetto/gui/mainwindow.hh>
#include <gepetto/gui/selection-event.hh>
#include <gepetto/gui/windows-manager.hh>
#include "gepetto/gui/selection-handler.hh"

namespace gepetto {
  namespace gui {
    static const unsigned int LAST_SELECTED = 7;
    static const unsigned int SELECTED      = 8;
    static const unsigned int CLEAR_SELECT  = 0;

    SelectionHandler::SelectionHandler(WindowsManagerPtr_t wsm, QWidget *parent)
      : QComboBox(parent),
	index_(-1),
	wsm_(wsm)
    {
      connect(this, SIGNAL(currentIndexChanged(int)), SLOT(changeMode(int)));
    }

    SelectionHandler::~SelectionHandler()
    {
    }

    SelectionMode* SelectionHandler::mode ()
    {
      assert(index_ >= 0 && index_ < (int)modes_.size());
      return modes_[index_];
    }

    void SelectionHandler::changeMode(int index)
    {
      BodyTreeWidget* bt = MainWindow::instance()->bodyTree();
      foreach(QString name, selected_) {
        wsm_->setHighlight(name.toStdString(), CLEAR_SELECT);
      }
      if (index_ != -1) {
        modes_[index_]->reset();
        disconnect(bt, SIGNAL(bodySelected(SelectionEvent*)),
                   modes_[index_], SLOT(onSelect(SelectionEvent*)));
        disconnect(modes_[index_], SIGNAL(selectedBodies(QStringList)),
                   this, SLOT(getBodies(QStringList)));
      }
      index_ = index;
      connect(bt, SIGNAL(bodySelected(SelectionEvent*)),
              modes_[index], SLOT(onSelect(SelectionEvent*)),
              Qt::QueuedConnection);
      connect(modes_[index], SIGNAL(selectedBodies(QStringList)),
              SLOT(getBodies(QStringList)));
    }

    void SelectionHandler::addMode(SelectionMode* mode)
    {
      modes_.push_back(mode);
      addItem(mode->getName());
    }

    void SelectionHandler::getBodies(QStringList selectedBodies)
    {
      selected_ = selectedBodies;
    }

    UniqueSelection::UniqueSelection(WindowsManagerPtr_t wsm)
      : SelectionMode(wsm)
    {
    }

    UniqueSelection::~UniqueSelection()
    {
    }

    void UniqueSelection::onSelect(SelectionEvent* event)
    {
      if (!event) return;
      if (currentSelected_ == event->nodeName()) return;
      if (currentSelected_ != "") wsm_->setHighlight(currentSelected_.toStdString(), CLEAR_SELECT);
      currentSelected_ = event->nodeName();
      if (event->node()) {
        wsm_->setHighlight(event->node()->getID(), LAST_SELECTED);
        emit selectedBodies(QStringList() << currentSelected_);
      }
      event->done();
    }

    MultiSelection::MultiSelection(WindowsManagerPtr_t wsm)
      : SelectionMode(wsm)
    {
    }

    MultiSelection::~MultiSelection()
    {
    }

    void MultiSelection::reset()
    {
      SelectionMode::reset();
      selectedBodies_ = QStringList();
    }

    void MultiSelection::onSelect(SelectionEvent* event)
    {
      if (!event) return;
      int i = selectedBodies_.indexOf(event->nodeName());
      if (event->modKey() != Qt::ControlModifier) { // CTRL not pressed
        foreach (QString n, selectedBodies_) {
          wsm_->setHighlight(n.toStdString(), CLEAR_SELECT);
        }
        selectedBodies_.clear();
        currentSelected_ = event->nodeName();
        if (event->node()) {
          wsm_->setHighlight(event->node()->getID(), LAST_SELECTED);
          selectedBodies_ << currentSelected_;
        }
      } else {                                    // CTRL pressed
        if (!currentSelected_.isEmpty())
          wsm_->setHighlight(currentSelected_.toStdString(), SELECTED);
        if (i >= 0) {                             // Already selected.
          wsm_->setHighlight(event->node()->getID(), CLEAR_SELECT);
          currentSelected_ = "";
          selectedBodies_.removeAt(i);
        } else {
          currentSelected_ = event->nodeName();
          if (event->node()){   // Add to the list if not empty
            wsm_->setHighlight(event->node()->getID(), LAST_SELECTED);
            selectedBodies_ << currentSelected_;
          }
        }
      }
      qDebug() << selectedBodies_;
      emit selectedBodies(selectedBodies_);
      event->done();
    }
  }
}
