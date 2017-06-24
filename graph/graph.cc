//
// Created by gustav on 2017-06-24.
//

#include "graph/graph.h"

#include "wx/brush.h"

const int ViewData::HorizontalConvert(float f) const {
  return static_cast<int>(f);
}
const int ViewData::VerticalConvert(float f) const {
  return static_cast<int>(f);
}
wxPoint ViewData::Convert(const vec2f& p) const {
  return wxPoint(HorizontalConvert(p.x), VerticalConvert(p.y));
}
wxSize ViewData::Convert(const Sizef& s) const {
  return wxSize(HorizontalConvert(s.GetWidth()), VerticalConvert(s.GetHeight()));
}

Object::Object() : rect(Rectf::FromTopLeftWidthHeight(10.0f, 10.0f, 100.0f, 100.0f)) { }
Object::~Object() { }

void Object::Draw(wxPaintDC* dc, const ViewData& view) {
  dc->SetPen( wxPen(wxColor(0,0,0), 1) );
  dc->SetBrush(*wxTheBrushList->FindOrCreateBrush(wxColor(100, 255, 255)));
  dc->DrawRectangle(view.Convert(rect.GetPosition()), view.Convert(rect.GetSize()));

  dc->SetTextForeground(wxColor(0,0,0));
  dc->DrawText("Hello world", view.Convert(rect.GetPosition()));
}


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

  ViewData view;
  Object object;
  object.Draw(&dc, view);
}

