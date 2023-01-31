//---------------------------------------------------------------------------

#ifndef UnMainH
#define UnMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "sSkinManager.hpp"
#include <ExtCtrls.hpp>
//---------------------------
#include "UnASK_1.h"
#include "UnASK_2.h"
#include "UnLSK_1.h"
//#include "UnLSK_2.h"
#include "UnMK_1.h"
#include "UnMK_2.h"
#include "UnIK_1.h"
#include "UnIK_2.h"
#include "UnProduct.h"
//---------------------------
#include "sButton.hpp"
#include "sEdit.hpp"
#include "sToolBar.hpp"
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include "sPageControl.hpp"
#include <Menus.hpp>
#include "sSpeedButton.hpp"
#include <Buttons.hpp>
#include "sPanel.hpp"
#include "sSplitter.hpp"
#include "acCoolBar.hpp"
#include "sBevel.hpp"
#include "Unit1.h"
#include "sLabel.hpp"
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TsSkinManager *sSkinManager1;
	TsToolBar *sToolBar1;
	TToolButton *ToolButton1;
	TsPanel *sPanel1;
	TsBevel *sBevel1;
	TsSpeedButton *sSpeedButton1;
	TsSpeedButton *sSpeedButton2;
	TsSpeedButton *sSpeedButton3;
	TsSpeedButton *sSpeedButton4;
	TsButton *sButton1;
	TsButton *sButton2;
	TsButton *sButton3;
	TsButton *sButton4;
	TsButton *sButton5;
	TsButton *sButton6;
	TsSpeedButton *sSpeedButton5;
	TsSpeedButton *sSpeedButton6;
	TsSpeedButton *sSpeedButton7;
	TsSpeedButton *sSpeedButton8;
	TsLabel *sLabel1;
	void __fastcall sSpeedButton1Click(TObject *Sender);
	void __fastcall sSpeedButton2Click(TObject *Sender);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift, TPoint &MousePos,
          bool &Handled);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift, TPoint &MousePos,
          bool &Handled);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall sSpeedButton3Click(TObject *Sender);
	void __fastcall sSpeedButton4Click(TObject *Sender);
	void __fastcall sButton2Click(TObject *Sender);
	void __fastcall sSpeedButton5Click(TObject *Sender);
	void __fastcall sSpeedButton6Click(TObject *Sender);
	void __fastcall sSpeedButton7Click(TObject *Sender);
	void __fastcall sButton6Click(TObject *Sender);
	void __fastcall sButton3Click(TObject *Sender);
	void __fastcall sButton1Click(TObject *Sender);
	void __fastcall sButton5Click(TObject *Sender);
	void __fastcall sSpeedButton8Click(TObject *Sender);
	void __fastcall sButton4Click(TObject *Sender);


private:	// User declarations
	TForm *p;

public:		// User declarations
	TASK_1 *ASK_1;
	TASK_2 *ASK_2;
	TLSK_1 *LSK_1;
	TLSK_2 *LSK_2;
	TMK_1 *MK_1;
	TMK_2 *MK_2;
	TIK_1 *IK_1;
	TIK_2 *IK_2;
	TProduct *Product;
	TFrame1 *Frame1;
	__fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
