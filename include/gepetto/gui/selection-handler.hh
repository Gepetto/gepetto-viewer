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

#ifndef GEPETTO_GUI_SELECTION_HANDLER_H__
# define GEPETTO_GUI_SELECTION_HANDLER_H__

#include <QWidget>
#include <QComboBox>
#include <QVector3D>
#include <QKeyEvent>

#include <gepetto/gui/fwd.hh>

namespace gepetto {
  namespace gui {
    /// Base class to define selection mode.
    /// \note The class has no pure virtual method in order to be used in python.
    class SelectionMode : public QObject
    {
      Q_OBJECT
    public:
      SelectionMode(WindowsManagerPtr_t wsm) 
	: wsm_(wsm)
      {}
      ~SelectionMode() {}

      virtual void reset() { currentSelected_ = ""; }

      QString currentBody () { return currentSelected_; }

    signals:
      void selectedBodies(QStringList selectedBodies);

    public slots:
      /// Slot called when a body is selected.
      /// \param name body's name
      /// \param position click position
      virtual void onSelect(SelectionEvent* event) = 0;

      virtual QString getName() { return "None"; }

    protected:
      QString currentSelected_;
      WindowsManagerPtr_t wsm_;
    };

    /// Class that allows to select one body.
    class UniqueSelection : public SelectionMode
    {
      Q_OBJECT
    public:
      UniqueSelection(WindowsManagerPtr_t wsm);
      ~UniqueSelection();

    public slots:
      virtual void onSelect(SelectionEvent* event);
      virtual QString getName() { return "Unique"; }
    };

    class MultiSelection : public SelectionMode
    {
      Q_OBJECT
    public:
      MultiSelection(WindowsManagerPtr_t wsm);
      ~MultiSelection();

      virtual void reset();

    public slots:
      virtual void onSelect(SelectionEvent* event);
      virtual QString getName() { return "Multi"; }

    protected:
      QStringList selectedBodies_;
    };

    class SelectionHandler : public QComboBox
    {
      Q_OBJECT
    public:
      SelectionHandler(WindowsManagerPtr_t wsm, QWidget* parent = 0);
      ~SelectionHandler();

      SelectionMode* mode ();

    public slots:
      /// Add a mode to the list of available mode.
      /// \param mode mode to add
      void addMode(SelectionMode *mode);

    private slots:
      void getBodies(QStringList selectedBodies);
      void changeMode(int index);

    private:
      void initWidget();

      std::vector<SelectionMode *> modes_;
      int index_;
      WindowsManagerPtr_t wsm_;
      QStringList selected_;
    };
  }
}

#endif /* GEPETTO_GUI_SELECTION_HANDLER_H__ */
