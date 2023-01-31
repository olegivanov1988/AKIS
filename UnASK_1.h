//---------------------------------------------------------------------------

#ifndef UnASK_1H
#define UnASK_1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "sButton.hpp"
#include "sGroupBox.hpp"
#include "sBitBtn.hpp"
#include <Buttons.hpp>
#include "sLabel.hpp"
#include "sPanel.hpp"
#include "sComboBoxes.hpp"
#include <ComCtrls.hpp>
#include "iComponent.hpp"
#include "iCustomComponent.hpp"
#include "iPlotComponent.hpp"
#include "iVCLComponent.hpp"
#include "iXYPlot.hpp"
#include "Chart.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include <Grids.hpp>
#include "sBevel.hpp"
#include "iLed.hpp"
#include "iLedRound.hpp"
#include "iScope.hpp"
#include "sRadioButton.hpp"
#include "UnClasses.h"
#include "sListBox.hpp"
#include "sUpDown.hpp"
#include "sSpeedButton.hpp"
#include "UnASK_2.h"
#include "sEdit.hpp"
//---------------------------------------------------------------------------
class WAsk;
#define MODE_NULL	0
#define MODE_UTIL1	1
#define MODE_UTIL2	2
#define MODE_UTIL3	3
#define MODE_UTIL4	4
#define COPLING_AC 0
#define COPLING_DC 1

