#include "selectcolor.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/spinctrl.h>

wxFlexGridSizer* CreateSizer(int count) {
  wxFlexGridSizer* properties = new wxFlexGridSizer(count, 3, wxSize(3, 3));
  properties->AddGrowableCol(1, 1);
  return properties;
}

void AddWindow(wxWindow* parent, wxFlexGridSizer* sizer, const wxString& name, wxWindow* b, const wxString& unit) {
  wxStaticText* a = new wxStaticText(parent, wxID_ANY, name);
  wxStaticText* c = new wxStaticText(parent, wxID_ANY, unit);

  sizer->Add(a);
  sizer->Add(b);
  sizer->Add(c);
}

void AddNumber(wxWindow* parent, wxFlexGridSizer* sizer, const wxString& name, int min, int max, const wxString& unit) {
  wxSpinCtrl* b = new wxSpinCtrl(parent);
  b->SetRange(min, max);
  AddWindow(parent, sizer, name + ":", b, unit);
}

void AddText(wxWindow* parent, wxFlexGridSizer* sizer, const wxString& name, const wxString& unit) {
  wxTextCtrl* b = new wxTextCtrl(parent, wxID_ANY);
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

class ColorPanel : public wxPanel {
 public:
  ColorPanel(wxWindow* parent) : wxPanel(parent), hue(0) {
    SetCursor(*wxCROSS_CURSOR);
    SetMinSize(wxSize(101, 101));
    Bind(wxEVT_PAINT, &ColorPanel::OnPaint, this);
    Bind(wxEVT_SIZE, &ColorPanel::OnResize, this);
  }

  void SetHue(int h) {
    hue = h;
    Refresh(false);
  }

  void OnPaint(wxPaintEvent& event)
  {
    wxPaintDC dc(this);

    wxImage image(101, 101);
    for(int y=0; y<101; ++y) {
      for(int x=0; x<101; ++x) {
        const wxColor c = HsvToRgb(hue, x, y);
        image.SetRGB(x, y, c.Red(), c.Green(), c.Blue());
      }
    }
    wxSize size = dc.GetSize();
    image.Rescale(size.x, size.y, wxIMAGE_QUALITY_NEAREST);
    dc.DrawBitmap(wxBitmap(image), 0, 0);
  }

  void OnResize(wxSizeEvent& event)
  {
    Refresh(false);
  }

 private:
  int hue;
};

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
    wxFlexGridSizer* properties = new wxFlexGridSizer(2, 2, wxSize(3, 3));
    properties->AddGrowableRow(0, 1);
    properties->AddGrowableCol(0, 1);
    properties->Add(rgbhex);
    properties->Add(cmyk);
    properties->Add(hsb);
    properties->Add(lab);

    wxPanel* color = new ColorPanel(this);
    color->SetBackgroundColour(wxColor(0,0,0));
    color->SetMinSize(wxSize(100, 100));
    colordialog->Add(color, 1, wxEXPAND | wxALL, 10);
    colordialog->Add(properties, 0, wxALL, 10);

    topsizer->Add(colordialog, 1, wxEXPAND | wxALL, 10);
    topsizer->Add(CreateButtonSizer(wxOK| wxCANCEL), 0, wxALL, 10);

    SetSizerAndFit(topsizer);
  }
};

void ShowColorSelectionDialog(wxWindow* parent) {
  ColorDialog d(parent);
  d.ShowModal();
}