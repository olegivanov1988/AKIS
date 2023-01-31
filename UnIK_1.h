//---------------------------------------------------------------------------

#ifndef UnIK_1H
#define UnIK_1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "iScope.hpp"
#include "sBitBtn.hpp"
#include "sButton.hpp"
#include "sLabel.hpp"
#include "sListBox.hpp"
#include "sPanel.hpp"
#include "sRadioButton.hpp"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "acProgressBar.hpp"
#include <ComCtrls.hpp>
#include "sGauge.hpp"
#include "UnIK_2.h"
#include "sEdit.hpp"
#include "iComponent.hpp"
#include "iCustomComponent.hpp"
#include "iSevenSegmentDisplay.hpp"
#include "iSevenSegmentInteger.hpp"
#include "iVCLComponent.hpp"
class WIk;

#define COPLING_AC 0
#define COPLING_DC 1

#define MODE_NULL			0
#define MODE_UTIL0_STEP1  	1
#define MODE_UTIL1_STEP1  	2
#define MODE_UTIL2_STEP0  	3
#define MODE_UTIL2_STEP1  	4
#define MODE_UTIL3_STEP1  	5
#define MODE_UTIL3_STEP2  	6
#define MODE_UTIL4_STEP1  	7
#define MODE_UTIL4_STEP2  	8

#define MODE_UTIL4_STEP2_1  9
#define MODE_UTIL4_STEP2_2  10

#define MODE_UTIL4_STEP3  	11
#define MODE_UTIL10_STEP1  	12
//#define MODE_UTIL4_STEP4  	8
//---------------------------------------------------------------------------
class TIK_1 : public TForm
{
__published:	// IDE-managed Components
	TiScope *iScope1;
	TsPanel *sPanel32;
	TsListBox *sListBox5;
	TsListBox *sListBox6;
	TsListBox *sListBox4;
	TsPanel *sPanel8;
	TShape *Shape3;
	TShape *Shape1;
	TsLabel *sLabel1;
	TsPanel *sPanel5;
	TsRadioButton *sRadioButton1;
	TsPanel *sPanel2;
	TsPanel *sPanel3;
	TiScope *iScope2;
	TsPanel *sPanel1;
	TsListBox *sListBox1;
	TsListBox *sListBox2;
	TsListBox *sListBox3;
	TsPanel *sPanel4;
	TsPanel *sPanel10;
	TsLabel *sLabel3;
	TsButton *sButton1;
	TsButton *sButton2;
	TsPanel *sPanel11;
	TsPanel *sPanel12;
	TsPanel *sPanel13;
	TShape *Shape12;
	TShape *Shape5;
	TShape *Shape6;
	TsLabel *sLabel4;
	TsPanel *sPanel17;
	TsRadioButton *sRadioButton3;
	TsLabel *sLabel6;
	TShape *Shape7;
	TsPanel *sPanel28;
	TsRadioButton *sRadioButton8;
	TsPanel *sPanel23;
	TsLabel *sLabel5;
	TsButton *sButton4;
	TShape *Shape8;
	TShape *Shape9;
	TShape *Shape10;
	TsLabel *sLabel7;
	TsPanel *sPanel20;
	TsRadioButton *sRadioButton4;
	TsPanel *sPanel15;
	TShape *Shape11;
	TShape *Shape13;
	TsLabel *sLabel8;
	TShape *Shape14;
	TsPanel *sPanel16;
	TsPanel *sPanel18;
	TsPanel *sPanel19;
	TsRadioButton *sRadioButton5;
	TShape *Shape15;
	TsBitBtn *sBitBtn1;
	TsPanel *sPanel22;
	TsPanel *sPanel24;
	TsPanel *sPanel25;
	TsPanel *sPanel26;
	TShape *Shape2;
	TShape *Shape4;
	TsLabel *sLabel2;
	TsPanel *sPanel6;
	TsRadioButton *sRadioButton2;
	TsPanel *sPanel7;
	TsPanel *sPanel9;
	TsPanel *sPanel27;
	TsLabel *sLabel10;
	TsButton *sButton5;
	TShape *Shape16;
	TsPanel *sPanel30;
	TsButton *sButton7;
	TsButton *sButton8;
	TsPanel *sPanel31;
	TsButton *sButton9;
	TsButton *sButton10;
	TsPanel *sPanel33;
	TsPanel *sPanel34;
	TsPanel *sPanel35;
	TsPanel *sPanel36;
	TsLabel *sLabel13;
	TsGauge *sGauge1;
	TsLabel *sLabel12;
	TsLabel *sLabel14;
	TsLabel *sLabel15;
	TsButton *sButton11;
	TsEdit *sEdit1;
	TShape *Shape20;
	TShape *Shape21;
	TShape *Shape22;
	TShape *Shape23;
	TsPanel *sPanel14;
	TsLabel *sLabel16;
	TsLabel *sLabel17;
	TiSevenSegmentInteger *iSevenSegmentInteger1;
	TsButton *sButton14;
	TTimer *Timer1;
	TTimer *Timer2;
	TsButton *sButton3;
	TsButton *sButton6;
	TsButton *sButton12;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
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
	void __fastcall sButton9Click(TObject *Sender);
	void __fastcall sButton10Click(TObject *Sender);
	void __fastcall sButton5Click(TObject *Sender);
	void __fastcall sButton4Click(TObject *Sender);
	void __fastcall sButton1Click(TObject *Sender);
	void __fastcall sButton2Click(TObject *Sender);
	void __fastcall sButton11Click(TObject *Sender);
	void __fastcall sBitBtn1Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall Timer2Timer(TObject *Sender);
	void __fastcall sButton14Click(TObject *Sender);
	void __fastcall sButton3Click(TObject *Sender);
	void __fastcall sButton7Click(TObject *Sender);
	void __fastcall sButton8Click(TObject *Sender);
	void __fastcall sButton6Click(TObject *Sender);
	void __fastcall sButton12Click(TObject *Sender);
private:	// User declarations
	static const double SecPerDivTable[19];
	static const double VoltPerDivTable[22];

	WIk *wIk;
	int WorkMode;
    int WorkMode2;
	int WorkModeEND;
	int step;
	TDateTime t_start;
	TDateTime t_end;
	TIK_2 *IK_2;
	bool inclX8X10;
	bool inclPower;
	bool inclOptions;

	AnsiString form_string6;

public:		// User declarations
	__fastcall TIK_1(TComponent* Owner, AnsiString number_block, int type_block);
	__fastcall ~TIK_1();

private:
	__fastcall void CreateTh();
	__fastcall void ResetK1K2();
	__fastcall void	ResetUtil0();
	__fastcall void ResetUtil2K3();
	__fastcall void ResetUtil3K3();
	__fastcall void ResetUtil4K3();
	__fastcall void Util0Stop();
	__fastcall void Util1Stop();
	__fastcall void Util2Stop();
	__fastcall void Util7Stop();
	__fastcall void TransitionIK2();

	__fastcall void Frequency_Calculation();	// Постоянное вычисление частоты для Fконтр. запуск

	static DWORD WINAPI ScopeThread(TIK_1 *IK_1);
	static DWORD WINAPI CheckUtil4Step2Thread(TIK_1 *IK_1);
};
//---------------------------------------------------------------------------
//extern PACKAGE TIK_1 *IK_1;
//---------------------------------------------------------------------------
#endif
