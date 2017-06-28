

#ifndef WXUTIL_GRAPH_GRAPH_H
#define WXUTIL_GRAPH_GRAPH_H

#include <memory>
#include <string>

#include "wx.h"

#include "core/rect.h"

class ViewData {
 public:
  const int HorizontalConvert(float f) const;
  const int VerticalConvert(float f) const;

  wxPoint Convert(const vec2f& p) const;
  vec2f Convert(const wxPoint& p) const;

  wxSize Convert(const Sizef& s) const;
  Sizef Convert(const wxSize& s) const;
};

class DrawData {
 public:
  DrawData();

  // is this node selected
  bool selected;

  // client area
  int height;
  int width;
};

class Object {
 public:
  Object();
  virtual ~Object();

  virtual void Draw(wxPaintDC* dc, const ViewData& view, const DrawData& draw) const = 0;
  virtual bool HitTest(const vec2f& pos) const = 0;

  virtual void MoveSet(const vec2f& m) = 0;
  virtual void MoveApply(const vec2f& m) = 0;
  virtual void MoveCancel() = 0;
};

class Node : public Object {
 public:
  Node(const Rectf &r, const std::string& t);
  ~Node();

  void Draw(wxPaintDC* dc, const ViewData& view, const DrawData& draw) const override ;
  bool HitTest(const vec2f& pos) const override;

  void MoveSet(const vec2f& m) override ;
  void MoveApply(const vec2f& m) override ;
  void MoveCancel() override ;

  Rectf rect;
  vec2f movement;
  std::string text;
};

class Tool;

class GraphData {
 public:
  GraphData();

  std::vector<std::shared_ptr<Object>> objects;
  std::vector<std::weak_ptr<Object>> selected;
  std::vector<std::shared_ptr<Tool>> tools;
  bool pop;

  bool IsSelected(std::shared_ptr<Object> o);
  void Step(GraphData* data, wxMouseEvent& event);
  void Add(std::shared_ptr<Tool> tool);
  Tool& tool();
};

class Tool {
 public:
  Tool();
  Tool(const vec2f& m);
  virtual ~Tool();

  void OnMouseMoved(GraphData* data, wxMouseEvent& event);
  void OnMouseButton(GraphData *data, wxMouseEvent &event, bool down);

  virtual void MouseMoved(GraphData* data, wxMouseEvent& event) = 0;
  virtual void MouseButton(GraphData *data, wxMouseEvent &event) = 0;
  virtual void Paint(wxPaintDC* dc, const ViewData& view, const DrawData& draw) = 0;

  virtual void Refresh(GraphData* data, wxMouseEvent& event);

 protected:
  bool mouseButtonDown;
  vec2f mousePosition;
};

class Graph : public wxPanel
{
 public:
  Graph(wxWindow* parent);
  wxSize DoGetBestSize() const;

  void OnPaint(wxPaintEvent& event);

  void OnMouseMoved(wxMouseEvent& event);
  void OnMouseUp(wxMouseEvent& event);
  void OnMouseDown(wxMouseEvent& event);
  void OnMouseButton(wxMouseEvent &event, bool down);
  void OnMouseWheelMoved(wxMouseEvent& event);
  void OnMouseRightClick(wxMouseEvent& event);
  void OnMouseLeftWindow(wxMouseEvent& event);
  void OnKeyPressed(wxKeyEvent& event);
  void OnKeyReleased(wxKeyEvent& event);

  GraphData data;

  void Invalidate(wxMouseEvent& event);

  Tool& tool();
};


#endif //WXUTIL_GRAPH_GRAPH_H
