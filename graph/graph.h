

#ifndef WXUTIL_GRAPH_GRAPH_H
#define WXUTIL_GRAPH_GRAPH_H

#include <memory>

#include "wx.h"

#include "core/rect.h"

class ViewData {
 public:
  const int HorizontalConvert(float f) const;
  const int VerticalConvert(float f) const;
  wxPoint Convert(const vec2f& p) const;
  vec2f Convert(const wxPoint& p) const;
  wxSize Convert(const Sizef& s) const;
};

class DrawData {
 public:
  DrawData();
  vec2f mouse;
  bool selected;
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
  Node(const Rectf &r);
  ~Node();

  void Draw(wxPaintDC* dc, const ViewData& view, const DrawData& draw) const override ;
  bool HitTest(const vec2f& pos) const override;

  Rectf rect;
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

  int x;
  int y;
  bool down;

  std::vector<std::shared_ptr<Object>> objects;
  std::vector<Object*> selected;

  void Invalidate();
};


#endif //WXUTIL_GRAPH_GRAPH_H
