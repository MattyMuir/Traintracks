#pragma once
#include <wx/wx.h>
#include "guiMain.h"

class App : public wxApp
{
public:
	App();
	~App();

private:
	guiMain* m_frame1 = nullptr;

public:
	virtual bool OnInit();
};