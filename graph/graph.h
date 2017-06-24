

#ifndef WXUTIL_GRAPH_GRAPH_H
#define WXUTIL_GRAPH_GRAPH_H

#include "wx.h"

#include "core/rect.h"

class ViewData {
 public:
  const int HorizontalConvert(float f) const;
  const int VerticalConvert(float f) const;
  wxPoint Convert(const vec2f& p) const;
  wxSize Convert(const Sizef& s) const;
};

class Object {
 public:
  Object();
  ~Object();
  void Draw(wxPaintDC* dc, const ViewData& view);

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

  void Invalidate();
};


#endif //WXUTIL_GRAPH_GRAPH_H
