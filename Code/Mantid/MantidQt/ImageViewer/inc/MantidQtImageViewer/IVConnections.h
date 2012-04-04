#ifndef IV_CONNECTIONS_H
#define IV_CONNECTIONS_H

#include <QtCore/QtCore>
#include <QtGui/QWidget>

#include "ui_ImageView.h"
#include "MantidQtImageViewer/TrackingPicker.h"
#include "MantidQtImageViewer/ImageDisplay.h"
#include "MantidQtImageViewer/GraphDisplay.h"
#include "MantidQtImageViewer/DllOptionIV.h"


/**
    @class IVConnections 
  
       This class provides the connections between the ImageView GUI components
    made using QtDesigner and the classes that do the actual work for the
    ImageView.  It basically provides SLOTS that are called by the GUI 
   components' SIGNALS and in turn call methods on the ImageView 
   implementation objects.
 
    @author Dennis Mikkelson 
    @date   2012-04-03 
     
    Copyright © 2012 ORNL, STFC Rutherford Appleton Laboratories
  
    This file is part of Mantid.

    Mantid is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Mantid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    Code Documentation is available at 
                 <http://doxygen.mantidproject.org>
 */

namespace MantidQt
{
namespace ImageView
{


class EXPORT_OPT_MANTIDQT_IMAGEVIEWER IVConnections: public QWidget
{
  Q_OBJECT

public:

   IVConnections( Ui_MainWindow* ui, 
                  ImageDisplay*  image_display,
                  GraphDisplay*  h_graph_display,
                  GraphDisplay*  v_graph_display );
   ~IVConnections();

public slots:
  void somethingChanged();
  void toggle_Hscroll();
  void toggle_Vscroll();
  void v_scroll_bar_moved();
  void h_scroll_bar_moved();
  void imageSplitterMoved();
  void imagePickerMoved();
  void h_graphPickerMoved();
  void v_graphPickerMoved();

private:

  Ui_MainWindow*   iv_ui;
  ImageDisplay*    image_display;
  GraphDisplay*    h_graph_display;
  GraphDisplay*    v_graph_display;
  TrackingPicker*  image_picker;
  TrackingPicker*  h_graph_picker;
  TrackingPicker*  v_graph_picker;

};

} // namespace MantidQt 
} // namespace ImageView 


#endif  // IV_CONNECTIONS_H
