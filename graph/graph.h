

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
};


#endif //WXUTIL_GRAPH_GRAPH_H
