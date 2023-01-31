//---------------------------------------------------------------------------

#ifndef UnIK_2H
#define UnIK_2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "iScope.hpp"
#include "sButton.hpp"
#include "sLabel.hpp"
#include "sListBox.hpp"
#include "sPanel.hpp"
#include "sRadioButton.hpp"
#include <ExtCtrls.hpp>
#include "sBitBtn.hpp"
#include <Buttons.hpp>
#include "iComponent.hpp"
#include "iCustomComponent.hpp"
#include "iSevenSegmentDisplay.hpp"
#include "iSevenSegmentInteger.hpp"
#include "iVCLComponent.hpp"
class WIk;

#define MODE_NULL			0
#define MODE_UTIL4_STEP4  	1
#define MODE_UTIL5_STEP1  	2
#define MODE_UTIL5_STEP2  	3
#define MODE_UTIL6_STEP1  	4
#define MODE_UTIL6_STEP2  	5
#define MODE_UTIL6_STEP3  	6
#define MODE_UTIL6_STEP4    7
#define MODE_UTIL7_STEP1  	8
#define MODE_UTIL7_STEP2  	9
#define MODE_UTIL7_STEP3  	10
#define MODE_UTIL7_STEP4  	11
#define MODE_UTIL7_STEP5  	12
//---------------------------------------------------------------------------
class TIK_2 : public TForm
{
__published:	// IDE-managed Components
	TiScope *iScope1;
	TsPanel *sPanel32;
	TsListBox *sListBox5;
	TsListBox *sListBox6;
	TsListBox *sListBox4;
	TiScope *iScope2;
	TsPanel *sPanel1;
	TsListBox *sListBox1;
	TsListBox *sListBox2;
	TsListBox *sListBox3;
	TsPanel *sPanel2;
	TsButton *sButton4;
	TsButton *sButton5;
	TsButton *sButton6;
	TsPanel *sPanel3;
	TsPanel *sPanel8;
	TsLabel *sLabel6;
	TShape *Shape7;
	TsPanel *sPanel28;
	TsRadioButton *sRadioButton8;
	TShape *Shape11;
	TsLabel *sLabel1;
	TShape *Shape1;
	TsPanel *sPanel4;
	TsRadioButton *sRadioButton1;
	TShape *Shape2;
	TsLabel *sLabel2;
	TsPanel *sPanel5;
	TsRadioButton *sRadioButton2;
	TsPanel *sPanel14;
	TsPanel *sPanel23;
	TShape *Shape17;
	TsLabel *sLabel11;
	TsPanel *sPanel27;
	TsRadioButton *sRadioButton7;
	TShape *Shape6;
	TsPanel *sPanel13;
	TShape *Shape8;
	TShape *Shape9;
	TShape *Shape10;
	TsLabel *sLabel7;
	TsPanel *sPanel12;
	TsRadioButton *sRadioButton6;
	TsPanel *sPanel15;
	TsPanel *sPanel16;
	TsPanel *sPanel17;
	TsBitBtn *sBitBtn1;
	TsPanel *sPanel18;
	TShape *Shape12;
	TsLabel *sLabel9;
	TShape *Shape14;
	TsPanel *sPanel21;
	TsRadioButton *sRadioButton10;
	TShape *Shape15;
	TsLabel *sLabel12;
	TsPanel *sPanel22;
	TsRadioButton *sRadioButton11;
	TsPanel *sPanel24;
	TsPanel *sPanel25;
	TsPanel *sPanel26;
	TShape *Shape3;
	TsLabel *sLabel3;
	TsPanel *sPanel6;
	TsRadioButton *sRadioButton3;
	TsPanel *sPanel9;
	TsPanel *sPanel10;
	TShape *Shape4;
	TsLabel *sLabel4;
	TsPanel *sPanel7;
	TsRadioButton *sRadioButton4;
	TsPanel *sPanel29;
	TsLabel *sLabel13;
	TsButton *sButton2;
	TShape *Shape5;
	TsLabel *sLabel5;
	TsPanel *sPanel11;
	TsRadioButton *sRadioButton5;
	TTimer *Timer1;
	TsPanel *sPanel30;
	TsLabel *sLabel14;
	TsLabel *sLabel15;
	TsLabel *sLabel16;
	TsPanel *sPanel20;
	TsLabel *sLabel10;
	TsButton *sButton1;
	TShape *Shape22;
	TShape *Shape21;
	TShape *Shape16;
	TsButton *sButton7;
	TsButton *sButton8;
	TsButton *sButton9;
	TiSevenSegmentInteger *iSevenSegmentInteger1;
	TsButton *sButton11;
	TsPanel *sPanel19;
	TsPanel *sPanel31;
	TsPanel *sPanel33;
	TsButton *sButton3;
	TsButton *sButton10;
	TsButton *btnAuto;
	TTimer *timeAuto;
	TTimer *timeZader;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall sButton2Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall sButton1Click(TObject *Sender);
	void __fastcall sListBox4Click(TObject *Sender);
	void __fastcall sListBox5Click(TObject *Sender);
	void __fastcall sListBox6Click(TObject *Sender);
	void __fastcall sListBox3Click(TObject *Sender);
	void __fastcall sListBox1Click(TObject *Sender);
	void __fastcall sListBox2Click(TObject *Sender);
	void __fastcall sListBox4MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall sListBox4VScroll(TObject *Sender);
	void __fastcall sListBox3VScroll(TObject *Sender);
	void __fastcall sListBox5VScroll(TObject *Sender);
	void __fastcall sListBox1VScroll(TObject *Sender);
	void __fastcall sListBox6VScroll(TObject *Sender);
	void __fastcall sListBox2VScroll(TObject *Sender);
	void __fastcall sButton7Cl(TObject *Sender);
	void __fastcall sButton8Click(TObject *Sender);
	void __fastcall sButton9Click(TObject *Sender);
	void __fastcall sBitBtn1Click(TObject *Sender);
	void __fastcall sButton11Click(TObject *Sender);
	void __fastcall sButton4Click(TObject *Sender);
	void __fastcall sButton5Click(TObject *Sender);
	void __fastcall sButton6Click(TObject *Sender);
	void __fastcall sButton3Click(TObject *Sender);
	void __fastcall sButton10Click(TObject *Sender);
	void __fastcall btnAutoClick(TObject *Sender);
	void __fastcall timeZaderTimer(TObject *Sender);
	void __fastcall timeAutoTimer(TObject *Sender);
private:	// User declarations
	static const double SecPerDivTable[19];
	static const double VoltPerDivTable[22];

	WIk *wIk;
	int WorkMode;
	int step;
//	int secK4, secK5;
	bool checkK4, checkK5;
	HANDLE hnd_scope_thread2;
	bool inclPower;
	int stepUprImp2;
	int stepUprImp3;
//	bool power;
public:		// User declarations
	__fastcall TIK_2(TComponent* Owner, WIk *wIk);
	__fastcall ~TIK_2();

private:
	__fastcall void CreateTh();
	__fastcall void Util4Stop();
	__fastcall void CheckEndIK2();
	__fastcall void ResetUtil5();
	__fastcall void ResetUtil6();
	__fastcall void ResetUtil7();


	static DWORD WINAPI ScopeThread(TIK_2 *IK_2);
};
//---------------------------------------------------------------------------
//extern PACKAGE TIK_2 *IK_2;
//---------------------------------------------------------------------------
#endif
