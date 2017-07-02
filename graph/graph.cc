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

Object::Object() : removeThis(false) { }
Object::~Object() { }

Node::Node(const Rectf& r, const std::string& t) : rect(r), text(t), movement(0,0) { }
Node::~Node() { }

void Node::Draw(wxPaintDC* dc, const ViewData& view, const DrawData& draw) const {
  const Rectf currentRect = GetModifiedRect();

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

Rectf Node::GetModifiedRect() const {
  const Rectf currentRect = rect.OffsetCopy(movement);
  return currentRect;
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

class OptionalPoint {
 public:
  bool hasPoint;
  vec2f point;

  static OptionalPoint NoPoint() {
    return OptionalPoint(false, vec2f(0,0));
  }

  static OptionalPoint NodeCollision(const Node& fromNode, const OptionalPoint& oto) {
    if(oto.hasPoint == false) return NoPoint();

    const vec2f to = oto.point;

    const Rectf r = fromNode.GetModifiedRect();
    const vec2f fpp = r.GetAbsoluteCenterPos();

    const line2f::Collision fc = r.GetPointOnEdge(line2f::FromTo(fpp, to));

    if(fc.collision ) {
      return fc.point;
    }
    else {
      return NoPoint();
    }
  }

  OptionalPoint(const vec2f& p) : hasPoint(true), point(p) {};
  OptionalPoint(const Node& node) : hasPoint(true), point(node.GetModifiedRect().GetAbsoluteCenterPos()) {}
 private:
  OptionalPoint(bool hp, const vec2f& p) : hasPoint(hp), point(p) {};
};

void DrawEdge(wxPaintDC* dc, const ViewData& view, const OptionalPoint& fp, const OptionalPoint& tp) {
  if(fp.hasPoint && tp.hasPoint) {
    dc->DrawLine(view.Convert(fp.point), view.Convert(tp.point));
  }
}

class Link : public Object {
 public:
  Link(std::shared_ptr<Node> f, std::shared_ptr<Node> t) : from(f), to(t) {}
  ~Link() {}

  std::weak_ptr<Node> from;
  std::weak_ptr<Node> to;

  // todo: add a better edge drawing, possible draw all edges before the nodes
  void Draw(wxPaintDC* dc, const ViewData& view, const DrawData& draw) const override {
    std::shared_ptr<Node> f = from.lock();
    std::shared_ptr<Node> t = to.lock();
    if( f && t ) {
      // todo: remove self if the nodes has been removed
      dc->SetPen( wxPen(wxColor(0,255,0), 1) );
      DrawEdge(dc, view, OptionalPoint::NodeCollision(*f, *t), OptionalPoint::NodeCollision(*t, *f));
    }
  }

  bool HitTest(const vec2f& pos) const override {
    // todo: implement hit test
    return false;
  }

  void MoveSet(const vec2f& m) override {}
  void MoveApply(const vec2f& m) override {}
  void MoveCancel() override {}
};

GraphData::GraphData() {
}

void GraphData::DeleteSelected() {
  for(std::weak_ptr<Object> weak : selected) {
    std::shared_ptr<Object> strong = weak.lock();
    if( strong.get() ) {
      strong->removeThis = true;
    }
  }
}
void GraphData::RemoveTaggedItems() {
  objects.erase(std::remove_if(objects.begin(), objects.end(), [](std::shared_ptr<Object> o){ return o->removeThis; }), objects.end());
}

void GraphData::RemoveDeadSelections() {
  selected.erase(std::remove_if(selected.begin(), selected.end(), [](std::weak_ptr<Object> w){ std::shared_ptr<Object> o = w.lock(); return o.get() == nullptr; }), selected.end());
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

bool GraphData::RemoveDeadTools() {
  auto remove_start = std::remove_if(tools.begin(), tools.end(), [](std::shared_ptr<Tool> t){ return t->ShouldRemoveThis(); });
  const bool will_remove_items = remove_start != tools.end();
  tools.erase(remove_start, tools.end());
  return will_remove_items;
}

void GraphData::Step(wxMouseEvent& event) {
  if(RemoveDeadTools()) {
    tool().Refresh(this, event);
  }
  Step();
}

void GraphData::Step() {
  RemoveTaggedItems();
  RemoveDeadSelections();
}

void GraphData::Add(std::shared_ptr<Tool> tool) {
  tools.push_back(tool);
}

Tool& GraphData::tool() {
  return *tools.rbegin()->get();
}

std::vector<std::shared_ptr<Object>> GraphData::HitTestAll(const vec2f& mousePosition) {
  std::vector<std::shared_ptr<Object>> ret;
  for(std::shared_ptr<Object> o : objects) {
    const bool hit = o->HitTest(mousePosition);
    if(hit) {
      ret.push_back(o);
    }
  }

  return ret;
}

std::shared_ptr<Node> GraphData::HitTestTopMost(const vec2f& mousePosition) {
  // todo: currently bottom most, fix this
  for(std::shared_ptr<Object> o : objects) {
    const bool hit = o->HitTest(mousePosition);
    if(hit) {
      std::shared_ptr<Node> ret = std::dynamic_pointer_cast<Node>(o);
      if(ret.get()) {
        return ret;
      }
    }
  }

  std::shared_ptr<Node> ret;
  return ret;
}

Tool::Tool() : mousePosition(0,0), mouseButtonDown(false), removeThis(false) {}
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

void PaintCustomCursor(wxPaintDC *dc, const ViewData &view,
                       const DrawData &draw, const vec2f& mp) {
  const wxPoint m = view.Convert(mp);
  dc->SetPen( wxPen(wxColor(0,0,0), 1, wxPENSTYLE_SOLID ) );
  dc->DrawLine(m.x, 0, m.x, draw.height);
  dc->DrawLine(0, m.y, draw.width, m.y);
}

void Tool::OnPaint(wxPaintDC *dc, const ViewData &view, const DrawData &draw) {
  PaintCustomCursor(dc, view, draw, mousePosition);
  Paint(dc, view, draw);
}

bool Tool::ShouldRemoveThis() const {
  return removeThis;
}

class MoveTool : public Tool {
 public:
  MoveTool(const vec2f& s, const vec2f& m) : Tool(m), start(s) {
    mouseButtonDown = true;
  }
  ~MoveTool() {}

  void OnCancel(GraphData* data) override {
    for(std::weak_ptr<Object> wo : data->selected) {
      std::shared_ptr<Object> o = wo.lock();
      if(o.get()) {
        o->MoveCancel();
      }
    }
    removeThis = true;
  }

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
      removeThis = true;
      for(std::weak_ptr<Object> wo : data->selected) {
        std::shared_ptr<Object> o = wo.lock();
        if(o.get()) {
          o->MoveApply(move);
        }
      }
    }
  }

  void Paint(wxPaintDC *dc, const ViewData &view, const DrawData &draw) override {
    if(mouseButtonDown) {
      const wxPoint m = view.Convert(mousePosition);
      const wxPoint s = view.Convert(start);
      dc->SetPen( wxPen(wxColor(0,0,255), 1, wxPENSTYLE_SOLID ) );
      dc->DrawLine(s, m);
    }
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

  void OnCancel(GraphData* data) override {}

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
      auto hit = data->HitTestAll(mousePosition);
      for(std::shared_ptr<Object> o : hit) {
        data->selected.push_back(o);
      }
    }
  }

  void Paint(wxPaintDC* dc, const ViewData& view, const DrawData& draw) override {
  }

  vec2f start;
};

