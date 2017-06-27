#include "graph/graph.h"

#include "wx/brush.h"

Sizef GetTextExtents(const std::string &text) {
  wxBitmap temp;
  wxMemoryDC dc;
  dc.SelectObject(temp);

  ViewData view;
  const auto ret = view.Convert(dc.GetTextExtent(text.c_str()));

  dc.SelectObject(wxNullBitmap);
  return ret;
}

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
Sizef ViewData::Convert(const wxSize& s) const {
  return Sizef::FromWidthHeight(s.x, s.y);
}

DrawData::DrawData() : selected(false), height(0), width(0) {
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
  const Sizef size = view.Convert(dc->GetTextExtent(text.c_str()));
  vec2f offset =  rect.GetSize().CalculateCenterOffsetFor(size);
  dc->DrawText(text.c_str(), view.Convert(rect.GetPosition() + offset));
}

bool Node::HitTest(const vec2f& pos) const {
  return rect.ContainsInclusive(pos);
}

bool GraphData::IsSelected(std::shared_ptr<Object> specific_object) {
  for(std::weak_ptr<Object> weak : selected) {
    std::shared_ptr<Object> strong = weak.lock();
    if( strong.get() == specific_object.get()) {
      return true;
    }
  }

  return false;
}

Tool::Tool() {}
Tool::~Tool() {}

class SelectTool : public Tool {
 public:
  SelectTool() : down(false), start(0,0), mouse(0,0) {}
  ~SelectTool() {}

  void OnMouseMoved(GraphData* data, wxMouseEvent& event) override {
    ViewData view;
    mouse = view.Convert(event.GetPosition());
  }

  void OnMouse(GraphData* data, wxMouseEvent& event, bool d) override {
    down = d;

    if(d) {
      start = mouse;
    }

    if(d) {
      if(event.ShiftDown()) {
        // multi selection
      }
      else {
        // if no shift, clear previous selection
        data->selected.clear();
      }
      for(std::shared_ptr<Object> o : data->objects) {
        const bool hit = o->HitTest(mouse);
        if(hit) {
          data->selected.push_back(o);
        }
      }
    }
  }

  void Paint(wxPaintDC* dc, const ViewData& view, const DrawData& draw) override {
    const wxPoint m = view.Convert(mouse);
    dc->SetPen( wxPen(wxColor(0,0,0), 1, wxPENSTYLE_SOLID ) );
    dc->DrawLine(m.x, 0, m.x, draw.height);
    dc->DrawLine(0, m.y, draw.width, m.y);

    if(down) {
      const wxPoint s = view.Convert(start);
      dc->SetPen( wxPen(wxColor(0,0,255), 1, wxPENSTYLE_SOLID ) );
      dc->DrawLine(s, m);
    }
  }

  bool down;
  vec2f start;
  vec2f mouse;
};


Graph::Graph(wxWindow *parent) : wxPanel(parent) {
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
  data.objects.push_back(o);

  o.reset(new Node(Rectf::FromTopLeftWidthHeight(50.0f, 180.0f, 100.0f, 100.0f), "Goodbye world"));
  data.objects.push_back(o);

  o.reset(new Node(Rectf::FromTopLeftWidthHeight(150.0f, 80.0f, 100.0f, 100.0f), "Awesome world"));
  data.objects.push_back(o);

  data.tool.reset(new SelectTool());
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
  draw.width = width;
  draw.height = height;

  for(std::shared_ptr<Object> o : data.objects) {
    draw.selected = data.IsSelected(o);
    o->Draw(&dc, view, draw);
  }

  tool().Paint(&dc, view, draw);
}

void Graph::OnMouseMoved(wxMouseEvent& event) {
  tool().OnMouseMoved(&data, event);
  Invalidate();
}

void Graph::OnMouse(wxMouseEvent& event, bool d) {
  tool().OnMouseMoved(&data, event);
  tool().OnMouse(&data, event, d);
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
void Graph::OnMouseLeftWindow(wxMouseEvent& event) {
  tool().OnMouseMoved(&data, event);
  Invalidate();
}
void Graph::OnKeyPressed(wxKeyEvent& event) {}
void Graph::OnKeyReleased(wxKeyEvent& event) {}


Tool& Graph::tool() {
  return *data.tool.get();
}

const Tool& Graph::tool() const {
  return *data.tool.get();
}
