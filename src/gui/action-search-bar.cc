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

#include <QAbstractItemView>
#include <QAction>
#include <QCompleter>
#include <QDebug>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <gepetto/gui/action-search-bar.hh>

namespace gepetto {
namespace gui {
ActionSearchBar::ActionSearchBar(QWidget* grandparent)
    : QLineEdit(NULL),
      model_(new QStringListModel(this)),
      completer_(new QCompleter(model_, this)),
      showAction_(new QAction("Open action search bar popup", this)) {
  setPlaceholderText("Type here");
  setCompleter(completer_);
  completer_->setCaseSensitivity(Qt::CaseInsensitive);
  showAction_->setShortcut(Qt::Key_M);
  showAction_->setShortcutContext(Qt::WidgetWithChildrenShortcut);

  connect(this, SIGNAL(returnPressed()), SLOT(handleReturnPressed()));

  QWidget* popup(new QWidget(grandparent));
  popup->setWindowFlags(Qt::Popup);
  popup->connect(showAction_, SIGNAL(triggered()), SLOT(show()));
  QHBoxLayout* layout = new QHBoxLayout(popup);
  layout->addWidget(this);
  layout->setContentsMargins(0, 0, 0, 0);
}

void ActionSearchBar::addAction(QAction* action) {
  QString t = action->text();
  t.remove('&');
  if (actions_.contains(t)) {
    actions_[t] = action;
  } else {
    actions_[t] = action;
    model_->setStringList(model_->stringList() << t);
  }
}

void ActionSearchBar::keyPressEvent(QKeyEvent* event) {
  switch (event->key()) {
    case Qt::Key_Escape:
      parentWidget()->close();
      return;
    case Qt::Key_Enter:
      handleReturnPressed();
      return;
    case Qt::Key_Tab:
      // Autocomplete
      if (completer_->completionCount() > 0)
        setText(completer_->currentCompletion());
      return;
    default:
      QLineEdit::keyPressEvent(event);
      return;
  }
}

void ActionSearchBar::showEvent(QShowEvent*) {
  parentWidget()->move(QCursor::pos());
  setFocus(Qt::PopupFocusReason);
  selectAll();
}

bool ActionSearchBar::trigger(const QString& s) {
  // qDebug() << "Try action " << s;
  if (!actions_.contains(s)) return false;
  actions_[s]->trigger();
  // qDebug() << "Action " << s;
  parentWidget()->close();
  return true;
}

void ActionSearchBar::handleReturnPressed() {
  trigger(text());
  parentWidget()->close();
}
}  // namespace gui
}  // namespace gepetto
