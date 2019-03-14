#ifndef WRAPPER_QCP_HH
# define WRAPPER_QCP_HH
# include <qcustomplot.h>

namespace wrapper {
class PyQCP : public QObject
{ Q_OBJECT
  public:
    Q_ENUMS(MarginSide  AntialiasedElement  PlottingHint  Interaction )
    Q_FLAGS(MarginSides AntialiasedElements PlottingHints Interactions)

enum MarginSide { msLeft     = QCP::msLeft  
                  ,msRight   = QCP::msRight 
                  ,msTop     = QCP::msTop   
                  ,msBottom  = QCP::msBottom
                  ,msAll     = QCP::msAll   
                  ,msNone    = QCP::msNone  
                };
Q_DECLARE_FLAGS(MarginSides, MarginSide)

enum AntialiasedElement { aeAxes           = QCP::aeAxes       
                          ,aeGrid          = QCP::aeGrid       
                          ,aeSubGrid       = QCP::aeSubGrid    
                          ,aeLegend        = QCP::aeLegend     
                          ,aeLegendItems   = QCP::aeLegendItems
                          ,aePlottables    = QCP::aePlottables 
                          ,aeItems         = QCP::aeItems      
                          ,aeScatters      = QCP::aeScatters   
                          ,aeErrorBars     = QCP::aeErrorBars  
                          ,aeFills         = QCP::aeFills      
                          ,aeZeroLine      = QCP::aeZeroLine   
                          ,aeAll           = QCP::aeAll        
                          ,aeNone          = QCP::aeNone       
                        };
Q_DECLARE_FLAGS(AntialiasedElements, AntialiasedElement)

enum PlottingHint { phNone            = QCP::phNone         
                    ,phFastPolylines  = QCP::phFastPolylines
                    ,phForceRepaint   = QCP::phForceRepaint 
                    ,phCacheLabels    = QCP::phCacheLabels  
                  };
Q_DECLARE_FLAGS(PlottingHints, PlottingHint)

enum Interaction { iRangeDrag         = QCP::iRangeDrag       
                   ,iRangeZoom        = QCP::iRangeZoom       
                   ,iMultiSelect      = QCP::iMultiSelect     
                   ,iSelectPlottables = QCP::iSelectPlottables
                   ,iSelectAxes       = QCP::iSelectAxes      
                   ,iSelectLegend     = QCP::iSelectLegend    
                   ,iSelectItems      = QCP::iSelectItems     
                   ,iSelectOther      = QCP::iSelectOther     
                 };
Q_DECLARE_FLAGS(Interactions, Interaction)

};
}

#endif // WRAPPER_QCP_HH
