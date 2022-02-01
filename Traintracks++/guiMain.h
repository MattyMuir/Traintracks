#pragma once
#include <wx/wx.h>
#include "cCanvas.h"
#include "cMain.h"

class guiMain : public wxMDIParentFrame
{
public:
	guiMain();
	~guiMain();

private:
	cMain main;
	wxMenuBar* m_MenuBar = nullptr;

	void OnMenuNew(wxCommandEvent& evt);

	void OnMenuOpen(wxCommandEvent& evt);
	void OnMenuSave(wxCommandEvent& evt);
	void OnMenuExit(wxCommandEvent& evt);

	cCanvas* m_Canvas = nullptr;
	wxDialog* dlg = nullptr;
	wxSizer* dlgSzr = nullptr;
	wxButton* dlgBtn = nullptr;

	wxDECLARE_EVENT_TABLE();
};