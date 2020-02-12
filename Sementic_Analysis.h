#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H
#define OUTST
#include "ErrorHandler.h"
#include "SymbolTable_Analysis.h"
#include "Lexical_analysis.h"
#include "Quaternions.h"
#include "Code_Generator.h"
class Sementic_Analysis
{
public:
	static Sementic_Analysis& initSementic(ErrorHandler&, SymbolTable_Analysis&, Lexical_analysis&, Quaternions&, ofstream&, Code_Generator&);
	//Endmap初始化与清除

	//
	void constDefine(const string&, BasicType, int value);
	void varDefine(const string&, BasicType);
	void arrayDefine(const string&, BasicType, int limit);
	void functionDefine(const string& funcname, vector<Argument>& args, BasicType type);
	//
	void intToString(int n, string& str);
	void charToString(char n, string& str);
	string stringPlusInt(const string& str, int n);
	//
	void negative(const string& oper1, string& dest, STE* ste);
	void addition(const string& oper1, const string& oper2, string& dest, STE* ste);
	void subtraction(const string& oper1, const string& oper2, string& dest, STE* ste);
	void multiplicatioin(const string& oper1, const string& oper2, string& dest, STE* ste);
	void division(const string& oper1, const string& oper2, string& dest, STE* ste);
	void callfunc(const string& name, string& value, STE* ste);
	void callfuncreturn(string& value, STE* ste);
	void funcreturnvalue(string& value, STE* ste);
	void assignfromArray(const string& arrayname, const string& offset, string& value, STE* ste);
	void assignToArray(const string& arrname, const string& offset, const string& value, STE* ste);
	void push(const string& value, STE* ste);
	void passParameter(const string& value, STE* ste,string paraname,string funcname);
	void assign(const string& name, const string& value, STE* ste);
	void compare(const string& value1, const string& value2, STE* ste);
	string& stringDefine(const string& str, STE* ste);


	void readInteger(const string& name, STE* ste);
	void readChar(const string& name, STE* ste);
	void writeString(const string& name, STE* ste);
	void writeInteger(const string& name, STE* ste);
	void writeChar(const string& name, STE* ste);
	void newLine();
	void outmidcode();

	void label(const string& lab);
	string& newLabel();
	string& newLabel(const string& forehead);
	void dissatisfyJump(SymbolType op, const string& lab, STE* ste);
	void satisfyJump(SymbolType op, const string& lab, STE* ste);
	void jumpAnyway(string lab, STE* ste);

	int getTemporaryCount() { return temporaryCount; }
	void funcStart();
	void funcReturn();


	string newvar();
	string newlabel();
private:
	ErrorHandler& errorHandler;
	SymbolTable_Analysis& table;
	Lexical_analysis& lexical_analysis;
	Quaternions& quaternions;
	Code_Generator& generator;
	int temporaryCount;
	int labelCount;
	int varcnt;
	int labelcnt;
	string newTemporary();
	ofstream& midcode;

	Sementic_Analysis(ErrorHandler&, SymbolTable_Analysis&, Lexical_analysis&, Quaternions&, ofstream&, Code_Generator&);
	Sementic_Analysis(const Sementic_Analysis&);
	SymbolTable_Analysis& operator= (const Sementic_Analysis&);
};
#endif
