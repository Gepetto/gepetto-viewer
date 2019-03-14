#include <qcustomplot.h>
#include <qcpitemrichtext.hh>

#include <QObject>

void registerQCustomPlot ();

/// \sa See examples \ref pyplugin/qcustomplot_example_1.py
///     and \ref pyplugin/qcustomplot_example_2.py
class QCustomPlotDecorator : public QObject
{
  Q_OBJECT
  public Q_SLOTS:
    /// \name QCustomPlot
    /// \{
    QCustomPlot* new_QCustomPlot(QWidget* parent = 0)
    {
      return new QCustomPlot(parent);
    }
    void delete_QCustomPlot(QCustomPlot* o) //delete QCustomPlot object
    {
      delete o;
    }
    void clearGraphs(QCustomPlot* o)
    {
      o->clearGraphs();
    }

    QCPGraph* addGraph(QCustomPlot* o)
    {
      return o->addGraph();
    }
    void addPlottable(QCustomPlot* o, QCPAbstractPlottable* ap)
    {
      o->addPlottable(ap);
    }

    QCPGraph* graph(QCustomPlot* o, int graphnum)
    {
      return o->graph(graphnum);
    }
    void rescaleAxes(QCustomPlot* o, bool v = true) //rescale axis automatically if data does not fit
    {
      o->rescaleAxes(v);
    }
    /// \param interaction See QCP::Interaction
    void setInteraction(QCustomPlot* o, int interaction, bool enabled = true)
    {
      o->setInteraction((QCP::Interaction)interaction, enabled);
    }

    bool savePdf (QCustomPlot* o, const QString &fileName, bool noCosmeticPen=false, int width=0, int height=0, const QString &pdfCreator=QString(), const QString &pdfTitle=QString())
    {
      return o->savePdf (fileName, noCosmeticPen, width, height, pdfCreator, pdfTitle);
    }
    bool savePng (QCustomPlot* o, const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1)
    {
      return o->savePng (fileName, width, height, scale, quality);
    }
    bool saveJpg (QCustomPlot* o, const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1)
    {
      return o->saveJpg (fileName, width, height, scale, quality);
    }
    bool saveBmp (QCustomPlot* o, const QString &fileName, int width=0, int height=0, double scale=1.0)
    {
      return o->saveBmp (fileName, width, height, scale);
    }
    bool saveRastered (QCustomPlot* o, const QString &fileName, int width, int height, double scale, const char *format, int quality=-1)
    {
      return o->saveRastered (fileName, width, height, scale, format, quality);
    }

    QCPAxis* xAxis  (QCustomPlot* o) { return o->xAxis ; }
    QCPAxis* xAxis2 (QCustomPlot* o) { return o->xAxis2; }
    QCPAxis* yAxis  (QCustomPlot* o) { return o->yAxis ; }
    QCPAxis* yAxis2 (QCustomPlot* o) { return o->yAxis2; }
    QCPLegend* legend (QCustomPlot* o) { return o->legend; }
    QCPAxisRect* axisRect (QCustomPlot* o, int index=0) { return o->axisRect(index); }
    /// \}

    /// \name QCPAxis
    /// \todo Most of this function should be removed as they duplicates the
    ///       QProperty methods.
    /// \{
    int selectedParts(const QCPAxis* a)
    {
      return a->selectedParts();
    }
    void setLabel(QCPAxis* a, const QString text)
    {
      a->setLabel(text);
    }
    void setRange(QCPAxis* a, double position, double size)
    {
      a->setRange(position, size);
    }
    void setAutoTicks(QCPAxis* a, bool on)
    {
      a->setAutoSubTicks(on);
    }
    void setAutoTickLabels(QCPAxis* a, bool on)
    {
      a->setAutoTickLabels(on);
    }
    void setTickVector(QCPAxis* a, const QVector<double> &ticks)
    {
      a->setTickVector(ticks);
    }
    void setTickVectorLabels(QCPAxis* a, const QVector<QString> &labels)
    {
      a->setTickVectorLabels(labels);
    }
    void setTickLength(QCPAxis* a, int inside, int outside)
    {
      a->setTickLength(inside,outside);
    }
    void setSubTickLength(QCPAxis* a, int inside, int outside)
    {
      a->setSubTickLength(inside, outside);
    }
    double pixelToCoord(QCPAxis* a, double pixel)
    {
      return a->pixelToCoord(pixel);
    }
    /// \}

