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

Node::Node(const Rectf& r, const std::string& t) : rect(r), text(t), movement(0,0) { }
Node::~Node() { }

void Node::Draw(wxPaintDC* dc, const ViewData& view, const DrawData& draw) const {
  const Rectf currentRect = rect.OffsetCopy(movement);

  dc->SetPen( wxPen(wxColor(0,0,0), 1) );
  wxColor c (100, 255, 255);
  if(draw.selected) {
    c = wxColor(255, 255, 255);
  }
  dc->SetBrush(*wxTheBrushList->FindOrCreateBrush(c));
  dc->DrawRectangle(view.Convert(currentRect.GetPosition()), view.Convert(currentRect.GetSize()));

  dc->SetTextForeground(wxColor(0,0,0));
  const Sizef size = view.Convert(dc->GetTextExtent(text.c_str()));
  vec2f offset =  currentRect.GetSize().CalculateCenterOffsetFor(size);
  dc->DrawText(text.c_str(), view.Convert(currentRect.GetPosition() + offset));
}

bool Node::HitTest(const vec2f& pos) const {
  return rect.ContainsInclusive(pos);
}

void Node::MoveSet(const vec2f& m) {
  movement = m;
}

void Node::MoveApply(const vec2f& m) {
  rect = rect.OffsetCopy(m);
  movement = vec2f(0,0);
}

void Node::MoveCancel() {
  movement = vec2f(0,0);
}

GraphData::GraphData() : pop(false) {
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

void GraphData::Step(GraphData* data, wxMouseEvent& event) {
  if(pop) {
    if(tools.size() > 1) {
      tools.pop_back();
      tool().Refresh(data, event);
    }
    pop = false;
  }
}

void GraphData::Add(std::shared_ptr<Tool> tool) {
  tools.push_back(tool);
}

Tool& GraphData::tool() {
  return *tools.rbegin()->get();
}

Tool::Tool() : mousePosition(0,0), mouseButtonDown(false) {}
Tool::Tool(const vec2f& m) : mousePosition(m), mouseButtonDown(false) {}
Tool::~Tool() {}
void Tool::OnMouseMoved(GraphData* data, wxMouseEvent& event) {
  ViewData view;
  mousePosition = view.Convert(event.GetPosition());
  MouseMoved(data, event);
}
void Tool::Refresh(GraphData* data, wxMouseEvent& event) {
  ViewData view;
  mousePosition = view.Convert(event.GetPosition());
}
void Tool::OnMouseButton(GraphData *data, wxMouseEvent &event, bool d) {
  mouseButtonDown = d;
  MouseButton(data, event);
}

class MoveTool : public Tool {
 public:
  MoveTool(const vec2f& s, const vec2f& m) : Tool(m), start(s) {}
  ~MoveTool() {}

  void MouseMoved(GraphData* data, wxMouseEvent& event) override {
    const vec2f move = vec2f::FromTo(start, mousePosition);

    for(std::weak_ptr<Object> wo : data->selected) {
      std::shared_ptr<Object> o = wo.lock();
      if(o.get()) {
        o->MoveSet(move);
      }
    }
  }

  void MouseButton(GraphData *data, wxMouseEvent &event) override {
    // start + move = mousePosition;
    const vec2f move = vec2f::FromTo(start, mousePosition);

    if(!mouseButtonDown) {
      for(std::weak_ptr<Object> wo : data->selected) {
        std::shared_ptr<Object> o = wo.lock();
        if(o.get()) {
          o->MoveApply(move);
          data->pop = true;
        }
      }
    }
  }

  void Paint(wxPaintDC* dc, const ViewData& view, const DrawData& draw) override {
    const wxPoint m = view.Convert(mousePosition);
    dc->SetPen( wxPen(wxColor(0,0,0), 1, wxPENSTYLE_SOLID ) );
    dc->DrawLine(m.x, 0, m.x, draw.height);
    dc->DrawLine(0, m.y, draw.width, m.y);

    const wxPoint s = view.Convert(start);
    dc->SetPen( wxPen(wxColor(0,0,255), 1, wxPENSTYLE_SOLID ) );
    dc->DrawLine(s, m);
  }

  vec2f start;
};

class SelectTool : public Tool {
 public:
  SelectTool() : start(0,0) {}
  ~SelectTool() {}

  void MouseMoved(GraphData* data, wxMouseEvent& event) override {
    if(mouseButtonDown) {
      const float l = vec2f::FromTo(mousePosition, start).GetLengthSquared();
      if(l > 5) {
        mouseButtonDown = false;
        std::shared_ptr<Tool> t(new MoveTool(start, mousePosition));
        data->Add(t);
      }
    }
  }

  void MouseButton(GraphData *data, wxMouseEvent &event) override {
    if(mouseButtonDown) {
      start = mousePosition;
    }

    if(!mouseButtonDown) {
      if(event.ShiftDown()) {
        // multi selection
      }
      else {
        // if no shift, clear previous selection
        data->selected.clear();
      }
      for(std::shared_ptr<Object> o : data->objects) {
        const bool hit = o->HitTest(mousePosition);
        if(hit) {
          data->selected.push_back(o);
        }
      }
    }
  }

  void Paint(wxPaintDC* dc, const ViewData& view, const DrawData& draw) override {
    const wxPoint m = view.Convert(mousePosition);
    dc->SetPen( wxPen(wxColor(0,0,0), 1, wxPENSTYLE_SOLID ) );
    dc->DrawLine(m.x, 0, m.x, draw.height);
    dc->DrawLine(0, m.y, draw.width, m.y);

    if(mouseButtonDown) {
      const wxPoint s = view.Convert(start);
      dc->SetPen( wxPen(wxColor(0,0,255), 1, wxPENSTYLE_SOLID ) );
      dc->DrawLine(s, m);
    }
  }

  vec2f start;
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

  std::shared_ptr<Tool> t(new SelectTool());
  data.Add(t);
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
  Invalidate(event);
}

void Graph::OnMouseButton(wxMouseEvent &event, bool d) {
  tool().OnMouseMoved(&data, event);
  tool().OnMouseButton(&data, event, d);
  Invalidate(event);
}

void Graph::Invalidate(wxMouseEvent& event) {
  data.Step(&data, event);
  Refresh();
  Update();
}

void Graph::OnMouseUp(wxMouseEvent& event) { OnMouseButton(event, false);}
void Graph::OnMouseDown(wxMouseEvent& event) { OnMouseButton(event, true);}
void Graph::OnMouseWheelMoved(wxMouseEvent& event) {}
void Graph::OnMouseRightClick(wxMouseEvent& event) {}
void Graph::OnMouseLeftWindow(wxMouseEvent& event) {
  tool().OnMouseMoved(&data, event);
  Invalidate(event);
}
void Graph::OnKeyPressed(wxKeyEvent& event) {}
void Graph::OnKeyReleased(wxKeyEvent& event) {}


Tool& Graph::tool() {
  return data.tool();
}
