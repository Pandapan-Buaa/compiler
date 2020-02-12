#ifndef LEXICAL_ANALYSIS__H
#define LEXICAL_ANALYSIS__H
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "type.h"
using std::string;
using std::ifstream;
using std::ofstream;

class Lexical_analysis
{
public:
	static Lexical_analysis& initialLexicalanalysis(ifstream& file, ofstream& ofs, ofstream& err);
	bool overflow;
	void nextSymbol();
	void nextString();
	void nextChar();
	//void nextNum();
private:
	ifstream& sourceFile;
	SymbolType symbolType;
	int lineNumber;
	int beforenumber;
	ofstream& fout;
	ofstream& err;
	friend class Grammar_analysis;
	friend class ErrorHandler;
	friend class Sementic_Analysis;
	int number;
	string save;
	Lexical_analysis(ifstream& file, ofstream& ofs, ofstream& err);
	Lexical_analysis(const Lexical_analysis&);
	Lexical_analysis& operator  = (const Lexical_analysis&);
};


using std::ostream;
ostream& operator<< (ostream& ofs, SymbolType st);
ostream& operator<< (ostream& ofs, BasicType st);
ostream& operator<< (ostream& ofs, IdentifierType st);

#endif
