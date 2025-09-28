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

#include "gepetto/gui/dialog/dialogloadenvironment.hh"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_dialogloadenvironment.h"

namespace gepetto {
namespace gui {
QList<DialogLoadEnvironment::EnvironmentDefinition>
    DialogLoadEnvironment::definitions =
        QList<DialogLoadEnvironment::EnvironmentDefinition>()
        << DialogLoadEnvironment::EnvironmentDefinition();

DialogLoadEnvironment::DialogLoadEnvironment(QWidget* parent)
    : QDialog(parent), ui_(new ::Ui::DialogLoadEnvironment) {
  ui_->setupUi(this);
  ui_->groupBox_details->setVisible(false);
  defs_ = ui_->comboBox_defs;
  defs_->setFocus();

  foreach (EnvironmentDefinition r, definitions) {
    defs_->addItem(r.name_, QVariant::fromValue(r));
  }
}

DialogLoadEnvironment::~DialogLoadEnvironment() { delete ui_; }

void DialogLoadEnvironment::addEnvironmentDefinition(
    QString name, QString envName, QString package, QString urdfFilename,
    QString urdfSuffix, QString srdfSuffix) {
  definitions.append(EnvironmentDefinition(name, envName, package, urdfFilename,
                                           urdfSuffix, srdfSuffix));
}

QList<DialogLoadEnvironment::EnvironmentDefinition>
DialogLoadEnvironment::getEnvironmentDefinitions() {
  return definitions;
}

void DialogLoadEnvironment::accept() {
  /* TODO use the urdfParser::getFilename to check if the package exists
  QDir d (ui_->packagePath->text ());
  if (!d.cd("urdf")) {
    QMessageBox (QMessageBox::Warning, "Directory not found", d.absolutePath(),
  QMessageBox::Ok, this).exec(); return;
  }
  if (!d.exists(ui_->urdfFilename->text () + ".urdf")) {
    QMessageBox (QMessageBox::Warning, "File not found",
  d.absoluteFilePath(ui_->urdfFilename->text() + ".urdf"), QMessageBox::Ok,
  this).exec(); return;
  }
  if (!QDir (ui_->meshDirectory->text ()).exists()) {
    QMessageBox (QMessageBox::Warning, "File not found",
  ui_->meshDirectory->text (), QMessageBox::Ok, this).exec(); return;
  }*/
  selected_ =
      EnvironmentDefinition(defs_->currentText(), ui_->envName->text(),
                            ui_->packageName->text(), ui_->urdfFilename->text(),
                            ui_->urdfSuffix->text(), ui_->srdfSuffix->text());
  done(QDialog::Accepted);
}

void DialogLoadEnvironment::envSelect(int index) {
  QVariant v = defs_->itemData(index);
  if (v.canConvert<EnvironmentDefinition>()) {
    EnvironmentDefinition rd = v.value<EnvironmentDefinition>();
    ui_->envName->setText(rd.envName_);
    ui_->packageName->setText(rd.package_);
    ui_->urdfFilename->setText(rd.urdfFilename_);
    ui_->urdfSuffix->setText(rd.urdfSuf_);
    ui_->srdfSuffix->setText(rd.srdfSuf_);
  }
}
}  // namespace gui
}  // namespace gepetto
