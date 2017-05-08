//
// Comes from https://wxwidgets.info/how-to-create-nice-about-box-in-wxwidgets/
//

#ifndef WXUTIL_ABOUTBOX_H
#define WXUTIL_ABOUTBOX_H


#include "wx/gbsizer.h"
#include "wx/statline.h"

#define SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_TITLE _("About ")
#define SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_IDNAME ID_WXMOZILLALIKEABOUTBOXDIALOG
#define SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_POSITION wxDefaultPosition

class wxMozillaLikeAboutBoxDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS( wxMozillaLikeAboutBoxDialog )
DECLARE_EVENT_TABLE()
public:
  wxMozillaLikeAboutBoxDialog();
  wxMozillaLikeAboutBoxDialog( wxWindow* parent,
                               wxWindowID id = SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_IDNAME,
                               const wxString& caption = SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_TITLE,
                               const wxPoint& pos = SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_POSITION,
                               const wxSize& size = SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_SIZE,
                               long style = SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_STYLE );

  bool Create( wxWindow* parent,
               wxWindowID id = SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_IDNAME,
               const wxString& caption = SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_TITLE,
               const wxPoint& pos = SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_POSITION,
               const wxSize& size = SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_SIZE,
               long style = SYMBOL_WXMOZILLALIKEABOUTBOXDIALOG_STYLE );

  ~wxMozillaLikeAboutBoxDialog();

  void Init();
  void CreateControls();

  wxString GetAppName() const { return m_AppName ; }
  void SetAppName(wxString value) { m_AppName = value ; }

  wxString GetVersion() const { return m_Version ; }
  void SetVersion(wxString value) { m_Version = value ; }

  wxString GetCopyright() const { return m_Copyright ; }
  void SetCopyright(wxString value) { m_Copyright = value ; }

  wxString GetCustomBuildInfo() const { return m_CustomBuildInfo ; }
  void SetCustomBuildInfo(wxString value) { m_CustomBuildInfo = value ; }

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  enum wxBuildInfoFormat
  {
    wxBUILDINFO_SHORT,
    wxBUILDINFO_LONG
  };

  static wxString GetBuildInfo(wxBuildInfoFormat format);

  void SetHeaderBitmap(const wxBitmap & value);
  void ApplyInfo();

private:
  wxPanel* m_ContentPanel;
  wxStaticBitmap* m_HeaderStaticBitmap;
  wxStaticText* m_AppNameStaticText;
  wxStaticText* m_CopyrightStaticText;
  wxStaticText* m_VersionStaticText;
  wxStaticText* m_BuildInfoStaticText;
  wxString m_AppName;
  wxString m_Version;
  wxString m_Copyright;
  wxString m_CustomBuildInfo;
  /// Control identifiers
  enum {
    ID_WXMOZILLALIKEABOUTBOXDIALOG = 10000,
    ID_ContentPanel = 10001
  };
};

wxBitmap wxGetBitmapFromMemory(const void * data, size_t length);


#endif // WXUTIL_ABOUTBOX_H
