//---------------------------------------------------------------------------

#ifndef UnLSK_2H
#define UnLSK_2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "iScope.hpp"
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sLabel.hpp"
#include "sListBox.hpp"
#include "sPanel.hpp"
#include "sRadioButton.hpp"
#include <ExtCtrls.hpp>

class WLsk;
#define MODE_NULL			0
#define MODE_UTIL1_STEP1	1
#define MODE_UTIL1_STEP2	2
#define MODE_UTIL1_STEP3	3
#define MODE_UTIL2_STEP1	4
#define MODE_UTIL2_STEP2	5
#define MODE_UTIL2_STEP3	6
#define MODE_UTIL3_STEP1	7
#define MODE_UTIL3_STEP2	8
#define MODE_UTIL3_STEP3	9
#define MODE_UTIL4_STEP1	10
#define MODE_UTIL4_STEP2	11
#define MODE_UTIL4_STEP3	12
#define MODE_UTIL5_STEP1	13
#define MODE_UTIL5_STEP2	14


#define COPLING_AC 0
#define COPLING_DC 1
//---------------------------------------------------------------------------
class TLSK_2 : public TForm
{
__published:	// IDE-managed Components
	TiScope *iScope1;
	TsPanel *sPanel32;
	TsListBox *sListBox2;
	TsListBox *sListBox3;
	TsListBox *sListBox1;
	TsBevel *sBevel1;
	TsButton *sButton1;
	TsButton *sButton2;
	TsButton *sButton3;
	TsButton *sButton4;
	TsPanel *sPanel2;
	TsButton *sButton5;
	TsButton *sButton6;
	TsButton *sButton7;
	TsPanel *sPanel3;
	TShape *Shape11;
	TShape *Shape7;
	TShape *Shape3;
	TsLabel *sLabel1;
	TShape *Shape5;
	TsLabel *sLabel2;
	TsPanel *sPanel13;
	TsPanel *sPanel19;
	TsRadioButton *sRadioButton2;
	TsPanel *sPanel20;
	TsRadioButton *sRadioButton1;
	TShape *Shape1;
	TsLabel *sLabel3;
	TsPanel *sPanel1;
	TsRadioButton *sRadioButton3;
	TShape *Shape12;
	TsLabel *sLabel4;
	TsPanel *sPanel17;
	TsRadioButton *sRadioButton4;
	TsPanel *sPanel21;
	TsPanel *sPanel22;
	TsPanel *sPanel4;
	TsPanel *sPanel5;
	TsPanel *sPanel6;
	TsPanel *sPanel7;
	TsPanel *sPanel24;
	TsLabel *sLabel21;
	TsButton *sButton8;
	TsPanel *sPanel8;
	TsPanel *sPanel9;
	TsPanel *sPanel10;
	TShape *Shape2;
	TsLabel *sLabel5;
	TsPanel *sPanel11;
	TsRadioButton *sRadioButton5;
	TShape *Shape8;
	TsLabel *sLabel8;
	TsPanel *sPanel15;
	TsRadioButton *sRadioButton8;
	TsPanel *sPanel16;
	TsPanel *sPanel18;
	TsPanel *sPanel23;
	TShape *Shape9;
	TShape *Shape15;
	TShape *Shape10;
	TShape *Shape13;
	TShape *Shape16;
	TShape *Shape17;
	TShape *Shape18;
	TShape *Shape19;
	TShape *Shape20;
	TShape *Shape21;
	TsPanel *sPanel26;
	TShape *Shape22;
	TsLabel *sLabel10;
	TsPanel *sPanel27;
	TsRadioButton *sRadioButton10;
	TShape *Shape23;
	TsLabel *sLabel11;
	TsPanel *sPanel28;
	TsRadioButton *sRadioButton11;
	TShape *Shape24;
	TsPanel *sPanel29;
	TsPanel *sPanel30;
	TsLabel *sLabel12;
	TsButton *sButton9;
	TsPanel *sPanel31;
	TShape *Shape25;
	TsLabel *sLabel13;
	TsPanel *sPanel33;
	TsRadioButton *sRadioButton12;
	TsPanel *sPanel34;
	TShape *Shape26;
	TsLabel *sLabel14;
	TsPanel *sPanel35;
	TsRadioButton *sRadioButton13;
	TShape *Shape27;
	TsLabel *sLabel15;
	TsPanel *sPanel36;
	TsRadioButton *sRadioButton14;
	TsPanel *sPanel37;
	TsLabel *sLabel16;
	TsButton *sButton10;
	TsPanel *sPanel38;
	TsLabel *sLabel17;
	TsButton *sButton11;
	TShape *Shape28;
	TShape *Shape29;
	TShape *Shape4;
	TsLabel *sLabel6;
	TsPanel *sPanel12;
	TsRadioButton *sRadioButton6;
	TShape *Shape6;
	TsLabel *sLabel7;
	TsPanel *sPanel14;
	TsRadioButton *sRadioButton7;
	TShape *Shape14;
	TsLabel *sLabel9;
	TsPanel *sPanel25;
	TsRadioButton *sRadioButton9;
	TTimer *Timer1;
	TsLabel *sLabel18;
	TsLabel *sLabel19;
	TsLabel *sLabel20;
	TsButton *sButton12;
	TsLabel *sLabel22;
	TsLabel *sLabel23;
	TsButton *sButton13;
	TsButton *sButton14;
	TTimer *timeAuto;
	TTimer *timeZader;
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall sButton1Click(TObject *Sender);
	void __fastcall sListBox1Click(TObject *Sender);
	void __fastcall sListBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall sListBox2Click(TObject *Sender);
	void __fastcall sListBox3Click(TObject *Sender);
	void __fastcall sListBox1VScroll(TObject *Sender);
	void __fastcall sListBox2VScroll(TObject *Sender);
	void __fastcall sListBox3VScroll(TObject *Sender);
	void __fastcall sButton2Click(TObject *Sender);
	void __fastcall sButton3Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall sButton4Click(TObject *Sender);
	void __fastcall sButton12Click(TObject *Sender);
	void __fastcall sButton6Click(TObject *Sender);
	void __fastcall sButton5Click(TObject *Sender);
	void __fastcall sButton7Click(TObject *Sender);
	void __fastcall sButton13Click(TObject *Sender);
	void __fastcall sButton14Click(TObject *Sender);
	void __fastcall timeAutoTimer(TObject *Sender);
	void __fastcall timeZaderTimer(TObject *Sender);

private:	// User declarations
	static const double SecPerDivTable[19];
	static const double VoltPerDivTable[22];

