#include "Main.h"

wxBEGIN_EVENT_TABLE(Main, wxFrame)
	EVT_MENU(10001, OnMenuNew)
	EVT_MENU(10002, OnMenuOpen)
	EVT_MENU(10003, OnMenuSave)
	EVT_MENU(10004, OnMenuExit)
wxEND_EVENT_TABLE()

Main::Main() : wxMDIParentFrame(nullptr, wxID_ANY, "Traintracks Solver", wxPoint(30, 30), wxSize(800, 600))
{
	menuBar = new wxMenuBar();

	SetMenuBar(menuBar);

	wxMenu* menuFile = new wxMenu();
	menuFile->Append(10001, "New\tCtrl + N");
	menuFile->Append(10002, "Open\tCtrl + O");
	menuFile->Append(10003, "Save\tCtrl + S");
	menuFile->Append(10004, "Exit\tAlt + F4");

	menuBar->Append(menuFile, "File");

	canvas = new Canvas(this);

	wxAcceleratorEntry entries[4];
	entries[0].Set(wxACCEL_CTRL, (int)'N', 10001);
	entries[1].Set(wxACCEL_CTRL, (int)'O', 10002);
	entries[2].Set(wxACCEL_CTRL, (int)'S', 10003);
	entries[3].Set(wxACCEL_ALT, WXK_F4, 10004);
	wxAcceleratorTable accel(4, entries);
	this->SetAcceleratorTable(accel);
}

Main::~Main()
{

}

void Main::OnMenuNew(wxCommandEvent& evt)
{
	dlg = new wxDialog(this, wxID_ANY, "New Puzzle", wxDefaultPosition, wxSize(200, 120));
	dlgSzr = dlg->CreateButtonSizer(0);

	wxTextCtrl* dlgTextW = new wxTextCtrl(dlg, wxID_ANY, "", wxPoint(20, 10), wxSize(50, 20));
	wxStaticText* wLab = new wxStaticText(dlg, wxID_ANY, "w:", wxPoint(5, 10));
	wxStaticText* hLab = new wxStaticText(dlg, wxID_ANY, "h:", wxPoint(75, 10));
	wxTextCtrl* dlgTextH = new wxTextCtrl(dlg, wxID_ANY, "", wxPoint(90, 10), wxSize(50, 20));
	dlgBtn = new wxButton(dlg, wxID_OK, "Ok", wxPoint(55, 40), wxSize(70, 30));

	if (dlg->ShowModal() == wxID_OK)
	{
		auto wStr = dlgTextW->GetValue().ToStdString();
		auto hStr = dlgTextH->GetValue().ToStdString();

		bool validInputs = true;
		int w, h;
		try
		{
			w = std::stoi(wStr, nullptr, 10);
			h = std::stoi(hStr, nullptr, 10);
		}
		catch (std::exception e) { validInputs = false; }

		if (w < 1 || h < 1)
			validInputs = false;

		if (validInputs)
		{
			canvas->board.Resize(w, h);
			canvas->Refresh();
		}
	}
	dlg->Destroy();
}

void Main::OnMenuOpen(wxCommandEvent& evt)
{
	wxFileDialog dlg(this, "Open Traintracks Puzzle", "", "", ".trn Files|*.trn", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (dlg.ShowModal() == wxID_OK)
		canvas->Open(dlg.GetPath().ToStdString());

	evt.Skip();
}

void Main::OnMenuSave(wxCommandEvent& evt)
{
	wxFileDialog dlg(this, "Save Traintracks Puzzle", "", "", ".trn Files|*.trn", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dlg.ShowModal() == wxID_OK)
		canvas->Save(dlg.GetPath().ToStdString());

	evt.Skip();
}

void Main::OnMenuExit(wxCommandEvent& evt)
{
	Close();
	evt.Skip();
}