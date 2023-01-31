//---------------------------------------------------------------------------

#ifndef UnMK_1H
#define UnMK_1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "sBitBtn.hpp"
#include "sLabel.hpp"
#include "sPanel.hpp"
#include "sRadioButton.hpp"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "iScope.hpp"
#include "sButton.hpp"
#include "sListBox.hpp"
#include "sBevel.hpp"
#include "sEdit.hpp"
class WMk;

#define MODE_NULL	0
#define MODE_UTIL1_STEP1  	1
#define MODE_UTIL2_STEP1	2
#define MODE_UTIL3_STEP1	3
//---------------------------------------------------------------------------
class TMK_1 : public TForm
{
__published:	// IDE-managed Components
	TsBitBtn *sBitBtn1;
	TsPanel *sPanel8;
	TsPanel *sPanel11;
	TsPanel *sPanel12;
	TsPanel *sPanel1;
	TsPanel *sPanel2;
	TsPanel *sPanel14;
	TsPanel *sPanel15;
	TsPanel *sPanel3;
	TsPanel *sPanel4;
	TShape *Shape17;
	TsLabel *sLabel11;
	TsPanel *sPanel27;
	TsRadioButton *sRadioButton7;
	TShape *Shape5;
	TsLabel *sLabel2;
	TsPanel *sPanel6;
	TsRadioButton *sRadioButton2;
	TShape *Shape6;
	TiScope *iScope2;
	TsPanel *sPanel33;
	TsListBox *sListBox1;
	TsListBox *sListBox2;
	TsListBox *sListBox3;
	TiScope *iScope1;
	TsPanel *sPanel9;
	TsListBox *sListBox4;
	TsListBox *sListBox5;
	TsListBox *sListBox6;
	TsPanel *sPanel10;
	TsLabel *sLabel4;
	TsButton *sButton1;
	TsButton *sButton2;
	TsPanel *sPanel13;
	TShape *Shape4;
	TsLabel *sLabel5;
	TsPanel *sPanel20;
	TsRadioButton *sRadioButton4;
	TShape *Shape16;
	TsPanel *sPanel19;
	TShape *Shape12;
	TsLabel *sLabel7;
	TsPanel *sPanel23;
	TsRadioButton *sRadioButton6;
	TsPanel *sPanel24;
	TsBevel *sBevel1;
	TsButton *sButton3;
	TsButton *sButton4;
	TShape *Shape2;
	TsLabel *sLabel3;
	TShape *Shape3;
	TsPanel *sPanel7;
	TsRadioButton *sRadioButton3;
	TShape *Shape1;
	TsLabel *sLabel1;
	TsPanel *sPanel5;
	TsRadioButton *sRadioButton1;
	TShape *Shape9;
	TsLabel *sLabel8;
	TShape *Shape11;
	TiScope *iScope3;
	TsPanel *sPanel25;
	TsListBox *sListBox7;
	TsListBox *sListBox8;
	TsListBox *sListBox9;
	TiScope *iScope4;
	TsPanel *sPanel26;
	TsListBox *sListBox10;
	TsListBox *sListBox11;
	TsListBox *sListBox12;
	TsPanel *sPanel29;
	TsRadioButton *sRadioButton8;
	TsPanel *sPanel35;
	TShape *Shape13;
	TsLabel *sLabel10;
	TsPanel *sPanel36;
	TsRadioButton *sRadioButton10;
	TsPanel *sPanel37;
	TTimer *Timer1;
	TsPanel *sPanel21;
	TsPanel *sPanel30;
	TsPanel *sPanel38;
	TsPanel *sPanel39;
	TsLabel *sLabel9;
	TsLabel *sLabel6;
	TsButton *sButton7;
	TsEdit *sEdit1;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall sListBox1Click(TObject *Sender);
	void __fastcall sListBox2Click(TObject *Sender);
	void __fastcall sListBox3Click(TObject *Sender);
	void __fastcall sListBox7Click(TObject *Sender);
	void __fastcall sListBox8Click(TObject *Sender);
	void __fastcall sListBox9Click(TObject *Sender);
	void __fastcall sListBox4Click(TObject *Sender);
	void __fastcall sListBox5Click(TObject *Sender);
	void __fastcall sListBox6Click(TObject *Sender);
	void __fastcall sListBox10Click(TObject *Sender);
	void __fastcall sListBox11Click(TObject *Sender);
	void __fastcall sListBox12Click(TObject *Sender);
	void __fastcall sListBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall sListBox1VScroll(TObject *Sender);
	void __fastcall sListBox4VScroll(TObject *Sender);
	void __fastcall sListBox7VScroll(TObject *Sender);
	void __fastcall sListBox10VScroll(TObject *Sender);
	void __fastcall sListBox2VScroll(TObject *Sender);
	void __fastcall sListBox5VScroll(TObject *Sender);
	void __fastcall sListBox8VScroll(TObject *Sender);
	void __fastcall sListBox11VScroll(TObject *Sender);
	void __fastcall sListBox3VScroll(TObject *Sender);
	void __fastcall sListBox6VScroll(TObject *Sender);
	void __fastcall sListBox9VScroll(TObject *Sender);
	void __fastcall sListBox12VScroll(TObject *Sender);
	void __fastcall sButton3Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall sButton4Click(TObject *Sender);
	void __fastcall sBitBtn1Click(TObject *Sender);
	void __fastcall sButton7Click(TObject *Sender);
private:	// User declarations
	static const double SecPerDivTable[19];
	static const double VoltPerDivTable[22];

	WMk *wMk;
	int WorkMode;
	bool inclPower;

public:		// User declarations
	__fastcall TMK_1(TComponent* Owner);

private:
	__fastcall void CreateTh();
	__fastcall void Util1Stop();
    __fastcall void ResetUtil2_3();

	static DWORD WINAPI ScopeThread(TMK_1 *MK_1);
};
//---------------------------------------------------------------------------
//extern PACKAGE TMK_1 *MK_1;
//---------------------------------------------------------------------------
#endif
