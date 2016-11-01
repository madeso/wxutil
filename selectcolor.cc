#include "selectcolor.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

#include <wx/spinctrl.h>

wxFlexGridSizer* CreateSizer(int count) {
  wxFlexGridSizer* properties = new wxFlexGridSizer(count, 3, wxSize(3, 3));
  properties->AddGrowableCol(1, 1);
  return properties;
}

void AddWindow(wxWindow* parent, wxFlexGridSizer* sizer, const wxString& name, wxWindow* b, const wxString& unit) {
  wxStaticText* a = new wxStaticText(parent, wxID_ANY, name);
  wxStaticText* c = new wxStaticText(parent, wxID_ANY, unit);

  sizer->Add(a, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 0);
  sizer->Add(b, 1, wxEXPAND | wxALL, 0);
  sizer->Add(c, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 0);
}

void SetMaxSize(wxWindow* b) { b->SetMaxSize(wxSize(75, 400)); }

void AddNumber(wxWindow* parent, wxFlexGridSizer* sizer, const wxString& name, int min, int max, const wxString& unit) {
  wxSpinCtrl* b = new wxSpinCtrl(parent);
  b->SetRange(min, max);
  b->SetValue(max);
  SetMaxSize(b);
  AddWindow(parent, sizer, name + ":", b, unit);
}

void AddText(wxWindow* parent, wxFlexGridSizer* sizer, const wxString& name, const wxString& unit) {
  wxTextCtrl* b = new wxTextCtrl(parent, wxID_ANY);
  b->SetMaxLength(6);
  b->SetValue("FAFAFA");
  SetMaxSize(b);
  AddWindow(parent, sizer, name, b, unit);
}

int SelectMax(float a, float b, float c) {
  if( a > b && a > c ) return 0;
  else if( b > c ) return 1;
  else return 2;
}

int SelectMin(float a, float b, float c) {
  if( a < b && a < c ) return 0;
  else if( b < c ) return 1;
  else return 2;
}

float GetMax(float a, float b, float c) {
  return std::max(a, std::max(b, c));
}

float GetMin(float a, float b, float c) {
  return std::min(a, std::min(b, c));
}

void RgbToHsv(const wxColor& c, int* h, int* s, int* v) {
  // http://www.rapidtables.com/convert/color/rgb-to-hsv.htm
  const float r = c.Red() / 255.0f;
  const float g = c.Green() / 255.0f;
  const float b = c.Blue() / 255.0f;
  const int cmaxi = SelectMax(r, g, b);
  const float cmax = GetMax(r, g, b);
  const float cmin = GetMin(r, g, b);
  const float delta = cmax - cmin;

  const float closetozero = 0.0001f;

  if( delta < closetozero ) *h = 0;
  else if (cmaxi == 0) *h = 60 * static_cast<int>((g-b)/delta)%6;
  else if (cmaxi == 1) *h = 60 * ((b-r)/delta + 2);
  else  *h = 60 * ((r-g)/delta + 4);

  if( cmax < closetozero ) *s = 0.0f;
  else *s = delta / cmax;

  *v = cmax;
}

wxColor HsvToRgb(int h, int s0, int v0) {
  // http://www.rapidtables.com/convert/color/hsv-to-rgb.htm
  const float s = s0 / 100.0f;
  const float v = v0 / 100.0f;
  const float c = v * s;
  const float x = c * (1.0f - std::abs((h/60)%2-1) );
  const float m = v - c;

  float r;
  float g;
  float b;

  if( h < 60 ) {
    r = c;
    g = x;
    b = 0;
  } else if( h  < 120) {
    r = x;
    g = c;
    b = 0;
  } else if( h  < 180) {
    r = 0;
    g = c;
    b = x;
  } else if( h  < 240) {
    r = 0;
    g = x;
    b = c;
  } else if( h  < 300) {
    r = x;
    g = 0;
    b = c;
  }
  else {
    r = c;
    g = 0;
    b = x;
  }

  return wxColor((r+m)*255.0f, (g+m)*255.0f, (b+m)*255.0f);
}

class ColorModel {
 public:
  ColorModel(const wxSize& minSize) : minSize_(minSize) {}
  virtual ~ColorModel() {}
  virtual void Render(wxPaintDC* dc) = 0;

  const wxSize& GetMinSize() const { return minSize_; }
 private:
  wxSize minSize_;
};

