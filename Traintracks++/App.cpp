#include "App.h"

wxIMPLEMENT_APP(App);

App::App()
{

}

App::~App()
{

}

bool App::OnInit()
{
	freopen("log.log", "w", stdout);
	freopen("err.log", "w", stderr);

	m_frame1 = new guiMain();
	m_frame1->Show();
	return true;
}