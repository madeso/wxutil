//
// Created by gustav on 2017-06-24.
//

#include "graph/graph.h"

#include "wx/brush.h"

Graph::Graph(wxWindow *parent) : wxPanel(parent) {
  Bind(wxEVT_PAINT, &Graph::OnPaint, this);
}

wxSize Graph::DoGetBestSize() const {
  return wxSize(800, 600);
}

void Graph::OnPaint(wxPaintEvent&) {
  wxPaintDC dc (this);
  wxCoord width = 0;
  wxCoord  height = 0;

  GetClientSize( &width, &height );

  dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(wxColor(255, 255, 255)));
  dc.Clear();

  dc.SetPen( wxPen(wxColor(0,0,0), 1) );
  dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(wxColor(100, 255, 255)));
  dc.DrawRectangle(wxPoint(0,0), wxSize(100, 100));

  dc.SetTextForeground(wxColor(0,0,0));
  dc.DrawText("Hello world", 0, 0);
}

