//---------------------------------------------------------------------------

#ifndef UnMK_2H
#define UnMK_2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "iComponent.hpp"
#include "iCustomComponent.hpp"
#include "iLed.hpp"
#include "iLedArrow.hpp"
#include "iVCLComponent.hpp"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "iScope.hpp"
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sLabel.hpp"
#include "sListBox.hpp"
#include "sPanel.hpp"
#include "sRadioButton.hpp"
#include "sDialogs.hpp"
#include <Dialogs.hpp>
#include "sEdit.hpp"
#include "sBitBtn.hpp"
#include <Buttons.hpp>
#include "iSevenSegmentDisplay.hpp"
#include "iSevenSegmentInteger.hpp"
class WMk;

#define MODE_NULL	0
#define MODE_UTIL0_STEP1  	1
#define MODE_UTIL1_STEP1  	2
#define MODE_UTIL2_STEP1  	3
#define MODE_UTIL3_STEP1  	4
#define MODE_UTIL4_STEP1  	5
#define MODE_UTIL5_STEP1  	6
#define MODE_UTIL6_STEP1  	7
#define POWER_PAUSE  	   	8
//---------------------------------------------------------------------------
class TMK_2 : public TForm
{
__published:	// IDE-managed Components
	TsBevel *sBevel1;
	TsButton *sButton1;
	TsButton *sButton2;
	TsButton *sButton3;
	TsLabel *sLabel5;
	TShape *Shape1;
	TShape *Shape10;
	TShape *Shape11;
	TsPanel *sPanel24;
	TsPanel *sPanel2;
	TiScope *iScope1;
	TsPanel *sPanel32;
	TsListBox *sListBox5;
	TsListBox *sListBox6;
	TsListBox *sListBox4;
	TiScope *iScope2;
	TsPanel *sPanel3;
	TsListBox *sListBox1;
	TsListBox *sListBox2;
	TsListBox *sListBox3;
	TsPanel *sPanel8;
	TiScope *iScope3;
	TsPanel *sPanel9;
	TsListBox *sListBox7;
	TsListBox *sListBox8;
	TsListBox *sListBox9;
	TsPanel *sPanel10;
	TsButton *sButton4;
	TsButton *sButton5;
	TsPanel *sPanel11;
	TShape *Shape6;
	TShape *Shape7;
	TShape *Shape8;
	TsLabel *sLabel4;
	TsPanel *sPanel17;
	TsRadioButton *sRadioButton4;
	TsPanel *sPanel12;
	TShape *Shape9;
	TsLabel *sLabel6;
	TsPanel *sPanel13;
	TsRadioButton *sRadioButton6;
	TsPanel *sPanel38;
	TsLabel *sLabel17;
	TsButton *sButton11;
	TsPanel *sPanel15;
	TsPanel *sPanel16;
	TShape *Shape14;
	TsLabel *sLabel8;
	TsPanel *sPanel18;
	TsRadioButton *sRadioButton8;
	TsPanel *sPanel20;
	TShape *Shape15;
	TsLabel *sLabel9;
	TsPanel *sPanel21;
	TsRadioButton *sRadioButton9;
	TsPanel *sPanel22;
	TsPanel *sPanel23;
	TShape *Shape16;
	TShape *Shape18;
	TsLabel *sLabel10;
	TsPanel *sPanel25;
	TsRadioButton *sRadioButton10;
	TsPanel *sPanel26;
	TsPanel *sPanel1;
	TsRadioButton *sRadioButton1;
	TsLabel *sLabel1;
	TShape *Shape4;
	TsPanel *sPanel7;
	TsRadioButton *sRadioButton5;
	TShape *Shape13;
	TsLabel *sLabel7;
	TsPanel *sPanel14;
	TsRadioButton *sRadioButton7;
	TTimer *Timer1;
	TsOpenDialog *sOpenDialog1;
	TsLabel *sLabel11;
	TsButton *sButton7;
	TsEdit *sEdit1;
	TsLabel *sLabel12;
	TsLabel *sLabel13;
	TsBitBtn *sBitBtn1;
	TsButton *sButton8;
	TsButton *sButton9;
	TsButton *sButton10;
	TsPanel *sPanel27;
	TsPanel *sPanel28;
	TShape *Shape19;
	TShape *Shape20;
	TsPanel *sPanel29;
	TsPanel *sPanel35;
	TShape *Shape3;
	TShape *Shape2;
	TShape *Shape23;
	TsLabel *sLabel15;
	TsPanel *sPanel4;
	TsPanel *sPanel5;
	TsPanel *sPanel6;
	TsPanel *sPanel19;
	TsPanel *sPanel30;
	TsPanel *sPanel31;
	TsPanel *sPanel33;
	TsPanel *sPanel34;
	TsPanel *sPanel36;
	TsPanel *sPanel41;
	TsRadioButton *sRadioButton12;
	TShape *Shape17;
	TsLabel *sLabel2;
	TsPanel *sPanel37;
	TsRadioButton *sRadioButton2;
	TsPanel *sPanel39;
	TsPanel *sPanel40;
	TsButton *sButton6;
	TsButton *sButton12;
	TiSevenSegmentInteger *iSevenSegmentInteger1;
	TsButton *btnAuto;
	TTimer *timeAuto;
	TTimer *timeZader;
	void __fastcall sListBox4Click(TObject *Sender);
	void __fastcall sListBox5Click(TObject *Sender);
	void __fastcall sListBox6Click(TObject *Sender);
	void __fastcall sListBox3Click(TObject *Sender);
	void __fastcall sListBox1Click(TObject *Sender);
	void __fastcall sListBox2Click(TObject *Sender);
	void __fastcall sListBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall sListBox4VScroll(TObject *Sender);
	void __fastcall sListBox5VScroll(TObject *Sender);

