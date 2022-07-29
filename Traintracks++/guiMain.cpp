#include "guiMain.h"

wxBEGIN_EVENT_TABLE(guiMain, wxFrame)
	EVT_MENU(10001, OnMenuNew)
	EVT_MENU(10002, OnMenuOpen)
	EVT_MENU(10003, OnMenuSave)
	EVT_MENU(10004, OnMenuExit)
wxEND_EVENT_TABLE()

guiMain::guiMain() : wxMDIParentFrame(nullptr, wxID_ANY, "Traintracks Solver", wxPoint(30, 30), wxSize(800, 600))
{
	main = Main();
	m_MenuBar = new wxMenuBar();

	this->SetMenuBar(m_MenuBar);

	wxMenu* menuFile = new wxMenu();
	menuFile->Append(10001, "New\tCtrl + N");
	menuFile->Append(10002, "Open\tCtrl + O");
	menuFile->Append(10003, "Save\tCtrl + S");
	menuFile->Append(10004, "Exit\tAlt + F4");

	m_MenuBar->Append(menuFile, "File");

	m_Canvas = new Canvas(this, &main.board, &main.colLabels, &main.rowLabels, &main.gridW, &main.gridH);
	m_Canvas->mainPtr = &main;

	wxAcceleratorEntry entries[4];
	entries[0].Set(wxACCEL_CTRL, (int)'N', 10001);
	entries[1].Set(wxACCEL_CTRL, (int)'O', 10002);
	entries[2].Set(wxACCEL_CTRL, (int)'S', 10003);
	entries[3].Set(wxACCEL_ALT, WXK_F4, 10004);
	wxAcceleratorTable accel(4, entries);
	this->SetAcceleratorTable(accel);
}

guiMain::~guiMain()
{

}

void guiMain::OnMenuNew(wxCommandEvent& evt)
{
	dlg = new wxDialog(this, wxID_ANY, "New Puzzle", wxDefaultPosition, wxSize(200, 120));
	//dlgBtn = new wxButton(dlg, wxID_ANY, "Ok", wxPoint(10, 10), wxSize(50, 50), 0);
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

		bool isNumeric = true;
		int w, h;
		try
		{
			w = std::stoi(wStr, nullptr, 10);
			h = std::stoi(hStr, nullptr, 10);
		}
		catch (std::exception e)
		{
			isNumeric = false;
		}

		if (w < 1 || h < 1)
			isNumeric = false;

		if (isNumeric)
		{
			main.gridH = h;
			main.gridW = w;
			main.NewBoard();
			main.NewLabels();
			m_Canvas->m_Refresh();
		}
	}
	else
	{

	}
	dlg->Destroy();
}

void guiMain::OnMenuOpen(wxCommandEvent& evt)
{
	wxFileDialog dlg(this, "Open Traintracks Puzzle", "", "", ".trn Files|*.trn", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (dlg.ShowModal() == wxID_OK)
	{
		m_Canvas->Open(dlg.GetPath().ToStdString());
	}
	evt.Skip();
}

void guiMain::OnMenuSave(wxCommandEvent& evt)
{
	wxFileDialog dlg(this, "Save Traintracks Puzzle", "", "", ".trn Files|*.trn", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dlg.ShowModal() == wxID_OK)
	{
		m_Canvas->Save(dlg.GetPath().ToStdString());
	}
	evt.Skip();
}

void guiMain::OnMenuExit(wxCommandEvent& evt)
{
	Close();
	evt.Skip();
}