void Rescale(wxImage &image, int width, int height, wxImageResizeQuality quality) {
  if( height <= 0) return;
  if( width <= 0) return;
  image.Rescale(width, height, quality);
}

const int SLIDER_SIZE = 30;

class RectangleModel : public ColorModel {
 private:
  static const int SPACE = 12;
  static const int SLIDER = SLIDER_SIZE;
  static wxSize CalculateMinSize(const wxSize& rectangleSize, int sliderMax) {
    return wxSize(rectangleSize.x+1 + SPACE + SLIDER, std::max(rectangleSize.y+1, sliderMax+1));
  }
 public:
  RectangleModel(const wxSize& rectangleSize, int sliderMax) : ColorModel(CalculateMinSize(rectangleSize, sliderMax)), rectangleSize_(rectangleSize), sliderMax_(sliderMax) {}
  ~RectangleModel() {}

  virtual wxColor GetColor(int x, int y) const = 0;
  virtual wxColor GetColor(int y) const = 0;

  void Render(wxPaintDC* dc) {
    wxImage rectangle(rectangleSize_.x+1, rectangleSize_.y+1);
    for(int y=0; y<=rectangleSize_.y; ++y) {
      for(int x=0; x<=rectangleSize_.x; ++x) {
        const wxColor c = GetColor(x,y);
        rectangle.SetRGB(x, y, c.Red(), c.Green(), c.Blue());
      }
    }

    wxImage slider(1, sliderMax_+1);
    for(int y=0; y<=sliderMax_; ++y) {
      const wxColor c = GetColor(y);
      slider.SetRGB(0, y, c.Red(), c.Green(), c.Blue());
    }

    wxSize size = dc->GetSize();
    Rescale(rectangle, size.x-SPACE-SLIDER, size.y, wxIMAGE_QUALITY_NEAREST);
    Rescale(slider, SLIDER, size.y, wxIMAGE_QUALITY_NEAREST);

    dc->Clear();
    dc->DrawBitmap(wxBitmap(rectangle), 0, 0);
    dc->DrawBitmap(wxBitmap(slider), size.x-SLIDER, 0);
  }
 private:
  wxSize rectangleSize_;
  int sliderMax_;
};

class HsvColorModel : public RectangleModel {
 public:
  HsvColorModel() : RectangleModel(wxSize(100, 100), 360) {}
  ~HsvColorModel() {}

  wxColor GetColor(int x) const {
    return HsvToRgb(x, 50, 50);
  }

  wxColor GetColor(int x, int y) const {
    return HsvToRgb(0, x, y);
  }
};

class ColorPanel : public wxPanel {
 public:
  ColorPanel(wxWindow* parent) : wxPanel(parent), currentModel_( new HsvColorModel() ) {
    SetCursor(*wxCROSS_CURSOR);

    SetMinSize(currentModel_->GetMinSize());
    Bind(wxEVT_PAINT, &ColorPanel::OnPaint, this);
    Bind(wxEVT_SIZE, &ColorPanel::OnResize, this);
  }

  void OnPaint(wxPaintEvent& event)
  {
    wxPaintDC dc(this);
    wxColor b = wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK);
    dc.SetBackground(wxBrush(b));
    currentModel_->Render(&dc);
  }

  void OnResize(wxSizeEvent& event)
  {
    Refresh(false);
  }

 private:
  std::shared_ptr<ColorModel> currentModel_;
};

const wxColour ColorAlpha(const wxColor &color, int i) {
  return wxColor(color.Red(), color.Green(), color.Blue(), 255);
}

class AlphaPanel : public wxPanel {
 private:
  static const int CHECK_SIZE = 12;
  static const int CHECKER_COUNT = 4;
  static const int grey = 200;
 public:
  AlphaPanel(wxWindow* parent) : wxPanel(parent), color(255, 0, 0) {
    SetCursor(*wxCROSS_CURSOR);

    SetMinSize(wxSize(CHECK_SIZE*CHECKER_COUNT, CHECK_SIZE*CHECKER_COUNT));
    Bind(wxEVT_PAINT, &AlphaPanel::OnPaint, this);
    Bind(wxEVT_SIZE, &AlphaPanel::OnResize, this);
  }

