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

#ifndef GEPETTO_GUI_ACTION_SEARCH_BAR_HH
#define GEPETTO_GUI_ACTION_SEARCH_BAR_HH

#include <gepetto/gui/fwd.hh>
// This include must be include before any other Qt include for GLDEBUGPROC
#include <QLineEdit>
#include <QMap>
#include <QString>
#include <QStringListModel>

namespace gepetto {
namespace gui {
class ActionSearchBar : public QLineEdit {
  Q_OBJECT

 public:
  ActionSearchBar(QWidget* parent);

  void addAction(QAction* action);

  QAction* showAction() const { return showAction_; }

 protected:
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void showEvent(QShowEvent* event);

 private slots:
  bool trigger(const QString& s);
  void handleReturnPressed();

 private:
  QStringListModel* model_;
  QCompleter* completer_;
  QMap<QString, QAction*> actions_;
  QAction* showAction_;
};
}  // namespace gui
}  // namespace gepetto

#endif  // GEPETTO_GUI_ACTION_SEARCH_BAR_HH
