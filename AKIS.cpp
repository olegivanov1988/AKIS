//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("UnASK_1.cpp", ASK_1);
USEFORM("UnLSK_2.cpp", LSK_2);
USEFORM("UnMK_2.cpp", MK_2);
USEFORM("UnMK_1.cpp", MK_1);
USEFORM("UnASK_2.cpp", ASK_2);
USEFORM("UnIK_1.cpp", IK_1);
USEFORM("UnProduct.cpp", Product);
USEFORM("UnMain.cpp", MainForm);
USEFORM("UnIK_2.cpp", IK_2);
USEFORM("UnLSK_1.cpp", LSK_1);
USEFORM("Unit1.cpp", Frame1); /* TFrame: File Type */
USEFORM("UnVisibleReport.cpp", VsReport);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