class LinkTool : public Tool {
 public:
  LinkTool() {}
  ~LinkTool() {}

  void MouseMoved(GraphData* data, wxMouseEvent& event) override {
    hovering_node = data->HitTestTopMost(mousePosition);
  }

  void OnCancel(GraphData* data) override {
    removeThis = true;
  }

  void MouseButton(GraphData *data, wxMouseEvent &event) override {
    if(!mouseButtonDown) {
      std::shared_ptr<Node> hit_node = data->HitTestTopMost(mousePosition);

      if(first_node) {
        // linking from first_node
        // todo: create link from first_node to hit_node, possible creating hit_node if null
        if(hit_node) {
          std::shared_ptr<Object> link(new Link(first_node, hit_node));
          data->objects.push_back(link);
        }
        removeThis = true;
      }
      else {
        if(hit_node == nullptr) {
          removeThis = true;
          return;
        }
        first_node = hit_node;
      }
    }
  }

  void PaintHotspot(wxPaintDC *dc, const ViewData &view,
                         const DrawData &draw, const vec2f& mp) {
    const int CROSS_SIZE = 20;
    const wxPoint m = view.Convert(mp);
    float distance = vec2f::FromTo(mp, mousePosition).GetLength();
    dc->SetPen( wxPen(wxColor(0,0,255), 1, wxPENSTYLE_LONG_DASH ) );
    dc->SetBrush(wxNullBrush);
    dc->DrawCircle(m, view.HorizontalConvert(distance));
    dc->DrawLine(m.x-CROSS_SIZE, m.y-CROSS_SIZE, m.x+CROSS_SIZE, m.y+CROSS_SIZE);
    dc->DrawLine(m.x-CROSS_SIZE, m.y+CROSS_SIZE, m.x+CROSS_SIZE, m.y-CROSS_SIZE);
  }

