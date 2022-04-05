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

#include <QCoreApplication>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QSettings>
#include <QVBoxLayout>
#include <gepetto/gui/shortcut-factory.hh>
#include <iostream>

namespace gepetto {
namespace gui {
ShortcutFactory::ShortcutFactory() : widgetsBindings_(MapBindings()) {
  readShortcutsFile();
}

ShortcutFactory::~ShortcutFactory() { writeShortcutsFile(); }

void ShortcutFactory::addBinding(QString widgetName, QString actionName,
                                 QAction* action) {
  if (widgetsBindings_.find(widgetName) == widgetsBindings_.end()) {
    widgetsBindings_.insert(std::make_pair(widgetName, BindingList()));
  }
  widgetsBindings_[widgetName].push_back(Binding(actionName, action));
  if (saved_.find(widgetName + "-" + actionName) != saved_.end()) {
    action->setShortcut(saved_[widgetName + "-" + actionName]);
  }
}

void ShortcutFactory::open() {
  QScrollArea* s = new QScrollArea(NULL);
  QWidget* w = new QWidget(s, Qt::Window);
  QVBoxLayout* l = new QVBoxLayout(w);

  l->addWidget(
      new QLabel("Double click on the button to change the shortcut."));
  s->setWidget(w);
  s->setAttribute(Qt::WA_DeleteOnClose);
  s->setWidgetResizable(true);
  s->resize(400, 500);
  for (MapBindings::iterator it = widgetsBindings_.begin();
       it != widgetsBindings_.end(); ++it) {
    QGroupBox* g = new QGroupBox((*it).first, w);
    QFormLayout* f = new QFormLayout(g);
    for (BindingList::iterator itL = (*it).second.begin();
         itL != (*it).second.end(); ++itL) {
      f->addRow((*itL).first, new ShortcutButton((*itL).second, g));
    }
    l->addWidget(g);
  }
  s->show();
}

void ShortcutFactory::writeShortcutsFile() {
  QSettings shortcut(QSettings::SystemScope,
                     QCoreApplication::organizationName(),
                     QString::fromStdString("shortcuts"));
  if (!shortcut.isWritable()) {
    return;
  }
  for (MapBindings::iterator it = widgetsBindings_.begin();
       it != widgetsBindings_.end(); ++it) {
    shortcut.beginGroup((*it).first);
    for (BindingList::iterator itL = (*it).second.begin();
         itL != (*it).second.end(); ++itL) {
      shortcut.setValue((*itL).first, (*itL).second->shortcut().toString());
    }
    shortcut.endGroup();
  }
}

void ShortcutFactory::readShortcutsFile() {
  QSettings robot(QSettings::SystemScope, QCoreApplication::organizationName(),
                  QString::fromStdString("shortcuts"));
  if (robot.status() != QSettings::NoError) {
    return;
  }
  foreach (QString name, robot.childGroups()) {
    robot.beginGroup(name);
    foreach (QString child, robot.childKeys()) {
      saved_.insert(std::make_pair(name + "-" + child,
                                   robot.value(child, "").toString()));
    }
    robot.endGroup();
  }
}

ShortcutFactory::ShortcutButton::ShortcutButton(QAction* action,
                                                QWidget* parent)
    : QPushButton(parent) {
  action_ = action;
  hasFocus_ = false;
  modifiers_.push_back(Qt::Key_Shift);
  modifiers_.push_back(Qt::Key_Control);
  modifiers_.push_back(Qt::Key_Meta);
  modifiers_.push_back(Qt::Key_Alt);

  setText(action->shortcut().toString());
}

ShortcutFactory::ShortcutButton::~ShortcutButton() {
  if (hasFocus_) releaseKeyboard();
}

void ShortcutFactory::ShortcutButton::mouseDoubleClickEvent(
    QMouseEvent* event) {
  if (!hasFocus_ && event->button() == Qt::LeftButton) {
    grabKeyboard();
    setText("Changing");
    hasFocus_ = true;
  }
}

void ShortcutFactory::ShortcutButton::keyPressEvent(QKeyEvent* event) {
  if (hasFocus_) {
    if (modifiers_.indexOf(event->key()) == -1) {
      int seq = event->key() + event->modifiers();

      action_->setShortcut(seq);
      setText(action_->shortcut().toString(QKeySequence::NativeText));
      hasFocus_ = false;
      releaseKeyboard();
    }
  }
}
}  // namespace gui
}  // namespace gepetto
