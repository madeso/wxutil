#include "selectcolor.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

void ShowColorSelectionDialog() {
  wxMessageBox( "Color selection", "title", wxOK | wxICON_INFORMATION );
}