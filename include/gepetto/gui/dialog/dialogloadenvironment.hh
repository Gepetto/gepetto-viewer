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

#ifndef GEPETTO_GUI_DIALOGLOADENVIRONMENT_HH
#define GEPETTO_GUI_DIALOGLOADENVIRONMENT_HH

#include <QDialog>
#include <QComboBox>

namespace Ui {
  class DialogLoadEnvironment;
}

namespace gepetto {
  namespace gui {
    class DialogLoadEnvironment : public QDialog
    {
      Q_OBJECT

      public:
        explicit DialogLoadEnvironment(QWidget *parent = 0);
        ~DialogLoadEnvironment();

        struct EnvironmentDefinition {
          QString name_, envName_, urdfFilename_, package_, urdfSuf_,
	    srdfSuf_;
          EnvironmentDefinition () {}
          EnvironmentDefinition (QString name, QString envName,
	    QString package,
	    QString urdfFilename, QString urdfSuffix, QString srdfSuffix) :
            name_(name), envName_ (envName), urdfFilename_(urdfFilename),
            package_ (package), urdfSuf_(urdfSuffix), srdfSuf_(srdfSuffix)
          {}
        };

        static void addEnvironmentDefinition (QString name,
            QString envName,
            QString package,
            QString urdfFilename,
	    QString urdfSuffix,
	    QString srdfSuffix);
        static QList <EnvironmentDefinition> getEnvironmentDefinitions ();

        EnvironmentDefinition getSelectedDescription () {
          return selected_;
        }

        private slots:
          void accept();
        void envSelect(int index);

      private:
        ::Ui::DialogLoadEnvironment *ui_;
        QComboBox* defs_;
        EnvironmentDefinition selected_;

        static QList <EnvironmentDefinition> definitions;
    };
  } // namespace gui
} // namespace gepetto

Q_DECLARE_METATYPE (gepetto::gui::DialogLoadEnvironment::EnvironmentDefinition)

#endif // GEPETTO_GUI_DIALOGLOADENVIRONMENT_HH
