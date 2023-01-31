//---------------------------------------------------------------------------

#ifndef UnRaportH
#define UnRaportH

#include <Classes.hpp>

class ZRaport
{
public:
	ZRaport();
	~ZRaport();
	void __fastcall WriteData(int line, int column, double data);
    void __fastcall WriteData(int line, int column, AnsiString data);
	bool __fastcall ExcelInit();
	bool __fastcall CopyRepFromSablon(AnsiString WayFile);
	void __fastcall CloseReport(AnsiString Dir);

private:
	Variant XL, XLBooks, Sablon, RpXL, ShSab, ShRp, Range;
	long lgSab, lgRp;
	bool RezultInit;
	bool RezultCopySablon;

protected:

private:
//	bool __fastcall ExcelInit();
//	bool __fastcall CopyRepFromSablon(AnsiString WayFile);
	bool __fastcall OpenSablon(AnsiString AFile);
	void __fastcall OpenReport();

};
//---------------------------------------------------------------------------
#endif
