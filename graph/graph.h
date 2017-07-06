

#ifndef WXUTIL_GRAPH_GRAPH_H
#define WXUTIL_GRAPH_GRAPH_H

#include <memory>
#include <string>

#include "wx.h"

#include "core/rect.h"
#include "core/poly2.h"

class ViewData {
 public:
  const int HorizontalConvert(float f) const;
  const int VerticalConvert(float f) const;

  wxPoint Convert(const vec2f& p) const;
  vec2f Convert(const wxPoint& p) const;

  wxSize Convert(const Sizef& s) const;
  Sizef Convert(const wxSize& s) const;
};

class DrawContext {
 public:
  DrawContext();

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

  virtual void Draw(wxPaintDC* dc, const ViewData& view, const DrawContext& context) const = 0;
  virtual bool HitTest(const vec2f& pos) const = 0;

  virtual void MoveSet(const vec2f& m) = 0;
  virtual void MoveApply(const vec2f& m) = 0;
  virtual void MoveCancel() = 0;

  bool removeThis;
};

class Node : public Object {
 public:
  Node(const Rectf &r, const std::string& t);
  ~Node();

  void Draw(wxPaintDC* dc, const ViewData& view, const DrawContext& drawContext) const override ;
  bool HitTest(const vec2f& pos) const override;

  void MoveSet(const vec2f& m) override ;
  void MoveApply(const vec2f& m) override ;
  void MoveCancel() override ;

  Rectf GetModifiedRect() const;

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

  void DeleteSelected();
  void RemoveTaggedItems();
  void RemoveDeadSelections();
  bool RemoveDeadTools(); // returns true if some tools were removed
  bool IsSelected(std::shared_ptr<Object> o);
  void Step(wxMouseEvent& event);
  void Step();
  void Add(std::shared_ptr<Tool> tool);
  Tool& tool();

  std::vector<std::shared_ptr<Object>> HitTestAll(const vec2f& mousePosition);
  std::shared_ptr<Node> HitTestTopMost(const vec2f& mousePosition);
};

class Tool {
 public:
  Tool();
  Tool(const vec2f& m);
  virtual ~Tool();

  void OnMouseMoved(GraphData* data, wxMouseEvent& event);
  void OnMouseButton(GraphData *data, wxMouseEvent &event, bool down);

  void OnPaint(wxPaintDC *dc, const ViewData &view, const DrawContext &draw);

  virtual void MouseMoved(GraphData* data, wxMouseEvent& event) = 0;
  virtual void MouseButton(GraphData *data, wxMouseEvent &event) = 0;
  virtual void Paint(wxPaintDC *dc, const ViewData &view, const DrawContext &draw) = 0;
  virtual void OnCancel(GraphData* data) = 0;

  virtual void Refresh(GraphData* data, wxMouseEvent& event);

  bool ShouldRemoveThis() const;

 protected:
  bool mouseButtonDown;
  vec2f mousePosition;

  bool removeThis;
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

  void DeleteSelected();

  // temporary
  void LinkNodes();

  void Invalidate(wxMouseEvent& event);
  void Invalidate();
  void Redraw();

  Tool& tool();
};


#endif //WXUTIL_GRAPH_GRAPH_H
