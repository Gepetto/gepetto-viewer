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

#include <decorator.hh>
#include <wrapper/qcp.hh>

#include <PythonQt.h>

void registerQCustomPlot ()
{
  PythonQt::self()->registerCPPClass ("QCP", "", "QCustomPlot", PythonQtCreateObject<wrapper::PyQCP>, NULL);

  PythonQt::self()->registerClass (&QCustomPlot     ::staticMetaObject    , "QCustomPlot");
  PythonQt::self()->registerClass (&QCPPainter      ::staticMetaObject    , "QCustomPlot");
  PythonQt::self()->registerClass (&QCPLayer        ::staticMetaObject    , "QCustomPlot");

  PythonQt::self()->registerClass (&QCPLayerable    ::staticMetaObject, "QCustomPlot");

  PythonQt::self()->registerClass (&QCPAbstractPlottable::staticMetaObject, "QCustomPlot");
  PythonQt::self()->registerClass (&QCPBars             ::staticMetaObject, "QCustomPlot");
  PythonQt::self()->registerClass (&QCPColorMap         ::staticMetaObject, "QCustomPlot");
  PythonQt::self()->registerClass (&QCPCurve            ::staticMetaObject, "QCustomPlot");
  PythonQt::self()->registerClass (&QCPFinancial        ::staticMetaObject, "QCustomPlot");
  PythonQt::self()->registerClass (&QCPGraph            ::staticMetaObject, "QCustomPlot");

  PythonQt::self()->registerClass (&QCPAbstractItem ::staticMetaObject, "QCustomPlot");
  PythonQt::self()->registerClass (&QCPItemBracket     ::staticMetaObject , "QCustomPlot");
  PythonQt::self()->registerClass (&QCPItemCurve       ::staticMetaObject , "QCustomPlot");
  PythonQt::self()->registerClass (&QCPItemEllipse     ::staticMetaObject , "QCustomPlot");
  PythonQt::self()->registerClass (&QCPItemLine        ::staticMetaObject , "QCustomPlot");
  PythonQt::self()->registerClass (&QCPItemPixmap      ::staticMetaObject , "QCustomPlot");
  PythonQt::self()->registerClass (&QCPItemRect        ::staticMetaObject , "QCustomPlot");
  PythonQt::self()->registerClass (&QCPItemStraightLine::staticMetaObject , "QCustomPlot");
  PythonQt::self()->registerClass (&QCPItemText        ::staticMetaObject , "QCustomPlot");
  PythonQt::self()->registerClass (&QCPItemRichText    ::staticMetaObject, "QCustomPlot");
  PythonQt::self()->registerClass (&QCPItemTracer      ::staticMetaObject , "QCustomPlot");
  //PythonQt::self()->addParentClass ("QCPItemRichText", "QCPItemText", PythonQtUpcastingOffset<QCPItemRichText,QCPItemText>());

  PythonQt::self()->registerClass (&QCPLayoutElement::staticMetaObject, "QCustomPlot");
  PythonQt::self()->registerClass (&QCPLayout       ::staticMetaObject, "QCustomPlot");
  PythonQt::self()->registerClass (&QCPAxisRect     ::staticMetaObject, "QCustomPlot");
  PythonQt::self()->registerClass (&QCPLayoutGrid   ::staticMetaObject, "QCustomPlot");
  PythonQt::self()->registerClass (&QCPLegend       ::staticMetaObject, "QCustomPlot");
  PythonQt::self()->registerClass (&QCPAxis         ::staticMetaObject, "QCustomPlot");

  PythonQt::self()->addDecorators (new QCustomPlotDecorator ());
  PythonQt::self()->registerCPPClass ("QCPPainter"           , "QPainter", "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCustomPlot"          , "QWidget" , "QCustomPlot");
  // PythonQt::self()->registerCPPClass ("QCPAxisPainterPrivate", "QGraphicsItem" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPLayer"             , "QObject" , "QCustomPlot");
  // PythonQt::self()->registerCPPClass ("QCPPlotTitle"         , "QGraphicsItem" , "QCustomPlot");
  // PythonQt::self()->registerCPPClass ("QCPAbstractLegendItem", "QGraphicsItem" , "QCustomPlot");
  // PythonQt::self()->registerCPPClass ("QCPColorMap"          , "QGraphicsItem" , "QCustomPlot");
  // PythonQt::self()->registerCPPClass ("QCPColorScale"        , "QGraphicsItem" , "QCustomPlot");
  // PythonQt::self()->registerCPPClass ("QCPBars"              , "QGraphicsItem" , "QCustomPlot");

  PythonQt::self()->registerCPPClass ("QCPItemAnchor"        , NULL , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPItemPosition"      , "QCPItemAnchor" , "QCustomPlot");

  // Children of QCPLayerable
  PythonQt::self()->registerCPPClass ("QCPLayerable"         , "QObject" , "QCustomPlot");

  PythonQt::self()->registerCPPClass ("QCPAbstractItem"      , "QCPLayerable" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPItemBracket"       , "QCPAbstractItem" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPItemCurve"         , "QCPAbstractItem" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPItemEllipse"       , "QCPAbstractItem" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPItemLine"          , "QCPAbstractItem" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPItemPixmap"        , "QCPAbstractItem" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPItemRect"          , "QCPAbstractItem" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPItemStraightLine"  , "QCPAbstractItem" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPItemText"          , "QCPAbstractItem" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPItemTracer"        , "QCPAbstractItem" , "QCustomPlot");

  PythonQt::self()->registerCPPClass ("QCPItemRichText"      , "QCPItemText"     , "QCustomPlot");

  PythonQt::self()->registerCPPClass ("QCPAbstractPlottable" , "QCPLayerable" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPBars"              , "QCPAbstractPlottable" , "QCustomPlot");
  // PythonQt::self()->registerCPPClass ("QCPColorMap"          , "QCPAbstractPlottable" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPCurve"             , "QCPAbstractPlottable" , "QCustomPlot");
  // PythonQt::self()->registerCPPClass ("QCPFinancial"         , "QCPAbstractPlottable" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPGraph"             , "QCPAbstractPlottable" , "QCustomPlot");
  // PythonQt::self()->registerCPPClass ("QCPStatisticalBox"    , "QCPAbstractPlottable" , "QCustomPlot");

  PythonQt::self()->registerCPPClass ("QCPLayoutElement"     , "QCPLayerable" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPLayout"            , "QCPLayoutElement" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPAxisRect"          , "QCPLayoutElement" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPLayoutGrid"        , "QCPLayout" , "QCustomPlot");
  PythonQt::self()->registerCPPClass ("QCPLegend"            , "QCPLayoutGrid" , "QCustomPlot");

  PythonQt::self()->registerCPPClass ("QCPAxis"              , "QCPLayerable" , "QCustomPlot");
}
