//---------------------------------------------------------------------------

#ifndef UnASK_2H
#define UnASK_2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "iComponent.hpp"
#include "iCustomComponent.hpp"
#include "iPlotComponent.hpp"
#include "iVCLComponent.hpp"
#include "iXYPlot.hpp"
#include "sLabel.hpp"
#include "sBevel.hpp"
#include "sButton.hpp"
#include <ExtCtrls.hpp>
#include "sPanel.hpp"
#include "sRadioButton.hpp"
#include "iScope.hpp"
#include "sListBox.hpp"
#include "sDialogs.hpp"
#include <Dialogs.hpp>
#include "acPathDialog.hpp"
//#include "UnASK_1.h"

//#define COPLING_AC 0
//#define COPLING_DC 1

#define MODE_NULL			0
#define MODE_UTIL5			5
#define MODE_UTIL6			6
#define MODE_UTIL6_STEP2	7
#define MODE_UTIL4_STEP1	4

//---------------------------------------------------------------------------
class WAsk;
//class TVsReport;

//---------------------------------------------------------------------------
class TASK_2 : public TForm
{
__published:	// IDE-managed Components
	TsButton *sButton1;
	TsBevel *sBevel1;
	TsButton *sButton2;
	TsButton *sButton3;
	TsPanel *sPanel2;
	TsButton *sButton4;
	TsButton *sButton5;
	TsPanel *sPanel3;
	TsButton *sButton6;
	TsPanel *sPanel1;
	TsPanel *sPanel4;
	TsPanel *sPanel5;
	TsPanel *sPanel6;
	TsPanel *sPanel7;
	TsPanel *sPanel8;
	TsPanel *sPanel16;
	TsPanel *sPanel14;
	TsPanel *sPanel21;
	TsPanel *sPanel22;
	TsPanel *sPanel9;
	TsPanel *sPanel10;
	TsPanel *sPanel11;
	TsPanel *sPanel12;
	TsPanel *sPanel13;
	TsPanel *sPanel15;
	TShape *Shape12;
	TsLabel *sLabel4;
	TsPanel *sPanel17;
	TsRadioButton *sRadioButton3;
	TShape *Shape1;
	TsLabel *sLabel1;
	TsPanel *sPanel18;
	TsRadioButton *sRadioButton1;
	TShape *Shape2;
	TsLabel *sLabel2;
	TsPanel *sPanel19;
	TsRadioButton *sRadioButton2;
	TShape *Shape3;
	TsLabel *sLabel3;
	TsPanel *sPanel20;
	TsRadioButton *sRadioButton4;
	TShape *Shape4;
	TShape *Shape5;
	TShape *Shape6;
	TsPanel *sPanel23;
	TsRadioButton *sRadioButton5;
	TsLabel *sLabel5;
	TsPanel *sPanel24;
	TShape *Shape9;
	TsLabel *sLabel6;
	TShape *Shape10;
	TShape *Shape11;
	TShape *Shape13;
	TiScope *iScope2;
	TiScope *iScope1;
	TiScope *iScope3;
	TsPanel *sPanel33;
	TsListBox *sListBox1;
	TsListBox *sListBox2;
	TsListBox *sListBox3;
	TsPanel *sPanel27;
	TsListBox *sListBox4;
	TsListBox *sListBox5;
	TsListBox *sListBox6;
	TShape *Shape7;
	TsPanel *sPanel28;
	TsRadioButton *sRadioButton8;
	TsPanel *sPanel30;
	TsListBox *sListBox7;
	TsListBox *sListBox8;
	TsListBox *sListBox9;
	TsLabel *sLabel8;
	TShape *Shape8;
	TsPanel *sPanel29;
	TsRadioButton *sRadioButton9;
	TTimer *Timer1;
	TsButton *sButton7;
	TsButton *sButton8;
	TsLabel *sLabel9;
	TsPanel *sPanel25;
	TsPanel *sPanel26;
	TsPanel *sPanel31;
	TsPanel *sPanel32;
	TsPanel *sPanel34;
	TsPanel *sPanel35;
	TsLabel *sLabel10;
	TsButton *btnAuto;
	TTimer *timeAuto;
	TTimer *timeZader;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall sListBox1Click(TObject *Sender);
	void __fastcall sListBox2Click(TObject *Sender);
	void __fastcall sListBox3Click(TObject *Sender);
	void __fastcall sListBox4Click(TObject *Sender);
	void __fastcall sListBox5Click(TObject *Sender);
	void __fastcall sListBox6Click(TObject *Sender);
	void __fastcall sListBox7Click(TObject *Sender);
	void __fastcall sListBox8Click(TObject *Sender);
	void __fastcall sListBox9Click(TObject *Sender);
	void __fastcall sListBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall sListBox1VScroll(TObject *Sender);
	void __fastcall sListBox4VScroll(TObject *Sender);
	void __fastcall sListBox7VScroll(TObject *Sender);
	void __fastcall sListBox2VScroll(TObject *Sender);
	void __fastcall sListBox5VScroll(TObject *Sender);
	void __fastcall sListBox8VScroll(TObject *Sender);
	void __fastcall sListBox3VScroll(TObject *Sender);
	void __fastcall sListBox6VScroll(TObject *Sender);
	void __fastcall sListBox9VScroll(TObject *Sender);
	void __fastcall sButton1Click(TObject *Sender);
	void __fastcall sButton4Click(TObject *Sender);
	void __fastcall sButton2Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall sButton3Click(TObject *Sender);
	void __fastcall sButton5Click(TObject *Sender);
	void __fastcall sButton6Click(TObject *Sender);
	void __fastcall sButton7Click(TObject *Sender);
	void __fastcall sButton8Click(TObject *Sender);
	void __fastcall btnAutoClick(TObject *Sender);
	void __fastcall timeZaderTimer(TObject *Sender);
	void __fastcall timeAutoTimer(TObject *Sender);
private:	// User declarations
	static const double SecPerDivTable[19];
	static const double VoltPerDivTable[22];

	WAsk *wAsk;
	int WorkMode;
//	int WorkModeEND;
	int scope;
	bool inclCK1;
	bool inclCK2;
	bool inclCK3;
	double maxValueUtil2;
	bool power;
	int stepUtil6;
	int countPorog_5XP1, countPorog_7XP1;
//	bool Uroven1_5XP1, Uroven1_7XP1;
	int count_sample_5XP1_min;
	int count_sample_5XP1_max;
	int count_sample_7XP1_min;
	int count_sample_7XP1_max;
//	TASK_1 *ASK_1;
//	TVsReport *vsReport;

//	HANDLE hnd_scope_thread2;
private:
	__fastcall void CreateTh();
	__fastcall void Util4Stop();
	__fastcall void Util5Stop();
	__fastcall void Util6Stop();
	__fastcall void ResetK1();
	__fastcall void ResetK2K3K4();
    __fastcall void ResetCK3();
	__fastcall void CheckEndASK2();

	static DWORD WINAPI ScopeThread(TASK_2 *ASK_2);

public:		// User declarations
	__fastcall TASK_2(TComponent* Owner, WAsk *wask);
	__fastcall ~TASK_2();
};
//---------------------------------------------------------------------------
//extern PACKAGE TASK_2 *ASK_2;
//---------------------------------------------------------------------------
#endif
