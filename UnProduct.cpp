//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnProduct.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "iScope"
#pragma link "sLabel"
#pragma link "sListBox"
#pragma link "sPanel"
#pragma link "sButton"
#pragma link "sBevel"
#pragma resource "*.dfm"
//TProduct *Product;
//---------------------------------------------------------------------------
__fastcall TProduct::TProduct(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TProduct::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------------
