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

#ifndef GEPETTO_GUI_SAFEAPPLICATION_HH
#define GEPETTO_GUI_SAFEAPPLICATION_HH

#include <assert.h>

#include <gepetto/gui/fwd.hh>

#include <QApplication>

namespace gepetto {
  namespace gui {
    class SlotExceptionCatch
    {
      public:
        SlotExceptionCatch () : child_ (NULL) {}

        /// Inherited classes must call impl_notify method surronded by a try/catch
        virtual bool safeNotify (QApplication* app, QObject* receiver, QEvent* e) = 0;

        void addAsLeaf (SlotExceptionCatch* child)
        {
          if (child_ == NULL) child_ = child;
          else child_->addAsLeaf (child);
        }

      protected:
        bool impl_notify(QApplication* app, QObject* receiver, QEvent* e)
        {
          if (child_ == NULL)
            return app->QApplication::notify(receiver, e);
          else
            return child_->safeNotify(app, receiver, e);
        }

        SlotExceptionCatch* child_;
    };

    class SafeApplication : public QApplication, public SlotExceptionCatch
    {
      public:
        explicit SafeApplication (int& argc, char ** argv)
          : QApplication(argc, argv) {}

        bool notify(QObject* receiver, QEvent* e)
        {
          return impl_notify (this, receiver, e);
        }

      private:
        bool safeNotify (QApplication*, QObject*, QEvent*) { assert(false); return false; };
    };
  } // namespace gui
} // namespace gepetto

#endif // GEPETTO_GUI_SAFEAPPLICATION_HH