  void Paint(wxPaintDC* dc, const ViewData& view, const DrawData& draw) override {
    if(hovering_node && hovering_node != first_node) {
      const vec2f p = hovering_node->rect.GetAbsoluteCenterPos();
      PaintHotspot(dc, view, draw, p);
    }

    if(first_node.get()) {
      dc->SetPen( wxPen(wxColor(255,0,0), 1, wxPENSTYLE_LONG_DASH ) );

      const OptionalPoint to = hovering_node && hovering_node != first_node ? OptionalPoint::NodeCollision(*hovering_node, *first_node) : mousePosition;

      DrawEdge(dc, view, OptionalPoint::NodeCollision(*first_node, to), to);
    }
  }

  std::shared_ptr<Node> hovering_node;
  std::shared_ptr<Node> first_node;
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

  tool().OnPaint(&dc, view, draw);
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

void Graph::OnMouseUp(wxMouseEvent& event) { OnMouseButton(event, false);}
void Graph::OnMouseDown(wxMouseEvent& event) { OnMouseButton(event, true);}
void Graph::OnMouseWheelMoved(wxMouseEvent& event) {}
void Graph::OnMouseRightClick(wxMouseEvent& event) {}
void Graph::OnMouseLeftWindow(wxMouseEvent& event) {
  tool().OnMouseMoved(&data, event);
  Invalidate(event);
}

enum {
  ABORT_KEY = wxKeyCode::WXK_ESCAPE
};

void Graph::OnKeyPressed(wxKeyEvent& event) {
  if(event.m_keyCode == ABORT_KEY) {
    return;
  }
  event.Skip();
}

void Graph::OnKeyReleased(wxKeyEvent& event) {
  if(event.m_keyCode == ABORT_KEY) {
    tool().OnCancel(&data);
    Invalidate();
    return;
  }
  event.Skip();
}

void Graph::DeleteSelected() {
  data.DeleteSelected();
  Invalidate();
}

void Graph::LinkNodes() {
  std::shared_ptr<Tool> t(new LinkTool());
  data.Add(t);
}

void Graph::Invalidate(wxMouseEvent& event) {
  data.Step(event);
  Redraw();
}
void Graph::Invalidate() {
  data.Step();
  Redraw();
}
void Graph::Redraw() {
  Refresh();
  Update();
}

Tool& Graph::tool() {
  return data.tool();
}
