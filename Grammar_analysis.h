#ifndef GRAMMA_ANALYSIS__H
#define GRAMMA_ANALYSIS__H
#include <fstream>
#include "Lexical_analysis.h"
#include "ErrorHandler.h"
#include "Sementic_Analysis.h"
#include "SymbolTable_Analysis.h"
#include "Code_Generator.h"
using std::ofstream;
class Grammar_analysis {
public:
	Grammar_analysis(Lexical_analysis&, ErrorHandler& eh, SymbolTable_Analysis& table, Sementic_Analysis& se, ofstream&, Code_Generator&);
	void grammarAnalyze(); //测试
	void stringAnalysis(); //字符串
	void numAnalysis(int&);    // 无符号整数
	void numAnalysis();

	void intAnalysis(int&);    // 整数
	void constdefineAnalysis(); //常量定义
	void constlistAnalysis(); //常量说明
	void nameAnalysis(string&, BasicType&);//声明头部

	void vardefineAnalysis(); //变量定义
	void varlistAnalysis(); //变量说明

	void exprAnalysis(int&, string& value); //表达式
	void termAnalysis(int&, string& value); //项
	void factorAnalysis(int&, string& value); //因子


	void statementAnalysis(vector<pair<int, int>>&);//语句

	void funcstatementAnalysis(); //带（无）返回值函数语句

	void funcparavalueAnalysis(vector<int>&, string name);//值参数表

	void assignstatementAnalysis(); //赋值语句
	void ifstatementAnalysis(vector<pair<int, int>>&); //条件语句
	void ifconditionAnalysis(SymbolType& condition, string&); //条件
	void loopstatementAnalysis(vector<pair<int, int>>&); //循环语句
	void loopintAnalysis(int& value); //步长
	void scanfAnalysis(); //读语句
	void printfAnalysis(); //写语句

	void returnAnalysis(vector<pair<int, int>>&); //返回语句

	void statementsAnalysis(vector<pair<int, int>>&);//语句列

	void funcdefinewithReturnAnalysis(); //有返回值函数定义
	void funcdefinewithoutReturnAnalysis(); //无返回值函数定义

	void complexstatementAnalysis(string, vector<pair<int, int>>&); //复合语句 

	//line type -2结尾 -1分号 0 int 1 char 
	void funcparaAnalysis(vector<Argument>&);//参数表

	void mainAnalysis(); //主函数
	void programAnalysis(); //程序

	void skip();
	void skiptocs();

	bool searchVector(vector<string> names, string name);



private:
	int symbolCount;                    // 符号计数
	ofstream& fout;
	Lexical_analysis& lexical_analysis;   // 词法分析器，语法分析使用
	ErrorHandler& errorHandler;         // 错误处理器
	Sementic_Analysis& sementic_Analysis; //处理函数(错误处理后继集合处理+表填写)
	SymbolTable_Analysis& table;//符号表
	Code_Generator& generator;


};
#define SYMBOL_VALUE   (lexical_analysis.save)     
#define SYMBOL_NUMBER    (lexical_analysis.number)      
#define SYMBOL_TYPE     (lexical_analysis.symbolType)  

#define NEXTSYM {                           \
    lexical_analysis.nextSymbol();           \
    symbolCount++;                          \
}
#endif
