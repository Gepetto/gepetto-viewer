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

#include "gepetto/gui/ledindicator.hh"

#include <QColor>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>

namespace gepetto {
namespace gui {
LedIndicator::LedIndicator(QWidget *parent)
    : QWidget(parent), width(28), height(12) {
  setFixedSize(width, height);
  lit = false;
}

void LedIndicator::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.fillRect(0, 0, width, height, lit ? Qt::green : Qt::red);
}

void LedIndicator::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    emit mouseClickEvent();
  }
}

void LedIndicator::switchLed() {
  lit = !lit;
  repaint();
  emit switched(lit);
}

void LedIndicator::switchLed(bool on) {
  if (lit == on) return;
  switchLed();
}
}  // namespace gui
}  // namespace gepetto
