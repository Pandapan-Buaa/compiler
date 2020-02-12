#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "type.h"



class SymbolTable_Analysis
{
public:
	STE* ste;
	int p;
	static SymbolTable_Analysis& initSymbolTable();
	void goIn(const string&);
	void goOut();
	bool isHere(const string&);
	bool find(const string&);
	void insert();
	void defineArguments(STE* te, vector<Argument>& args);
	int addString(const string& str);
	vector<string> getStringMap();
	string& encodeKey();
	string& encodePosition();
	void setTempcount(int count);
	char* pointToStr(const void* point);
private:
	STE* position;
	friend class Sementic_Analysis;
	friend class Grammar_analysis;
	SymbolTable_Analysis();
	SymbolTable_Analysis(const SymbolTable_Analysis&);
	SymbolTable_Analysis& operator=(const SymbolTable_Analysis&);

};
#endif
