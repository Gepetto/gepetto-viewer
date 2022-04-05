// Copyright (c) 2018, Joseph Mirabel
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// gepetto-viewer. If not, see <http://www.gnu.org/licenses/>.

#include <PythonQt.h>
#include <QGVEdge.h>
#include <QGVNode.h>
#include <QGVScene.h>
#include <QGVSubGraph.h>

#include <decorator.hh>

namespace PyQgv {

// ------- QGVScene ------------------------------------------- //
QGVScene* QGVDecorator::new_QGVScene(const QString& name, QObject* parent) {
  return new QGVScene(name, parent);
}
void QGVDecorator::delete_QGVScene(QGVScene* s) { delete s; }

void QGVDecorator::setGraphAttribute(QGVScene* s, const QString& name,
                                     const QString& value) {
  s->setGraphAttribute(name, value);
}
void QGVDecorator::setNodeAttribute(QGVScene* s, const QString& name,
                                    const QString& value) {
  s->setNodeAttribute(name, value);
}
void QGVDecorator::setEdgeAttribute(QGVScene* s, const QString& name,
                                    const QString& value) {
  s->setEdgeAttribute(name, value);
}

QGVNode* QGVDecorator::addNode(QGVScene* s, const QString& label) {
  return s->addNode(label);
}
QGVEdge* QGVDecorator::addEdge(QGVScene* s, QGVNode* source, QGVNode* target,
                               const QString label) {
  return s->addEdge(source, target, label);
}
QGVSubGraph* QGVDecorator::addSubGraph(QGVScene* s, const QString& name,
                                       bool cluster) {
  return s->addSubGraph(name, cluster);
}

void QGVDecorator::setRootNode(QGVScene* s, QGVNode* node) {
  s->setRootNode(node);
}
void QGVDecorator::setNodePositionAttribute(QGVScene* s) {
  s->setNodePositionAttribute();
}

void QGVDecorator::loadLayout(QGVScene* s, const QString& text) {
  s->loadLayout(text);
}
void QGVDecorator::applyLayout(QGVScene* s, const QString& algorithm) {
  s->applyLayout(algorithm);
}
void QGVDecorator::render(QGVScene* s, const QString& algorithm) {
  s->render(algorithm);
}
void QGVDecorator::render(QGVScene* s, const QString& algorithm,
                          const QString file) {
  s->render(algorithm, file);
}
bool QGVDecorator::writeGraph(QGVScene* s, const QString& filename) {
  return s->writeGraph(filename);
}
void QGVDecorator::freeLayout(QGVScene* s) { s->freeLayout(); }
void QGVDecorator::clear(QGVScene* s) { s->clear(); }
// ------- QGVScene ------------------------------------------- //

// ------- QGVScene ------------------------------------------- //
void QGVDecorator::setAttribute(QGVSubGraph* s, const QString& name,
                                const QString& value) {
  s->setAttribute(name, value);
}
QString QGVDecorator::getAttribute(QGVSubGraph* s, const QString& name) {
  return s->getAttribute(name);
}

QGVNode* QGVDecorator::addNode(QGVSubGraph* s, const QString& label) {
  return s->addNode(label);
}
QGVSubGraph* QGVDecorator::addSubGraph(QGVSubGraph* s, const QString& name,
                                       bool cluster) {
  return s->addSubGraph(name, cluster);
}
// ------- QGVScene ------------------------------------------- //

// ------- QGVNode  ------------------------------------------- //
void QGVDecorator::setAttribute(QGVNode* n, const QString& name,
                                const QString& value) {
  n->setAttribute(name, value);
}
QString QGVDecorator::getAttribute(QGVNode* n, const QString& name) {
  return n->getAttribute(name);
}
// ------- QGVNode  ------------------------------------------- //

// ------- QGVEdge  ------------------------------------------- //
void QGVDecorator::setAttribute(QGVEdge* e, const QString& name,
                                const QString& value) {
  e->setAttribute(name, value);
}
QString QGVDecorator::getAttribute(QGVEdge* e, const QString& name) {
  return e->getAttribute(name);
}
// ------- QGVEdge  ------------------------------------------- //

void registerQGV() {
  PythonQt::self()->addDecorators(new QGVDecorator());
  PythonQt::self()->registerCPPClass("QGVScene", "QGraphicsScene", "QGraphViz");
  PythonQt::self()->registerCPPClass("QGVSubGraph", "QGraphicsItem",
                                     "QGraphViz");
  PythonQt::self()->registerCPPClass("QGVNode", "QGraphicsItem", "QGraphViz");
  PythonQt::self()->registerCPPClass("QGVEdge", "QGraphicsItem", "QGraphViz");
}

}  // namespace PyQgv
