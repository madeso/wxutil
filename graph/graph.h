

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
  Object(const Rectf& r);
  ~Object();

  void Draw(wxPaintDC* dc, const ViewData& view, const DrawData& draw);
  bool HitTest(const vec2f& pos);

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
