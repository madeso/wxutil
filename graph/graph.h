

#ifndef WXUTIL_GRAPH_GRAPH_H
#define WXUTIL_GRAPH_GRAPH_H

#include "wx.h"

class Graph : public wxPanel
{
 public:
  Graph(wxWindow* parent);
  wxSize DoGetBestSize() const;
  void OnPaint(wxPaintEvent& event);
};


#endif //WXUTIL_GRAPH_GRAPH_H
