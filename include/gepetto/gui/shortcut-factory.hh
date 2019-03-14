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

#ifndef GEPETTO_GUI_SHORTCUT_FACTORY_HH_
# define GEPETTO_GUI_SHORTCUT_FACTORY_HH_

#include <QAction>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPushButton>

#include <map>
#include <utility>

namespace gepetto {
  namespace gui {
    class ShortcutFactory : public QObject {
      Q_OBJECT
    public:
      ShortcutFactory();
      ~ShortcutFactory();

    public:
      /// Add a binding to a given widget.
      /// If widgetName doesn't exists, then it will be add.
      /// If actionName exists, do nothing
      /// \param widgetName name of the corresponding widget
      /// \param actionName name of the action
      /// \param action instance of the action
      void addBinding(QString widgetName, QString actionName, QAction* action);

    public slots:
      /// Open the widget that allows to change the different shortcut
      void open();

    private:
      void writeShortcutsFile();
      void readShortcutsFile();

      typedef std::pair<QString, QAction*> Binding;
      typedef std::list<Binding> BindingList;
      typedef std::map<QString, BindingList> MapBindings;

      class ShortcutButton : public QPushButton {
      public:
	ShortcutButton(QAction* action, QWidget* parent = 0);
	~ShortcutButton();

      protected:
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	virtual void keyPressEvent(QKeyEvent* event);

	bool hasFocus_;
        QList<int> modifiers_;
	QAction* action_;
      };

      MapBindings widgetsBindings_;
      std::map<QString, QKeySequence> saved_;
    };
  }
}

#endif /* GEPETTO_GUI_SHORTCUT_FACTORY_HH_ */