	WLsk *wLsk;
	int WorkMode;
	double a[1000];
	bool power;
	bool inclLS1, inclLS2, inclLS3, inclLS4, inclLS5;

	HANDLE hnd_scope_thread2;
public:		// User declarations
	__fastcall TLSK_2(TComponent* Owner, WLsk *wlsk);

private:
	__fastcall void PaintScopeUtil1();
	__fastcall void PaintScopeUtil2();
    __fastcall void PaintScopeUtil3();
	__fastcall void PaintScopeUtil4();
	__fastcall void PaintScopeUtil5();
//	__fastcall void MassiveScope();
//	__fastcall void DisplayResultUtil1Step2();
	__fastcall void DisplayResultUtil1_5Step2(bool *PerepadUtil, int numberUtil);
	__fastcall void DisplayResultUtil1_4Step3(double *valueK1_K3, double Min, double Max, int numberUtil);
	__fastcall void ResetAllUtill();
//	__fastcall void EnableButton(bool enable);
	__fastcall void ExposeVh(double Vh3, double Vh4, double Vh5, double VhUl);
	__fastcall void CheckEndLSK2();
	__fastcall void Util1Stop();
	__fastcall void Util2Stop();
	__fastcall void Util3Stop();
	__fastcall void Util4Stop();
	__fastcall void Util5Stop();

};
//---------------------------------------------------------------------------
//extern PACKAGE TLSK_2 *LSK_2;
//---------------------------------------------------------------------------
#endif
