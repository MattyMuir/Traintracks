#pragma once
#include <wx/wx.h>
#include "Canvas.h"

class Main : public wxMDIParentFrame
{
public:
	Main();
	~Main();

private:
	wxMenuBar* menuBar = nullptr;

	void OnMenuNew(wxCommandEvent& evt);

	void OnMenuOpen(wxCommandEvent& evt);
	void OnMenuSave(wxCommandEvent& evt);
	void OnMenuExit(wxCommandEvent& evt);

	Canvas* canvas = nullptr;
	wxDialog* dlg = nullptr;
	wxSizer* dlgSzr = nullptr;
	wxButton* dlgBtn = nullptr;

	wxDECLARE_EVENT_TABLE();
};