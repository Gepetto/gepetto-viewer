#ifndef QCP_ITEM_RICH_TEXT
#define QCP_ITEM_RICH_TEXT
#include <qcustomplot.h>

#include <QTextDocument>

class QCP_LIB_DECL QCPItemRichText : public QCPItemText
{ Q_OBJECT
  Q_PROPERTY(QString text READ text WRITE setText)
public:
  QCPItemRichText(QCustomPlot *parentPlot)
    : QCPItemText (parentPlot) {}
  virtual ~QCPItemRichText() {}

  void setText(const QString &text)
  {
    QCPItemText::setText (text);
    doc.setHtml(text);
  }
  
protected:
  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter)
  {
    QPointF pos(position->pixelPoint());
    QTransform transform = painter->transform();
    transform.translate(pos.x(), pos.y());
    if (!qFuzzyIsNull(mRotation))
      transform.rotate(mRotation);
    painter->setFont(mainFont());
    QRect textRect = painter->fontMetrics().boundingRect(0, 0, 0, 0, Qt::TextDontClip|mTextAlignment, mText);
    QRect textBoxRect = textRect.adjusted(-mPadding.left(), -mPadding.top(), mPadding.right(), mPadding.bottom());
    QPointF textPos = getTextDrawPoint(QPointF(0, 0), textBoxRect, mPositionAlignment); // 0, 0 because the transform does the translation
    textRect.moveTopLeft(textPos.toPoint()+QPoint(mPadding.left(), mPadding.top()));
    textBoxRect.moveTopLeft(textPos.toPoint());
    qreal clipPadF = mainPen().widthF();
    int clipPad = (int)clipPadF;
    QRect boundingRect = textBoxRect.adjusted(-clipPad, -clipPad, clipPad, clipPad);
    if (transform.mapRect(boundingRect).intersects(painter->transform().mapRect(clipRect())))
    {
      painter->setTransform(transform);
      if ((mainBrush().style() != Qt::NoBrush && mainBrush().color().alpha() != 0) ||
          (mainPen().style() != Qt::NoPen && mainPen().color().alpha() != 0))
      {
        painter->setPen(mainPen());
        painter->setBrush(mainBrush());
        painter->drawRect(textBoxRect);
      }
      painter->setBrush(Qt::NoBrush);
      painter->setPen(QPen(mainColor()));
      doc.setDefaultFont(mainFont());
      doc.drawContents(painter);
    }
  }

  QTextDocument doc;
};
#endif // QCP_ITEM_RICH_TEXT
