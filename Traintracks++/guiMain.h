#pragma once
#include <wx/wx.h>
#include "Canvas.h"
#include "Main.h"

class guiMain : public wxMDIParentFrame
{
public:
	guiMain();
	~guiMain();

private:
	Main main;
	wxMenuBar* m_MenuBar = nullptr;

	void OnMenuNew(wxCommandEvent& evt);

	void OnMenuOpen(wxCommandEvent& evt);
	void OnMenuSave(wxCommandEvent& evt);
	void OnMenuExit(wxCommandEvent& evt);

	Canvas* m_Canvas = nullptr;
	wxDialog* dlg = nullptr;
	wxSizer* dlgSzr = nullptr;
	wxButton* dlgBtn = nullptr;

	wxDECLARE_EVENT_TABLE();
};