	void __fastcall sListBox3VScroll(TObject *Sender);
	void __fastcall sListBox1VScroll(TObject *Sender);
	void __fastcall sListBox2VScroll(TObject *Sender);
	void __fastcall sListBox6VScroll(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall sButton1Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall sButton2Click(TObject *Sender);
	void __fastcall sListBox9Click(TObject *Sender);
	void __fastcall sListBox7Click(TObject *Sender);
	void __fastcall sListBox8Click(TObject *Sender);
	void __fastcall sListBox9VScroll(TObject *Sender);
	void __fastcall sListBox7VScroll(TObject *Sender);
	void __fastcall sListBox8VScroll(TObject *Sender);
	void __fastcall sButton3Click(TObject *Sender);
	void __fastcall sButton7Click(TObject *Sender);
	void __fastcall sBitBtn1Click(TObject *Sender);
	void __fastcall sButton8Clic(TObject *Sender);
	void __fastcall sButton9Click(TObject *Sender);
	void __fastcall sButton10Click(TObject *Sender);
	void __fastcall sButton6Click(TObject *Sender);
	void __fastcall sButton12Click(TObject *Sender);
	void __fastcall sButton4Click(TObject *Sender);
	void __fastcall sButton5Click(TObject *Sender);
	void __fastcall btnAutoClick(TObject *Sender);
	void __fastcall timeAutoTimer(TObject *Sender);
	void __fastcall timeZaderTimer(TObject *Sender);

private:	// User declarations
	static const double SecPerDivTable[19];
	static const double VoltPerDivTable[22];

	WMk *wMk;
	int WorkMode;
	int scope;
//	TDateTime t_start;
//	TDateTime t_end;
	int counterK9;
	int counterK10;
	bool inclPower;
	int attempt;
	int time_step;
	bool inclMK1, inclMK2, inclMK3, inclMK4, inclMK5, inclMKR;

public:		// User declarations
	__fastcall TMK_2(TComponent* Owner, AnsiString number_block);

private:
	__fastcall void CreateTh();
//	__fastcall void CreateTh3();
	__fastcall void ResetAllUtil();
	__fastcall void ResetUtil0();
	__fastcall void ResetUtil1_2();
	__fastcall void ResetUtil3();
	__fastcall void ResetUtil4();
	__fastcall void ResetUtil5();
	__fastcall void ResetUtil6();
	__fastcall void Util0Stop();
	__fastcall void ScopeMK1_3_4(double amp);
	__fastcall void ScopeMK2();
	__fastcall void ScopeMK5();
	__fastcall void EnableButton(bool enab);
	__fastcall void Util1Stop();
	__fastcall void Util2Stop();
	__fastcall void Util3Stop();
	__fastcall void Util4Stop();
	__fastcall void Util5Stop();
	__fastcall void Util6Stop();
	__fastcall void CheckEndMK();

//	static DWORD WINAPI ScopeThread(TMK_2 *MK_2);
	static DWORD WINAPI ScopeThread2(TMK_2 *MK_2);
	static DWORD WINAPI ScopeThreadUtil4_5(TMK_2 *MK_2);
    static DWORD WINAPI ScopeThreadUtil6(TMK_2 *MK_2);
//	static DWORD WINAPI ScopeThread3(TMK_2 *MK_2);
	static DWORD WINAPI ThreadUtil1_2(TMK_2 *MK_2);
	static DWORD WINAPI ThreadUtil3(TMK_2 *MK_2);
};
//---------------------------------------------------------------------------
//extern PACKAGE TMK_2 *MK_2;
//---------------------------------------------------------------------------
#endif
