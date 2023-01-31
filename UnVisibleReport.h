//---------------------------------------------------------------------------

#ifndef UnVisibleReportH
#define UnVisibleReportH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <OleCtnrs.hpp>
#include "sDialogs.hpp"
#include <Dialogs.hpp>
#include "sButton.hpp"
#include <Menus.hpp>
#include "sPageControl.hpp"
#include <ComCtrls.hpp>
#include "sSkinManager.hpp"
//---------------------------------------------------------------------------
class TVsReport : public TForm
{
__published:	// IDE-managed Components
	TOleContainer *OleContainer1;
private:	// User declarations
public:		// User declarations
	__fastcall TVsReport(TComponent* Owner, AnsiString number_block);
};
//---------------------------------------------------------------------------
//extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------
#endif
