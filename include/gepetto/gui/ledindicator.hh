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

#ifndef GEPETTO_GUI_LEDINDICATOR_HH
#define GEPETTO_GUI_LEDINDICATOR_HH

#include <QWidget>

namespace gepetto {
  namespace gui {
    class LedIndicator : public QWidget
    {
      Q_OBJECT

      public:
        LedIndicator (QWidget *parent = 0);

signals:
        void switched (bool on);
        void mouseClickEvent ();

        public slots:
          void switchLed();
        void switchLed(bool on);

      protected:
        void paintEvent(QPaintEvent *);
        void mouseReleaseEvent(QMouseEvent* event);

      private:
        bool lit;
        const int width, height;
    };
  } // namespace gui
} // namespace gepetto

#endif // GEPETTO_GUI_LEDINDICATOR_HH
