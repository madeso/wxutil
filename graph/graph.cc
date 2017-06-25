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

DrawData::DrawData() : mouse(0.0f, 0.0f), selected(false) {
}

Object::Object() { }
Object::~Object() { }

Node::Node(const Rectf& r, const std::string& t) : rect(r), text(t) { }
Node::~Node() { }

void Node::Draw(wxPaintDC* dc, const ViewData& view, const DrawData& draw) const {
  dc->SetPen( wxPen(wxColor(0,0,0), 1) );
  wxColor c (100, 255, 255);
  if(draw.selected) {
    c = wxColor(255, 255, 255);
  }
  dc->SetBrush(*wxTheBrushList->FindOrCreateBrush(c));
  dc->DrawRectangle(view.Convert(rect.GetPosition()), view.Convert(rect.GetSize()));

  dc->SetTextForeground(wxColor(0,0,0));
  if(HitTest(draw.mouse)) {
    dc->DrawText(text.c_str(), view.Convert(rect.GetPosition()));
  }
}

bool Node::HitTest(const vec2f& pos) const {
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

  std::shared_ptr<Object> o;

  o.reset(new Node(Rectf::FromTopLeftWidthHeight(10.0f, 10.0f, 100.0f, 100.0f), "Hello world"));
  objects.push_back(o);

  o.reset(new Node(Rectf::FromTopLeftWidthHeight(50.0f, 180.0f, 100.0f, 100.0f), "Goodbye world"));
  objects.push_back(o);

  o.reset(new Node(Rectf::FromTopLeftWidthHeight(150.0f, 80.0f, 100.0f, 100.0f), "Awesome world"));
  objects.push_back(o);
}

wxSize Graph::DoGetBestSize() const {
  return wxSize(800, 600);
}

void Graph::OnPaint(wxPaintEvent&) {
  wxPaintDC dc (this);
  wxCoord width = 0;
  wxCoord height = 0;

  GetClientSize( &width, &height );

  dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(wxColor(255, 255, 255)));
  dc.Clear();

  ViewData view;

  DrawData draw;
  draw.mouse = view.Convert(wxPoint(x,y));

  for(std::shared_ptr<Object> o : objects) {
    draw.selected = std::find(selected.begin(), selected.end(), o.get()) != selected.end();
    o->Draw(&dc, view, draw);
  }

  dc.SetPen( wxPen(wxColor(0,0,0), 1, wxPENSTYLE_SOLID ) );
  dc.DrawLine(x, 0, x, height);
  dc.DrawLine(0, y, width, y);
}

void Graph::OnMouseMoved(wxMouseEvent& event) {
  const auto p = event.GetPosition();
  x = p.x;
  y = p.y;
  Invalidate();
}

void Graph::OnMouse(wxMouseEvent& event, bool d) {
  const auto p = event.GetPosition();
  x = p.x;
  y = p.y;
  down = d;

  ViewData view;
  const vec2f mouse = view.Convert(wxPoint(x,y));

  if(d) {
    if(event.ShiftDown()) {
      // multi selection
    }
    else {
      // if no shift, clear previous selection
      selected.clear();
    }
    for(std::shared_ptr<Object> o : objects) {
      const bool hit = o->HitTest(mouse);
      if(hit) {
        selected.push_back(o.get());
      }
    }
  }

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
