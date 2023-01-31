//---------------------------------------------------------------------------

#ifndef UnProductH
#define UnProductH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "iScope.hpp"
#include "sLabel.hpp"
#include "sListBox.hpp"
#include "sPanel.hpp"
#include <ExtCtrls.hpp>
#include "sButton.hpp"
#include "sBevel.hpp"
//---------------------------------------------------------------------------
class TProduct : public TForm
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
	TiScope *iScope3;
	TsPanel *sPanel2;
	TsListBox *sListBox7;
	TsListBox *sListBox8;
	TsListBox *sListBox9;
	TsPanel *sPanel8;
	TShape *Shape5;
	TsPanel *sPanel3;
	TsButton *sButton5;
	TsButton *sButton6;
	TsPanel *sPanel4;
	TsPanel *sPanel5;
	TsButton *sButton2;
	TsBevel *sBevel1;
	TsButton *sButton1;
	TsButton *sButton3;
	TsPanel *sPanel21;
	TsPanel *sPanel22;
	TsPanel *sPanel6;
	TsPanel *sPanel7;
	TsPanel *sPanel9;
	TsPanel *sPanel10;
	TsPanel *sPanel11;
	TsPanel *sPanel12;
	TsPanel *sPanel13;
	TsPanel *sPanel14;
	TsPanel *sPanel15;
	TsPanel *sPanel16;
	TsPanel *sPanel17;
	TsPanel *sPanel18;
	TsPanel *sPanel19;
	TsPanel *sPanel20;
	TShape *Shape1;
	TShape *Shape10;
	TShape *Shape3;
	TShape *Shape4;
	TShape *Shape6;
	TShape *Shape7;
	TShape *Shape8;
	TShape *Shape9;
	TShape *Shape11;
	TShape *Shape12;
	TShape *Shape13;
	TShape *Shape14;
	TShape *Shape15;
	TShape *Shape16;
	TShape *Shape17;
	TsLabel *sLabel15;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
	__fastcall TProduct(TComponent* Owner);
};
//---------------------------------------------------------------------------
//extern PACKAGE TProduct *Product;
//---------------------------------------------------------------------------
#endif
