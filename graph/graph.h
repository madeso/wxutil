

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
};

class Node : public Object {
 public:
  Node(const Rectf &r, const std::string& t);
  ~Node();

  void Draw(wxPaintDC* dc, const ViewData& view, const DrawData& draw) const override ;
  bool HitTest(const vec2f& pos) const override;

  Rectf rect;
  std::string text;
};

class GraphData {
 public:
  std::vector<std::shared_ptr<Object>> objects;
  std::vector<std::weak_ptr<Object>> selected;

  bool IsSelected(std::shared_ptr<Object> o);
};

class Tool {
 public:
  Tool();
  virtual ~Tool();

  virtual void OnMouseMoved(GraphData* data, wxMouseEvent& event) = 0;
  virtual void OnMouse(GraphData* data, wxMouseEvent& event, bool down) = 0;
  virtual void Paint(wxPaintDC* dc, const ViewData& view, const DrawData& draw) = 0;
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
  void OnMouse(wxMouseEvent& event, bool down);
  void OnMouseWheelMoved(wxMouseEvent& event);
  void OnMouseRightClick(wxMouseEvent& event);
  void OnMouseLeftWindow(wxMouseEvent& event);
  void OnKeyPressed(wxKeyEvent& event);
  void OnKeyReleased(wxKeyEvent& event);

  GraphData data;

  std::shared_ptr<Tool> tool;

  void Invalidate();
};


#endif //WXUTIL_GRAPH_GRAPH_H
