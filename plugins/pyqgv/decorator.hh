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

#include "qgv.h"
#include <QObject>

class QGVScene;
class QGVSubGraph;
class QGVNode;
class QGVEdge;

namespace PyQgv {
  class QGVCORE_EXPORT QGVDecorator : public QObject
  {
    Q_OBJECT

    public slots:

    // ------- QGVScene ------------------------------------------- //
    QGVScene* new_QGVScene(const QString &name, QObject *parent = NULL);
    void delete_QGVScene(QGVScene* s);

    void setGraphAttribute(QGVScene* s, const QString &name, const QString &value);
    void setNodeAttribute (QGVScene* s, const QString &name, const QString &value);
    void setEdgeAttribute (QGVScene* s, const QString &name, const QString &value);

    QGVNode* addNode(QGVScene* s, const QString& label);
    QGVEdge* addEdge(QGVScene* s, QGVNode* source, QGVNode* target, const QString label = QString());
    QGVSubGraph* addSubGraph(QGVScene* s, const QString& name, bool cluster=true);

    void setRootNode(QGVScene* s, QGVNode *node);
    void setNodePositionAttribute (QGVScene* s);

    void loadLayout (QGVScene* s, const QString &text);
    void applyLayout(QGVScene* s, const QString &algorithm = "dot");
    void render     (QGVScene* s, const QString &algorithm);
    void render     (QGVScene* s, const QString &algorithm, const QString file);
    void freeLayout (QGVScene* s);
    void clear      (QGVScene* s);
    // ------- QGVScene ------------------------------------------- //

    // ------- QGVSubGraph ---------------------------------------- //
    void setAttribute (QGVSubGraph* n, const QString &name, const QString &value);
    QString getAttribute (QGVSubGraph* n, const QString &name);

    QGVNode* addNode(QGVSubGraph* s, const QString& label);
    QGVSubGraph* addSubGraph(QGVSubGraph* s, const QString& name, bool cluster=true);
    // ------- QGVSubGraph ---------------------------------------- //

    // ------- QGVNode  ------------------------------------------- //
    void setAttribute (QGVNode* n, const QString &name, const QString &value);
    QString getAttribute (QGVNode* n, const QString &name);
    // ------- QGVNode  ------------------------------------------- //

    // ------- QGVEdge  ------------------------------------------- //
    void setAttribute (QGVEdge* e, const QString &name, const QString &value);
    QString getAttribute (QGVEdge* e, const QString &name);
    // ------- QGVEdge  ------------------------------------------- //
  };

  /// Register QGV to PythonQt
  void registerQGV ();
} // namespace PyQgv