    /// \name QCPGraph
    /// \{
    QCPGraph* new_QCPGraph (QCPAxis* key, QCPAxis* value) { return new QCPGraph (key, value); }
    void delete_QCPGraph (QCPGraph* g) { delete g; }
    void setData (QCPGraph* g, const QVector<double> &keys, const QVector<double> &values)
    {
      g->setData(keys,values);
    }
    void addData (QCPGraph* g, const QVector<double> &keys, const QVector<double> &values)
    {
      g->addData(keys,values);
    }
    void addData (QCPGraph* g, const double &key, const double &value)
    {
      g->addData(key,value);
    }
    void clearData (QCPGraph* o) { o->clearData (); }
    /// \}

    /// \name QCPCurve
    /// \{
    QCPCurve* new_QCPCurve (QCPAxis* key, QCPAxis* value) { return new QCPCurve (key, value); }
    void delete_QCPCurve (QCPCurve* g) { delete g; }
    void setData (QCPCurve* c, const QVector<double> &keys, const QVector<double> &values)
    {
      c->setData(keys,values);
    }
    void addData (QCPCurve* c, const QVector<double> &ts, const QVector<double> &keys, const QVector<double> &values)
    {
      c->addData(ts, keys,values);
    }
    void clearData (QCPCurve* o) { o->clearData (); }
    /// \}

    /// \name QCPBars
    /// \{
    QCPBars* new_QCPBars (QCPAxis* key, QCPAxis* value) { return new QCPBars (key, value); }
    void delete_QCPBars (QCPBars* g) { delete g; }
    void setData (QCPBars* c, const QVector<double> &keys, const QVector<double> &values)
    {
      c->setData(keys,values);
    }
    void addData (QCPBars* c, const QVector<double> &keys, const QVector<double> &values)
    {
      c->addData(keys,values);
    }
    void clearData (QCPBars* o) { o->clearData (); }
    /// \}

    /// \name QCPAbstractPlottable
    /// \{
    void rescaleAxes(QCPAbstractPlottable* ap, bool v = true)
    {
      ap->rescaleAxes(v);
    }
    /// \}

    /// \name QCPLayerable
    /// \{
    /// \}

    /// \name QCPLayoutGrid
    /// \{
    void insertRow (QCPLayoutGrid* lg, int row) //insert row above graph
    {
      lg->insertRow(row);
    }
    void insertColumn (QCPLayoutGrid* lg, int column) //insert column above graph
    {
      lg->insertColumn(column);
    }
    void addElement (QCPLayoutGrid* lg, int row, int column, QCPLayoutElement *element) //add text to graph at row,column
    {
      lg->addElement(row,column,element);
    }
    /// \}

    /// \name QCPAxisRect
    /// \{
    void setRangeZoomAxes (QCPAxisRect* ar, QCPAxis* horizontal, QCPAxis* vertical)
    {
      ar->setRangeZoomAxes (horizontal, vertical);
    }
    /// \}

    /// \name QCPItemPosition
    /// \{
    void setType (QCPItemPosition* ip, int type) { return ip->setType((QCPItemPosition::PositionType)type); }
    bool setParentAnchor (QCPItemPosition* ip, QCPItemAnchor* a) { return ip->setParentAnchor (a); }
    void setCoords (QCPItemPosition* ip, double k, double v) { return ip->setCoords (k, v); }
    /// \}

    /// \name QCPAbstractItem
    /// \{
    QCPItemPosition* position (QCPAbstractItem* ai, QString p) { return ai->position(p); }
    QCPItemAnchor  * anchor   (QCPAbstractItem* ai, QString a) { return ai->anchor  (a); }
    /// \}

    /// \name QCPItemTracer
    /// \{
    QCPItemTracer* new_QCPItemTracer (QCustomPlot* parent) { return new QCPItemTracer (parent); }
    void delete_QCPItemTracer (QCPItemTracer* it) { delete it; }
    /// \}

    /// \name QCPItemRichText
    /// \{
    QCPItemRichText* new_QCPItemRichText (QCustomPlot* parent) { return new QCPItemRichText (parent); }
    void delete_QCPItemRichText (QCPItemRichText* it) { delete it; }
    /// \}

    /// \name QCPItemText
    /// \{
    QCPItemText* new_QCPItemText (QCustomPlot* parent) { return new QCPItemText (parent); }
    void delete_QCPItemText (QCPItemText* it) { delete it; }
    /// \}

    /// \name QCPItemEllipse
    /// \{
    QCPItemEllipse* new_QCPItemEllipse (QCustomPlot* parent) { return new QCPItemEllipse (parent); }
    void delete_QCPItemEllipse (QCPItemEllipse* it) { delete it; }
    /// \}
};
