//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnMain.h"
//#include "UnASK_1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sSkinManager"
#pragma link "sButton"
#pragma link "sEdit"
#pragma link "sToolBar"
#pragma link "sPageControl"
#pragma link "sSpeedButton"
#pragma link "sPanel"
#pragma link "sSplitter"
#pragma link "acCoolBar"
#pragma link "sBevel"
#pragma link "Unit1"
#pragma link "sLabel"
#pragma resource "*.dfm"
TMainForm *MainForm;

//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::sSpeedButton1Click(TObject *Sender)
{
//	int i;
//
//	for(i=0; i < MDIChildCount; i++)
//	  {
//		  if(MDIChildren[i]->Name == "ASK_1") return;
//	  }
//
//	TASK_1 *ask1 = new TASK_1(Application);
//	if(!ask1) return;
//	ask1->Show();

	TASK_1 *ASK_1 = new TASK_1(0, "-1");
	if(!ASK_1) return;
	ASK_1->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::sSpeedButton2Click(TObject *Sender)
{
//	TASK_2 *ASK_2 = new TASK_2(0);
//	if(!ASK_2) return;
//	ASK_2->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::sSpeedButton3Click(TObject *Sender)
{
//	int i;
//
//	for(i=0; i < MDIChildCount; i++)
//	  {
//		  if(MDIChildren[i]->Name == "LSK_1") return;
//	  }
//
//	TLSK_1 *LSK_1 = new TLSK_1(Application);
//	if(!LSK_1) return;
//	LSK_1->Show();

	TLSK_1 *LSK_1 = new TLSK_1(0, "-1");
	if(!LSK_1) return;
	LSK_1->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormMouseWheelDown(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled)
{
	if(ActiveMDIChild->WindowState != wsMaximized) ActiveMDIChild->Top += 5;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormMouseWheelUp(TObject *Sender, TShiftState Shift, TPoint &MousePos,
          bool &Handled)
{
	if(ActiveMDIChild->WindowState != wsMaximized) ActiveMDIChild->Top -= 5;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::sSpeedButton4Click(TObject *Sender)
{
//	int i;
//
//	for(i=0; i < MDIChildCount; i++)
//	  {
//		  if(MDIChildren[i]->Name == "LSK_2") return;
//	  }
//
//	TLSK_2 *LSK_2 = new TLSK_2();
//	if(!LSK_2) return;
//	LSK_2->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::sButton2Click(TObject *Sender)
{
	TASK_1 *ASK_1 = new TASK_1(0, "-1");
	if(!ASK_1) return;
	ASK_1->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::sSpeedButton5Click(TObject *Sender)
{
	int i;

	for(i=0; i < MDIChildCount; i++)
	  {
		  if(MDIChildren[i]->Name == "MK_1") return;
	  }

	TMK_1 *MK_1 = new TMK_1(Application);
	if(!MK_1) return;
	MK_1->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::sSpeedButton6Click(TObject *Sender)
{
	int i;

	for(i=0; i < MDIChildCount; i++)
	  {
		  if(MDIChildren[i]->Name == "MK_2") return;
	  }

	TMK_2 *MK_2 = new TMK_2(this, "-1");
	if(!MK_2) return;
	MK_2->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::sSpeedButton7Click(TObject *Sender)
{
	int i;

	for(i=0; i < MDIChildCount; i++)
	  {
		  if(MDIChildren[i]->Name == "IK_1") return;
	  }

	TIK_1 *IK_1 = new TIK_1(this, "-1", -1);
	if(!IK_1) return;
	IK_1->Show();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::sButton6Click(TObject *Sender)
{
	exit(0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::sButton3Click(TObject *Sender)
{
	TLSK_1 *LSK_1 = new TLSK_1(0, "-1");
	if(!LSK_1) return;
	LSK_1->Show();

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::sButton1Click(TObject *Sender)
{
	TIK_1 *IK_1 = new TIK_1(this, "-1", -1);
	if(!IK_1) return;
	IK_1->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::sButton5Click(TObject *Sender)
{
	Product = new TProduct(0);
	if(!Product) return;
	Product->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::sSpeedButton8Click(TObject *Sender)
{
//	IK_2 = new TIK_2(0);
//	if(!IK_2) return;
//	IK_2->Show();
}
//---------------------------------------------------------------------------
   #include <bass.h>

void __fastcall TMainForm::sButton4Click(TObject *Sender)
{


	MK_2 = new TMK_2(this, "-1");
	if(!MK_2) return;
	MK_2->Show();
}
//---------------------------------------------------------------------------


