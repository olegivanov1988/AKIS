//---------------------------------------------------------------------------


#pragma hdrstop

#include "UnRaport.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
//--------------------------/ CONSTRUCTOR /----------------------------------
ZRaport::ZRaport()
{
//	RezultInit = false;
//	RezultCopySablon = false;
}
//--------------------------/ DESTRUCTOR /-----------------------------------
ZRaport::~ZRaport()
{
}
//------------------------/ MEMBER FUNCTION /--------------------------------
bool __fastcall ZRaport::ExcelInit(void)
{
   bool Rezult = false;

   try
   { // Попытка подключиться к открытому приложению Excel
	  XL = Variant::GetActiveObject("EXCEL.Application");
	  XLBooks = XL.OlePropertyGet("WorkBooks");
	  Rezult = true;
   }
   catch(...)
   { //Попытка не удалась
      try
      { //Попытка открыть приложение Excel
		 XL = Variant::CreateObject("Excel.Application");
		 XLBooks = XL.OlePropertyGet("WorkBooks");
		 Rezult = true;
	  }
	  catch (...)
	  { //Попытка не удалась
//		 MessageBox("Невозможно открыть Microsoft Excel!"
//			"Возможно, Excel не установлен на компьютере.",
//			"Ошибка",MB_OK+MB_ICONERROR);
	  }
   }

//   AnsiString lCurDir = GetCurrentDir();
//   AnsiString lcNme = "\\Raport.xls";
//   lCurDir=lCurDir.Trim()+lcNme.Trim();
//
//   XL.OlePropertyGet("WorkBooks").OleProcedure("Open",lCurDir.c_str());

   return Rezult;
}
//---------------------------------------------------------------------------
bool __fastcall ZRaport::CopyRepFromSablon(AnsiString WayFile)
{
   bool Rezult = false;
   AnsiString lCurDir = GetCurrentDir();
//   AnsiString lcNme = "\\Raport.xlsx";

   lCurDir = lCurDir.Trim() + WayFile.Trim();

   if(OpenSablon(lCurDir))
   {
	  Sablon = XLBooks.OlePropertyGet("Item", lgSab);
	  ShSab = Sablon.OlePropertyGet("Worksheets").OlePropertyGet("Item", 1);
	  ShSab.OlePropertyGet("Cells").OleProcedure("Select");
	  XL.OlePropertyGet("Selection").OleProcedure("Copy");
      XLBooks.OleProcedure("Add");
	  lgRp = XLBooks.OlePropertyGet("Count");
	  RpXL = XLBooks.OlePropertyGet("Item", lgRp);
	  ShRp = RpXL.OlePropertyGet("Worksheets").OlePropertyGet("Item", 1);
	  Range = ShRp.OlePropertyGet("Range", "A1");
	  ShRp.OlePropertyGet("Cells").OlePropertyGet("Item", 1, 1).OleProcedure("Select");
      ShRp.OleProcedure("Paste");
	  XL.OlePropertySet("DisplayAlerts", false);
      Sablon.OleProcedure("Close");
      Sablon.Clear();
      ShSab.Clear();
	  ShRp.OlePropertyGet("Cells").OlePropertyGet("Item", 2, 1).OleProcedure("Select");
      //Форматируем документ для печати (см. Примечание)
//      Variant PgSet=ShRp.OlePropertyGet("PageSetup");
//	  PgSet.OlePropertySet("Zoom", 95);
//	  PgSet.OlePropertySet("Orientation", 2);
//	  PgSet.OlePropertySet("CenterVertically", true);
//	  PgSet.OlePropertySet("LeftMargin", XL.OleFunction
//		("InchesToPoints",0.196850393700787));
//	  PgSet.OlePropertySet("RightMargin", XL.OleFunction
//		("InchesToPoints",0.196850393700787));
//	  PgSet.OlePropertySet("TopMargin", XL.OleFunction
//		("InchesToPoints",0.196850393700787));
//	  PgSet.OlePropertySet("BottomMargin", XL.OleFunction
//		("InchesToPoints",0.196850393700787));
//	  PgSet.OlePropertySet("HeaderMargin", XL.OleFunction
//		("InchesToPoints",0.196850393700787));
//	  PgSet.OlePropertySet("FooterMargin", XL.OleFunction
//		("InchesToPoints",0.196850393700787));
	  Rezult = true;
   }

   return Rezult;
}
//---------------------------------------------------------------------------
bool __fastcall ZRaport::OpenSablon(AnsiString AFile)
{
   bool Rezult = false;
   try
   { //Попытка открытия
	  Procedure Open("Load");
	  XLBooks.OleProcedure("Open", AFile.c_str());
//	  XLBooks.Exec(Open << AFile);
	  lgSab = XLBooks.OlePropertyGet("Count");
	  Rezult = true;
   }
   catch(...)
   { //Попытка не удалась
//	  Application->MessageBox("Ошибка открытия шаблона Excel!",
//		"Ошибка",MB_OK+MB_ICONERROR);
   }

   return Rezult;
}
//---------------------------------------------------------------------------
void __fastcall ZRaport::OpenReport()
{
   if(RezultInit)
   {
	  if(RezultCopySablon)
      {
//		 ZapDan();
		 XL.OlePropertySet("Visible",(Variant)true);
		 XL.OlePropertyGet("WorkBooks", 1).OleProcedure("SaveAs", "MyTest");    // Сохранение файла
		 XL.OlePropertyGet("WorkBooks", 1).OleProcedure("Close");
//       try{
//	   		XL.OlePropertyGet("WorkBooks",1).OleProcedure("Close");
//	   	 }catch(...){
//		  ShowMessage("Не забудьте сами закрыть Excel.");
//	  	 }
	  }
   }
   XL.Clear();
   XLBooks.Clear();
   RpXL.Clear();
   ShRp.Clear();
   Range.Clear();

//   Close();
}
//---------------------------------------------------------------------------
void __fastcall ZRaport::WriteData(int line, int column, double data)
{
//	AnsiString Model;
//	Model = "blablablabla";
//	ShRp.OlePropertyGet("Cells", 10, 5).OlePropertySet("Value", Model.c_str());

	ShRp.OlePropertyGet("Cells", line, column).OlePropertySet("Value", data);
}
//---------------------------------------------------------------------------
void __fastcall ZRaport::WriteData(int line, int column, AnsiString data)
{
	ShRp.OlePropertyGet("Cells", line, column).OlePropertySet("Value", data.c_str());
}
//---------------------------------------------------------------------------
void __fastcall ZRaport::CloseReport(AnsiString Dir)
{
	XL.OlePropertyGet("WorkBooks", 1).OleProcedure("SaveAs", Dir.c_str());    // Сохранение файла
	XL.OlePropertyGet("WorkBooks", 1).OleProcedure("Close");

	XL.Clear();
	XLBooks.Clear();
	RpXL.Clear();
	ShRp.Clear();
   	Range.Clear();
}
//---------------------------------------------------------------------------
