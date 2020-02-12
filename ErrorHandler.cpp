#include "ErrorHandler.h"
#include <iostream>
#include <string>
using namespace std;

#define SYMBOL_VALUE	(lexical_analysis.save)   
#define SYMBOL_TYPE     (lexical_analysis.symbolType)  
#define LINE     (lexical_analysis.lineNumber)  
#define NEXTSYM lexical_analysis.nextSymbol()                

static map<ErrorType, string> errout;
ErrorHandler& ErrorHandler::initErrorHandler(Lexical_analysis& la, ofstream& ofs)
{
	static ErrorHandler theErrorHandler(la, ofs);
	return theErrorHandler;
}
static void initialErrout() {
#define INSERT(type, msg)   \
    errout.insert(pair<ErrorType, string>(ErrorType::type, msg))
	// 初始化错误类型表
	INSERT(ILLEGAL_CHAR, "a");
	INSERT(REDEFINE, "b");
	INSERT(UNDEFINED, "c");
	INSERT(WRONG_ARG_NUM, "d");
	INSERT(WRONG_ARG_TYPE, "e");
	INSERT(ILLEGAL_IFCONDITION, "f");
	INSERT(ILLEGAL_VOIDFUNCRETURN, "g");
	INSERT(ILLEGAL_FUNCRETURN, "h");
	INSERT(ILLEGAL_ARRAYNUM, "i");
	INSERT(CONST_CHANGE, "j");
	INSERT(SHOULD_BE_SEMICN, "k");
	INSERT(SHOULD_BE_RPARENT, "l");
	INSERT(SHOULD_BE_RBRACK, "m");
	INSERT(WITHOUT_WHILE, "n");
	INSERT(ILLEGAL_CONST_ASSIGN, "o");

	/*
	INSERT(ILLEGAL_CHAR, "a 非法符号或不符合词法");
	INSERT(REDEFINE, "b 名字重定义");
	INSERT(UNDEFINED, "c 未定义的名字");
	INSERT(WRONG_ARG_NUM, "d 函数参数个数不匹配");
	INSERT(WRONG_ARG_TYPE, "e 函数参数类型不匹配");
	INSERT(ILLEGAL_IFCONDITION, "f 条件判断中出现不合法的类型");
	INSERT(ILLEGAL_VOIDFUNCRETURN, "g 无返回值的函数存在不匹配的return语句");
	INSERT(ILLEGAL_FUNCRETURN, "h 有返回值的函数缺少return语句或存在不匹配的return语句");
	INSERT(ILLEGAL_ARRAYNUM, "i 数组元素的下标只能是整型表达式");
	INSERT(CONST_CHANGE, "j 不能改变常量的值");
	INSERT(SHOULD_BE_SEMICN, "k 应为分号");
	INSERT(SHOULD_BE_RPARENT, " l 应为右小括号");
	INSERT(SHOULD_BE_RBRACK, "m 应为右中括号");
	INSERT(WITHOUT_WHILE, "n do-while应为语句中缺少while");
	INSERT(ILLEGAL_CONST_ASSIGN, "o 常量定义中=后面只能是整型或字符型常量");*/
#undef INSERT
}

ErrorHandler::ErrorHandler(Lexical_analysis& la, ofstream& ofs) : lexical_analysis(la), fout(ofs) {
	errorCount = 0;
	initialErrout();
}


void ErrorHandler::printError(ErrorType err)
{
	errorCount++;
	if (err == SHOULD_BE_SEMICN)	 fout << lexical_analysis.beforenumber << " " << errout.find(err)->second << endl;
	else fout << LINE << " " << errout.find(err)->second << endl;
}

void ErrorHandler::printError(ErrorType err, int lineNumber)
{
	errorCount++;
	fout << lineNumber << " " << errout.find(err)->second << endl;
}

void ErrorHandler::printError(ErrorType err, const string& msg)
{
	errorCount++;
	fout << LINE << " " << errout.find(err)->second << ". (" << msg << ")" << endl;
}
//跳转到后继集合,返回错误类型
void ErrorHandler::skip()
{
	while (searchEndset() == false) {
		NEXTSYM;
	}
}
void ErrorHandler::addEndset(SymbolType st)
{
	endset.push_back(st);
}

void ErrorHandler::removeEndset()
{
	endset.clear();
}

bool ErrorHandler::searchEndset()
{
	for (vector<SymbolType>::iterator it = endset.begin(); it != endset.end(); it++) {
		if (*it == SYMBOL_TYPE) {
			return true;
		}
	}
	return false;
}
