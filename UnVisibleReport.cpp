//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnVisibleReport.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sDialogs"
#pragma link "sButton"
#pragma link "sPageControl"
#pragma link "sSkinManager"
#pragma resource "*.dfm"
//TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TVsReport::TVsReport(TComponent* Owner, AnsiString number_block)
	: TForm(Owner)
{
//   AnsiString lCurDir = GetCurrentDir();
//   AnsiString lcNme = "\\RaportASK.xlsx";
//   lCurDir = lCurDir.Trim()+lcNme.Trim();

	OleContainer1->CreateObjectFromFile(GetCurrentDir() + "\\~" + number_block + ".xlsx", false); // Делаем OLE объект из файла
//	OleContainer1->CreateLinkToFile(lCurDir, false);
//	XL = OleContainer1->OleObject;
//	XL.OlePropertyGet("WorkBooks",1).OleProcedure("SaveAs","MyTest");    // Сохранение файла

//	XLBooks = XL.OlePropertyGet("Worksheets");
//	CopyRepFromSablon();

	OleContainer1->DoVerb(ovShow);             // Выводим на экран OLE объект

}
//---------------------------------------------------------------------------



