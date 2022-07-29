#pragma once
#include <wx/wx.h>
#include "guiMain.h"

class cApp : public wxApp
{
public:
	cApp();
	~cApp();

private:
	guiMain* m_frame1 = nullptr;

public:
	virtual bool OnInit();
};