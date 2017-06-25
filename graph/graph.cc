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
vec2f ViewData::Convert(const wxPoint& p) const {
  return vec2f(p.x, p.y);
}
wxSize ViewData::Convert(const Sizef& s) const {
  return wxSize(HorizontalConvert(s.GetWidth()), VerticalConvert(s.GetHeight()));
}

DrawData::DrawData() : mouse(0.0f, 0.0f) {
}

Object::Object() : rect(Rectf::FromTopLeftWidthHeight(10.0f, 10.0f, 100.0f, 100.0f)) { }
Object::~Object() { }

void Object::Draw(wxPaintDC* dc, const ViewData& view, const DrawData& draw) {
  dc->SetPen( wxPen(wxColor(0,0,0), 1) );
  dc->SetBrush(*wxTheBrushList->FindOrCreateBrush(wxColor(100, 255, 255)));
  dc->DrawRectangle(view.Convert(rect.GetPosition()), view.Convert(rect.GetSize()));

  dc->SetTextForeground(wxColor(0,0,0));
  if(HitTest(draw.mouse)) {
    dc->DrawText("Hello world", view.Convert(rect.GetPosition()));
  }
}
bool Object::HitTest(const vec2f& pos) {
  return rect.ContainsInclusive(pos);
}


Graph::Graph(wxWindow *parent) : wxPanel(parent), x(0),y(0), down(false)  {
  Bind(wxEVT_PAINT, &Graph::OnPaint, this);

  Bind(wxEVT_MOTION, &Graph::OnMouseMoved, this);
  Bind(wxEVT_LEFT_DOWN, &Graph::OnMouseDown, this);
  Bind(wxEVT_LEFT_UP, &Graph::OnMouseUp, this);
  Bind(wxEVT_RIGHT_DOWN, &Graph::OnMouseRightClick, this);
  Bind(wxEVT_LEAVE_WINDOW, &Graph::OnMouseLeftWindow, this);
  Bind(wxEVT_KEY_DOWN, &Graph::OnKeyPressed, this);
  Bind(wxEVT_KEY_UP, &Graph::OnKeyReleased, this);
  Bind(wxEVT_MOUSEWHEEL, &Graph::OnMouseWheelMoved, this);
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

  DrawData draw;
  draw.mouse = view.Convert(wxPoint(x,y));
  Object object;
  object.Draw(&dc, view, draw);

  dc.SetPen( wxPen(wxColor(0,0,0), 3, down? wxPENSTYLE_LONG_DASH:wxPENSTYLE_SOLID ) );
  dc.DrawLine(0, 0, x, y);
}

void Graph::OnMouseMoved(wxMouseEvent& event) {
  const auto p = event.GetPosition();
  x = p.x;
  y = p.y;
  Invalidate();
}

void Graph::OnMouse(wxMouseEvent& event, bool d) {
  down = d;
  Invalidate();
}

void Graph::Invalidate() {
  Refresh();
  Update();
}

void Graph::OnMouseUp(wxMouseEvent& event) {OnMouse(event, false);}
void Graph::OnMouseDown(wxMouseEvent& event) {OnMouse(event, true);}
void Graph::OnMouseWheelMoved(wxMouseEvent& event) {}
void Graph::OnMouseRightClick(wxMouseEvent& event) {}
void Graph::OnMouseLeftWindow(wxMouseEvent& event) {}
void Graph::OnKeyPressed(wxKeyEvent& event) {}
void Graph::OnKeyReleased(wxKeyEvent& event) {}
