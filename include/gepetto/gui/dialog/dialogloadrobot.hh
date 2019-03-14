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

#ifndef GEPETTO_GUI_DIALOGLOADROBOT_HH
#define GEPETTO_GUI_DIALOGLOADROBOT_HH

#include <QDialog>
#include <QComboBox>

namespace Ui {
  class DialogLoadRobot;
}

namespace gepetto {
  namespace gui {
    class DialogLoadRobot : public QDialog
    {
      Q_OBJECT

      public:
        struct RobotDefinition {
          QString name_, robotName_, urdfSuf_, srdfSuf_, package_, modelName_, rootJointType_;
          RobotDefinition () : rootJointType_ ("Freeflyer") {}
          RobotDefinition (QString name,
              QString robotName,
              QString rootJointType,
              QString modelName,
              QString package,
              QString urdfSuffix,
              QString srdfSuffix) :
            name_(name), robotName_ (robotName), urdfSuf_(urdfSuffix), srdfSuf_(srdfSuffix),
            package_ (package), modelName_ (modelName),
            rootJointType_ (rootJointType){}
        };

        explicit DialogLoadRobot(QWidget *parent = 0);
        ~DialogLoadRobot();

        static void addRobotDefinition (QString name,
            QString robotName,
            QString rootJointType,
            QString modelName,
            QString package,
            QString urdfSuffix,
            QString srdfSuffix);
        static QList <RobotDefinition> getRobotDefinitions ();

        RobotDefinition getSelectedRobotDescription () {
          return selected_;
        }

        private slots:
          void accept();
        void robotSelect(int index);

      private:
        ::Ui::DialogLoadRobot *ui_;
        QComboBox* defs_;
        RobotDefinition selected_;

        static QList <RobotDefinition> definitions;
        static QStringList rootJointTypes;
    };
  } // namespace gui
} // namespace gepetto

Q_DECLARE_METATYPE (gepetto::gui::DialogLoadRobot::RobotDefinition)

#endif // GEPETTO_GUI_DIALOGLOADROBOT_HH
