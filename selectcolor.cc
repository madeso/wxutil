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


    wxPanel* color = new wxPanel(this);
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