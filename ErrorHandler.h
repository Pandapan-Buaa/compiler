#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "Lexical_analysis.h"
#include <map>
using std::map;

class ErrorHandler
{
public:
	int errorCount;
	vector <SymbolType> endset;
	void addEndset(SymbolType);
	void removeEndset();
	bool searchEndset();
	static ErrorHandler& initErrorHandler(Lexical_analysis&, ofstream& ofs);
	void printError(ErrorType);
	void printError(ErrorType, int);
	void printError(ErrorType, const string&);
	void skip();
private:
	Lexical_analysis& lexical_analysis;
	ofstream& fout;
	ErrorHandler(Lexical_analysis&, ofstream& ofs);
	ErrorHandler(const ErrorHandler&);
	ErrorHandler& operator= (const ErrorHandler&);
};
#endif