  void OnPaint(wxPaintEvent& event)
  {
    wxImage checkers(CHECK_SIZE*4, CHECK_SIZE*4);
    for(int x=0; x<CHECK_SIZE*4; ++x) {
      for(int y=0; y<CHECK_SIZE*4; ++y) {
        const bool ch = x/CHECK_SIZE%2==y/CHECK_SIZE%2;
        const int c = ch ? 255 : grey;
        checkers.SetRGB(x, y, c, c, c);
      }
    }
    wxPaintDC dc(this);
    wxSize size = dc.GetSize();dc.SetBrush(wxColor(255, 255, 255));
    dc.SetBrush(wxBrush(wxBitmap(checkers)));
    dc.DrawRectangle(size);
    dc.
    dc.GradientFillLinear(size, ColorAlpha(color, 255), ColorAlpha(color, 0));
  }

  void OnResize(wxSizeEvent& event)
  {
    Refresh(false);
  }

 private:
  wxColor color;
};

wxBoxSizer* CreateColorSelector(wxWindow* parent, const wxString& name, const wxColor c) {
  wxBoxSizer* s = new wxBoxSizer(wxVERTICAL);
  wxStaticText* text = new wxStaticText(parent, wxID_ANY, name);
  wxPanel* col = new wxPanel(parent);

  col->SetMinSize(wxSize(70, 25));
  col->SetBackgroundColour(c);

  s->Add(text, 0, wxCENTER | wxALL, 0);
  s->Add(col, 1, wxEXPAND | wxALL, 0);
  return s;
}

class ColorDialog : public wxDialog {
 public:
  ColorDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Select Color") {
    wxBoxSizer* topsizer = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer* rgbhex = CreateSizer(4);
    AddNumber(this, rgbhex, "R", 0, 255, "");
    AddNumber(this, rgbhex, "G", 0, 255, "");
    AddNumber(this, rgbhex, "B", 0, 255, "");
    AddText(this, rgbhex, "#", "");

    wxFlexGridSizer* cmyk = CreateSizer(4);
    AddNumber(this, cmyk, "C", 0, 100, "%");
    AddNumber(this, cmyk, "M", 0, 100, "%");
    AddNumber(this, cmyk, "Y", 0, 100, "%");
    AddNumber(this, cmyk, "K", 0, 100, "%");

    wxFlexGridSizer* hsb = CreateSizer(3);
    AddNumber(this, hsb, "H", 0, 365, "°");
    AddNumber(this, hsb, "S", 0, 100, "%");
    AddNumber(this, hsb, "B", 0, 100, "%");

    wxFlexGridSizer* lab = CreateSizer(3);
    AddNumber(this, lab, "L", 0, 255, "");
    AddNumber(this, lab, "a", 0, 255, "");
    AddNumber(this, lab, "b", 0, 255, "");

    wxBoxSizer* colordialog = new wxBoxSizer(wxHORIZONTAL);
    wxFlexGridSizer* properties = new wxFlexGridSizer(3, 2, wxSize(3, 3));
    properties->AddGrowableRow(0, 1);
    properties->AddGrowableCol(0, 1);
    properties->Add(CreateColorSelector(this, "Current", wxColor(0, 0, 0)), 1, wxEXPAND | wxALL, 3);
    properties->Add(CreateColorSelector(this, "New", wxColor(255, 0, 0)), 1, wxEXPAND | wxALL, 3);

    properties->Add(rgbhex, 0, wxALL, 6);
    properties->Add(cmyk, 0, wxALL, 6);
    properties->Add(hsb, 0, wxALL, 6);
    properties->Add(lab, 0, wxALL, 6);

    ColorPanel* color = new ColorPanel(this);
    color->SetBackgroundColour(wxColor(0,0,0));
    color->SetMinSize(wxSize(100, 100));

    wxComboBox* colorModelSelection = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN);

    wxBoxSizer* left = new wxBoxSizer(wxVERTICAL);
    left->Add(colorModelSelection, 0, wxCENTER | wxALL, 10);
    left->Add(color, 1, wxEXPAND | wxALL, 10);

    AlphaPanel* alpha = new AlphaPanel(this);

    colordialog->Add(left, 1, wxEXPAND | wxALL, 10);
    colordialog->Add(properties, 0, wxALL, 10);
    colordialog->Add(alpha, 0, wxEXPAND | wxALL, 10);

    topsizer->Add(colordialog, 1, wxEXPAND | wxALL, 10);
    topsizer->Add(CreateButtonSizer(wxOK| wxCANCEL), 0,  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 10);

    SetSizerAndFit(topsizer);
  }
};

void ShowColorSelectionDialog(wxWindow* parent) {
  ColorDialog d(parent);
  d.ShowModal();
}