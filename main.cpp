#include "Lexical_analysis.h"
#include "Grammar_analysis.h"
#include "SymbolTable_Analysis.h"
#include "Sementic_Analysis.h"
#include "Quaternions.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>


using namespace std;

int main() {
	ifstream fin;
	fin.open("testfile.txt");
	ofstream fout;
	fout.open("output.txt");
	ofstream err;
	err.open("error.txt");
	ofstream midcode;
	midcode.open("midcode.txt");
	FILE* mips;
	ofstream target;
	target.open("mips1.txt");
	if (!fin.is_open() || !fout.is_open() || !err.is_open()) {
		cerr << "err: cannot open file: " << endl;
		cerr << "Please check the path and file name." << endl;
		return 0;
	}

	Lexical_analysis& lexical_analysis = Lexical_analysis::initialLexicalanalysis(fin, fout, err);
	ErrorHandler& errorHandler = ErrorHandler::initErrorHandler(lexical_analysis, err);
	SymbolTable_Analysis& symbolTable = SymbolTable_Analysis::initSymbolTable();
	Quaternions quaternions;
	Code_Generator genemips(target, mips, midcode, quaternions, symbolTable);
	Sementic_Analysis& sementic = Sementic_Analysis::initSementic(errorHandler, symbolTable, lexical_analysis, quaternions, midcode, genemips);

	Grammar_analysis grammar_analysis(lexical_analysis, errorHandler, symbolTable, sementic, fout, genemips);
	/*while (lexical_analysis.getSymbol() != FINISH) {
		lexical_analysis.nextSymbol();
	}*/
	grammar_analysis.grammarAnalyze();
	genemips.getasm();
	genemips.outsymbol();

}
