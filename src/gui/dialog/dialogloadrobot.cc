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

#include "gepetto/gui/dialog/dialogloadrobot.hh"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_dialogloadrobot.h"

namespace gepetto {
namespace gui {
QStringList DialogLoadRobot::rootJointTypes =
    QStringList() << "freeflyer" << "planar" << "anchor";

QList<DialogLoadRobot::RobotDefinition> DialogLoadRobot::definitions =
    QList<DialogLoadRobot::RobotDefinition>()
    << DialogLoadRobot::RobotDefinition();

DialogLoadRobot::DialogLoadRobot(QWidget *parent)
    : QDialog(parent), ui_(new ::Ui::DialogLoadRobot) {
  ui_->setupUi(this);
  ui_->groupBox_details->setVisible(false);

  ui_->rootJointType->addItems(rootJointTypes);
  defs_ = ui_->comboBox_defs;

  foreach (RobotDefinition r, definitions) {
    defs_->addItem(r.name_, QVariant::fromValue(r));
  }
}

DialogLoadRobot::~DialogLoadRobot() { delete ui_; }

void DialogLoadRobot::addRobotDefinition(QString name, QString robotName,
                                         QString rootJointType,
                                         QString modelName, QString package,
                                         QString urdfSuffix,
                                         QString srdfSuffix) {
  definitions.append(RobotDefinition(name, robotName, rootJointType, modelName,
                                     package, urdfSuffix, srdfSuffix));
}

QList<DialogLoadRobot::RobotDefinition> DialogLoadRobot::getRobotDefinitions() {
  return definitions;
}

void DialogLoadRobot::accept() {
  /* TODO use the urdfParser::getFilename to check if the package exists
  QDir d (ui_->packagePath->text ());
  if (!d.cd("urdf")) {
    QMessageBox (QMessageBox::Warning, "Directory not found", d.absolutePath(),
  QMessageBox::Ok, this).exec(); return;
  }
  if (!d.exists(ui_->modelName->text () + ui_->urdfSuffix->text () + ".urdf")) {
    QMessageBox (QMessageBox::Warning, "File not found",
  d.absoluteFilePath(ui_->modelName->text () + ui_->urdfSuffix->text () +
  ".urdf"), QMessageBox::Ok, this).exec(); return;
  }
  if (!QDir (ui_->meshDirectory->text ()).exists()) {
    QMessageBox (QMessageBox::Warning, "File not found",
  ui_->meshDirectory->text (), QMessageBox::Ok, this).exec(); return;
  }*/
  selected_ = RobotDefinition(defs_->currentText(), ui_->robotName->text(),
                              ui_->rootJointType->currentText(),
                              ui_->modelName->text(), ui_->packageName->text(),
                              ui_->urdfSuffix->text(), ui_->srdfSuffix->text());
  done(QDialog::Accepted);
}

void DialogLoadRobot::robotSelect(int index) {
  QVariant v = defs_->itemData(index);
  if (v.canConvert<RobotDefinition>()) {
    RobotDefinition rd = v.value<RobotDefinition>();
    ui_->robotName->setText(rd.robotName_);
    ui_->modelName->setText(rd.modelName_);
    if (rootJointTypes.contains(rd.rootJointType_))
      ui_->rootJointType->setCurrentIndex(
          rootJointTypes.indexOf(rd.rootJointType_));
    ui_->packageName->setText(rd.package_);
    ui_->urdfSuffix->setText(rd.urdfSuf_);
    ui_->srdfSuffix->setText(rd.srdfSuf_);
  }
}
}  // namespace gui
}  // namespace gepetto
