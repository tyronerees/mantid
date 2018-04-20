from __future__ import (absolute_import, division, print_function)

from PyQt4 import QtGui

from Muon import dummy_widget
from Muon import dummy_label_widget
from Muon import dock_view


class DockWidget(QtGui.QWidget):

    """
    This is a special case of the widget class structure.
    Normally we would only store the presenter and would
    get the view via the presenter. However, the docks
    have no logic and therefore do not have a presenter.
    So this class simply wraps the dock (view) and
    populates it
    """

    def __init__(self, parent=None):
        super(DockWidget, self).__init__(parent)
        self.widget = QtGui.QWidget()

        self.dock_view = dock_view.DockView(self)

        self.btn = dummy_widget.DummyWidget("moo", self)
        self.dock_view.addDock(self.btn.getWidget(), "first")
        self.btn.setButtonConnection(self.handleButton)

        self.label = dummy_label_widget.DummyLabelWidget("boo", self)
        self.dock_view.addDock(self.label.getWidget(), "second")

        self.btn2 = dummy_widget.DummyWidget("waaa", self)
        self.dock_view.addDock(self.btn2.getWidget(), "third")
        self.btn2.setButtonConnection(self.handleButton)

        self.dock_view.makeTabs()
        self.dock_view.keepDocksOpen()

        QHbox = QtGui.QHBoxLayout()
        QHbox.addWidget(self.dock_view)

        self.widget.setLayout(QHbox)

    def handleButton(self, message):
        self.label.updateLabel(message)

    def getWidget(self):
        return self.widget

    def closeEvent(self, event):
        self.dock_view.closeEvent(event)
