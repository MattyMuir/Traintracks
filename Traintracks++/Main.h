#pragma once
#include <wx/wx.h>
#include "Canvas.h"

class Main : public wxFrame
{
public:
	Main();
	~Main();

private:
	wxMenuBar* menuBar = nullptr;

	void OnMenuNew(wxCommandEvent&);

	void OnMenuOpen(wxCommandEvent& evt);
	void OnMenuSave(wxCommandEvent& evt);
	void OnMenuExit(wxCommandEvent& evt);

	Canvas* canvas = nullptr;

	wxDECLARE_EVENT_TABLE();
};