//---------------------------------------------------------------------------

#ifndef UnLSK_1H
#define UnLSK_1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "sBitBtn.hpp"
#include <Buttons.hpp>
#include "iScope.hpp"
#include "sListBox.hpp"
#include "sPanel.hpp"
#include <ExtCtrls.hpp>
#include "sButton.hpp"
#include "sLabel.hpp"
#include "sRadioButton.hpp"
#include "UnLSK_2.h"
#include "sEdit.hpp"

class WLsk;
#define MODE_NULL			0
#define MODE_UTIL0			1

#define COPLING_AC 0
#define COPLING_DC 1
//---------------------------------------------------------------------------
class TLSK_1 : public TForm
{
__published:	// IDE-managed Components
	TsBitBtn *sBitBtn1;
	TShape *Shape1;
	TsPanel *sPanel14;
	TsPanel *sPanel15;
	TsPanel *sPanel8;
	TiScope *iScope1;
	TsPanel *sPanel32;
	TsListBox *sListBox2;
	TsListBox *sListBox3;
	TsListBox *sListBox1;
	TsPanel *sPanel1;
	TShape *Shape3;
	TsLabel *sLabel1;
	TsPanel *sPanel19;
	TsRadioButton *sRadioButton2;
	TsPanel *sPanel2;
	TsPanel *sPanel3;
	TsPanel *sPanel4;
	TsLabel *sLabel3;
	TsButton *sButton1;
	TsButton *sButton2;
	TsPanel *sPanel5;
	TShape *Shape10;
	TShape *Shape11;
	TShape *Shape2;
	TShape *Shape4;
	TShape *Shape5;
	TsLabel *sLabel2;
	TsPanel *sPanel6;
	TsRadioButton *sRadioButton1;
	TShape *Shape6;
	TTimer *Timer1;
	TsPanel *sPanel7;
	TShape *Shape12;
	TsPanel *sPanel17;
	TsRadioButton *sRadioButton3;
	TsPanel *sPanel21;
	TsPanel *sPanel22;
	TsLabel *sLabel5;
	TsLabel *sLabel9;
	TsLabel *sLabel8;
	TsButton *sButton7;
	TsEdit *sEdit1;
	TsLabel *sLabel10;
	TsButton *sButton9;
	TsButton *btnAuto;
	TsButton *sButton13;
	TsButton *sButton14;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall sListBox1Click(TObject *Sender);
	void __fastcall sListBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall sListBox1VScroll(TObject *Sender);
	void __fastcall sListBox2Click(TObject *Sender);
	void __fastcall sListBox2VScroll(TObject *Sender);
	void __fastcall sListBox3Click(TObject *Sender);
	void __fastcall sListBox3VScroll(TObject *Sender);
	void __fastcall sBitBtn1Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall sButton2Click(TObject *Sender);
	void __fastcall sButton1Click(TObject *Sender);
	void __fastcall sButton7Click(TObject *Sender);
	void __fastcall sButton9Click(TObject *Sender);
	void __fastcall btnAutoClick(TObject *Sender);
	void __fastcall sButton13Click(TObject *Sender);
	void __fastcall sButton14Click(TObject *Sender);
private:	// User declarations

	static const double SecPerDivTable[19];
	static const double VoltPerDivTable[22];
	WLsk *wLsk;
	bool inclPower;
	int WorkMode;
	int WorkModeEND;
	TLSK_2 *LSK_2;
	bool power;

public:		// User declarations
	__fastcall TLSK_1(TComponent* Owner, AnsiString number_block);
//     ~TLSK_1();

private:
	__fastcall void Util0Stop();
	__fastcall void CreateTh();
	__fastcall void ResetK9_X8();
	__fastcall void TransitionLSK2();
	__fastcall void CheckX8();

	static DWORD WINAPI ScopeThread(TLSK_1 *LSK_1);
};
//---------------------------------------------------------------------------
//extern PACKAGE TLSK_1 *LSK_1;
//---------------------------------------------------------------------------
#endif