class TASK_1 : public TForm
{
__published:	// IDE-managed Components
	TsBitBtn *sBitBtn1;
	TsPanel *sPanel2;
	TsPanel *sPanel3;
	TsButton *sButton1;
	TsButton *sButton2;
	TsLabel *sLabel3;
	TsPanel *sPanel4;
	TsPanel *sPanel1;
	TsPanel *sPanel8;
	TsPanel *sPanel13;
	TsPanel *sPanel16;
	TsPanel *sPanel17;
	TsPanel *sPanel14;
	TsPanel *sPanel15;
	TShape *Shape1;
	TShape *Shape3;
	TsPanel *sPanel19;
	TsRadioButton *sRadioButton2;
	TsLabel *sLabel1;
	TShape *Shape5;
	TsPanel *sPanel20;
	TsRadioButton *sRadioButton1;
	TsLabel *sLabel2;
	TsPanel *sPanel11;
	TsPanel *sPanel12;
	TShape *Shape2;
	TShape *Shape4;
	TShape *Shape6;
	TShape *Shape7;
	TShape *Shape8;
	TShape *Shape9;
	TShape *Shape10;
	TShape *Shape11;
	TsPanel *sPanel21;
	TsPanel *sPanel22;
	TShape *Shape12;
	TsPanel *sPanel6;
	TsRadioButton *sRadioButton3;
	TsLabel *sLabel4;
	TShape *Shape13;
	TsPanel *sPanel7;
	TsRadioButton *sRadioButton5;
	TsLabel *sLabel5;
	TsPanel *sPanel23;
	TsLabel *sLabel6;
	TsButton *sButton4;
	TsPanel *sPanel5;
	TsPanel *sPanel9;
	TiScope *iScope1;
	TShape *Shape14;
	TsPanel *sPanel10;
	TsRadioButton *sRadioButton4;
	TsLabel *sLabel7;
	TShape *Shape15;
	TShape *Shape16;
	TsPanel *sPanel24;
	TsLabel *sLabUpr1;
	TsButton *sButton3;
	TsPanel *sPanel25;
	TsLabel *sLabUpr2;
	TsPanel *sPanel26;
	TsLabel *sLabUpr3;
	TShape *Shape17;
	TsPanel *sPanel27;
	TsRadioButton *sRadioButton7;
	TsLabel *sLabel11;
	TsPanel *sPanel28;
	TsRadioButton *sRadioButton8;
	TsLabel *sLabel12;
	TShape *Shape19;
	TsPanel *sPanel29;
	TsRadioButton *sRadioButton9;
	TsLabel *sLabel13;
	TGridPanel *GridPanel1;
	TsPanel *sPanel30;
	TsPanel *sPanel31;
	TsPanel *Table_1_0;
	TsPanel *Table_1_1;
	TsPanel *sPanel34;
	TsPanel *Table_1_2;
	TsPanel *sPanel36;
	TsPanel *Table_1_3;
	TsPanel *Table_2_0;
	TsPanel *Table_2_1;
	TsPanel *Table_2_2;
	TsPanel *Table_2_3;
	TsPanel *Table_3_0;
	TsPanel *Table_3_1;
	TsPanel *Table_3_2;
	TsPanel *Table_3_3;
	TsPanel *Table_4_0;
	TsPanel *Table_4_1;
	TsPanel *Table_4_2;
	TsPanel *Table_4_3;
	TsPanel *Table_5_0;
	TsPanel *Table_5_1;
	TsPanel *Table_5_2;
	TsPanel *Table_5_3;
	TsPanel *Table_6_0;
	TsPanel *Table_6_1;
	TsPanel *Table_6_2;
	TsPanel *Table_6_3;
	TsPanel *Table_7_0;
	TsPanel *Table_7_1;
	TsPanel *Table_7_2;
	TsPanel *Table_7_3;
	TsPanel *Table_8_0;
	TsPanel *Table_8_1;
	TsPanel *Table_8_2;
	TsPanel *Table_8_3;
	TTimer *Timer1;
	TiScope *iScope2;
	TsPanel *sPanel32;
	TsListBox *sListBox5;
	TsListBox *sListBox6;
	TsPanel *sPanel33;
	TsListBox *sListBox1;
	TsListBox *sListBox2;
	TsListBox *sListBox3;
	TsButton *sButton5;
	TsListBox *sListBox4;
	TsButton *sButton6;
	TsButton *sButton7;
	TsLabel *sLabel8;
	TsEdit *sEdit1;
	TsLabel *sLabel9;
	TsLabel *sLabel10;
	TsButton *sButton8;
	TsButton *sButton9;
	TsButton *sButton10;
	TsButton *sButton11;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall sButton3Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall sListBox1Click(TObject *Sender);
	void __fastcall sListBox2Click(TObject *Sender);
	void __fastcall sListBox3Click(TObject *Sender);
	void __fastcall sListBox4Click(TObject *Sender);
	void __fastcall sListBox5Click(TObject *Sender);
	void __fastcall sListBox6Click(TObject *Sender);
	void __fastcall sListBox1VScroll(TObject *Sender);
	void __fastcall sListBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall sListBox2VScroll(TObject *Sender);
	void __fastcall sListBox3VScroll(TObject *Sender);
	void __fastcall sListBox4VScroll(TObject *Sender);
	void __fastcall sListBox5VScroll(TObject *Sender);
	void __fastcall sListBox6VScroll(TObject *Sender);
	void __fastcall sButton5Click(TObject *Sender);
	void __fastcall sButton4Click(TObject *Sender);
	void __fastcall sBitBtn1Click(TObject *Sender);
	void __fastcall sButton1Click(TObject *Sender);
	void __fastcall sButton6Click(TObject *Sender);
	void __fastcall sButton2Click(TObject *Sender);
	void __fastcall sButton7Click(TObject *Sender);
	void __fastcall sButton8Click(TObject *Sender);
	void __fastcall sButton9Click(TObject *Sender);
	void __fastcall sButton10Click(TObject *Sender);
	void __fastcall sButton11Click(TObject *Sender);

private:	// User declarations
//	UprClass *Upr;
	bool incl;
	bool inclK10;
	bool inclK9;
    bool inclK8K7;
	WAsk *wAsk;
	int WorkMode;
	int WorkModeEND;
	int scope;
	TASK_2 *ASK_2;
	bool power;

	static const double SecPerDivTable[19];
	static const double VoltPerDivTable[22];
public:		// User declarations
	__fastcall TASK_1(TComponent* Owner, AnsiString number_block);
	int Run; // имитация
private:
	__fastcall void Util1Stop();
	__fastcall void Util2Stop();
	__fastcall void Util3Stop();
	__fastcall void Util4Stop();
	__fastcall void LighteTable(int Step);
	__fastcall void ErrorLighteTable(int Step, TColor cl);
	__fastcall void ResetLighteTable();
	__fastcall void ResetK10();
    __fastcall void ResetK7K8();
    __fastcall void CheckK10();
	__fastcall void CreateTh();
	__fastcall void TransitionASK2();

	static DWORD WINAPI ScopeThread(TASK_1 *ASK_1);
};
//---------------------------------------------------------------------------
//extern PACKAGE TASK_1 *ASK_1;
//---------------------------------------------------------------------------
#endif
