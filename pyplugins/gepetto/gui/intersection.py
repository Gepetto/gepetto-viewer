from __future__ import print_function

from gepetto.corbaserver import Client
from PythonQt import Qt, QtGui

class Plugin(QtGui.QDockWidget):
    """
    Display coordinates and normals of the intersection between user click and nodes.
    """
    def __init__ (self, mainWindow):
        super(Plugin, self).__init__ ("Coordinates viewer plugin", mainWindow)
        self.setObjectName("Coordinates viewer plugin")
        self.client = Client()

        self.le_name   = QtGui.QLineEdit()
        self.le_name   .setReadOnly(True)
        self.transform = QtGui.QLineEdit()
        self.transform .setReadOnly(True)

        self.local  = QtGui.QCheckBox()
        self.local.setChecked (True)
        self.point  = QtGui.QLineEdit()
        self.point  .setReadOnly(True)
        self.normal = QtGui.QLineEdit()
        self.normal .setReadOnly(True)

        layout = QtGui.QFormLayout()
        layout.addRow ('Name'       , self.le_name)
        layout.addRow ('Transform'  , self.transform)
        layout.addRow ('Local frame', self.local)
        layout.addRow ('Point'      , self.point)
        layout.addRow ('Normal'     , self.normal)


        # Initialize the widget
        widget = QtGui.QWidget(self)
        widget.setLayout (layout)
        # This avoids having a widget bigger than what it needs. It avoids having
        # a big dock widget and a small osg widget when creating the main osg widget.
        p = Qt.QSizePolicy.Maximum
        widget.setSizePolicy(Qt.QSizePolicy(Qt.QSizePolicy.Expanding,Qt.QSizePolicy.Maximum))
        self.setWidget (widget)

        mainWindow.bodyTree().connect('bodySelected(SelectionEvent*)', self.selected)

    def selected(self, event):
        if event.hasIntersection():
            local = self.local.isChecked()
            self.le_name    .setText(event.nodeName())
            pt = event.point (local)
            nl = event.normal(local)
            self.point      .setText('({},{},{})'.format(pt.x(),pt.y(),pt.z()))
            self.normal     .setText('({},{},{})'.format(nl.x(),nl.y(),nl.z()))
            tf = self.client.gui.getNodeGlobalTransform(str(event.nodeName()))
            self.transform  .setText('(' + ",".join([str(v) for v in tf]) + ')')
        else:
            self.le_name    .setText("")
            self.transform  .setText("")
            self.point      .setText("")
            self.normal     .setText("")
        event.done()
