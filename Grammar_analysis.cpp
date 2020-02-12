#include "Grammar_analysis.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include "string.h"
using namespace std;
using LEXICAL_ANALYSIS__H::SymbolType;

map<string, int > SymbolTable;

#define OUT(item) {                                     				\
	Grammar_analysis::fout << '<' << item << '>' << endl;  \
}
#define OUTSYM {                                         \
	Grammar_analysis::fout << SYMBOL_TYPE << ' ' <<SYMBOL_VALUE << endl;     \
}
#define ERROR {                                         \
	Grammar_analysis::fout << "!!!!!!!!!!!!!!!!!!!!!!" << endl;     \
}
#define ADDMIDCODE generator.addmidcode
// 错误处理
#define ERROUT  errorHandler.printError



Grammar_analysis::Grammar_analysis(Lexical_analysis& la, ErrorHandler& eh, SymbolTable_Analysis& table, Sementic_Analysis& se, ofstream& ofs, Code_Generator& generator) : symbolCount(0), fout(ofs), lexical_analysis(la), errorHandler(eh), sementic_Analysis(se), table(table), generator(generator) {
}

//测试
void Grammar_analysis::grammarAnalyze() {
	NEXTSYM;
	OUTSYM;
	programAnalysis();
	generator.outmidcode();
	for (int i = 0; i < table.getStringMap().size(); i++) {
		cout << table.getStringMap().at(i) << endl;
	}
	//map<string, int>::iterator iter;
	/*for (iter = SymbolTable.begin(); iter != SymbolTable.end(); iter++) {
		cout << iter->first << " : " << iter->second << endl;
	}*/
}
//字符串 1
void Grammar_analysis::stringAnalysis() {
	if (SYMBOL_TYPE == STRCON) {
		OUT("字符串");
	}
	NEXTSYM;
	//OUTSYM;
}
//无符号整数 1
void Grammar_analysis::numAnalysis(int& value) {
	if (SYMBOL_TYPE == INTCON) {
		value = SYMBOL_NUMBER;
		OUT("无符号整数");
	}
	NEXTSYM;
	//OUTSYM;
}
void Grammar_analysis::numAnalysis()
{
	if (SYMBOL_TYPE == INTCON) {
		OUT("无符号整数");
	}
	NEXTSYM;
	//OUTSYM;
}
//整数 1
void Grammar_analysis::intAnalysis(int& value) {
	if (SYMBOL_TYPE == PLUS || SYMBOL_TYPE == MINU) {
		int mult = 1;
		if (SYMBOL_TYPE == MINU) mult = -1;
		NEXTSYM;
		OUTSYM;
		numAnalysis(value);
		value = mult * value;
		OUT("整数");
	}
	else if (SYMBOL_TYPE == INTCON) {
		numAnalysis(value);
		OUT("整数");
	}
	else { ERROR; }
	//NEXTSYM;
	//OUTSYM;
}
//常量定义 1
void Grammar_analysis::constdefineAnalysis() {
	BasicType type;
	string name;
	int value = 0;
	bool redefine = false;
	if (SYMBOL_TYPE == INTTK) {
		type = INTEGER;
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == IDENFR) {
			name = SYMBOL_VALUE;
			if (table.isHere(SYMBOL_VALUE)) {
				ERROUT(REDEFINE);
				redefine = true;
			}
			NEXTSYM;
			OUTSYM;
			if (SYMBOL_TYPE == ASSIGN) {
				NEXTSYM;
				OUTSYM;
				//intAnalysis(value);
				if (SYMBOL_TYPE == PLUS || SYMBOL_TYPE == MINU) {
					int mult = 1;
					if (SYMBOL_TYPE == MINU) mult = -1;
					NEXTSYM;
					OUTSYM;
					//numAnalysis(value);
					if (SYMBOL_TYPE == INTCON) {
						value = SYMBOL_NUMBER;
						OUT("无符号整数");
					}
					NEXTSYM;

					value = mult * value;
					OUT("整数");
				}
				else if (SYMBOL_TYPE == INTCON) {
					//numAnalysis(value);
					if (SYMBOL_TYPE == INTCON) {
						value = SYMBOL_NUMBER;
						OUT("无符号整数");
					}
					NEXTSYM;
					OUT("整数");
				}
				else {
					ERROUT(ILLEGAL_CONST_ASSIGN);
					skiptocs();
				}
				if (SYMBOL_TYPE == COMMA) {
					OUTSYM;
					if (!redefine)	sementic_Analysis.constDefine(name, type, value);
				}
				else {
					OUT("常量定义");
					OUTSYM;
					if (!redefine) sementic_Analysis.constDefine(name, type, value);
				}
			}
			else ERROR;
		}
		else ERROR;
		while (SYMBOL_TYPE == COMMA) {
			redefine = false;
			NEXTSYM;
			OUTSYM;
			if (SYMBOL_TYPE == IDENFR) {
				name = SYMBOL_VALUE;
				if (table.isHere(SYMBOL_VALUE)) {
					ERROUT(REDEFINE);
					redefine = true;
				}
				NEXTSYM;
				OUTSYM;
				if (SYMBOL_TYPE == ASSIGN) {
					NEXTSYM;
					OUTSYM;
					//intAnalysis(value);
					if (SYMBOL_TYPE == PLUS || SYMBOL_TYPE == MINU) {
						int mult = 1;
						if (SYMBOL_TYPE == MINU) mult = -1;
						NEXTSYM;
						OUTSYM;
						//numAnalysis(value);
						if (SYMBOL_TYPE == INTCON) {
							value = SYMBOL_NUMBER;
							OUT("无符号整数");
						}
						NEXTSYM;

						value = mult * value;
						OUT("整数");
					}
					else if (SYMBOL_TYPE == INTCON) {
						//numAnalysis(value);
						if (SYMBOL_TYPE == INTCON) {
							value = SYMBOL_NUMBER;
							OUT("无符号整数");
						}
						NEXTSYM;
						OUT("整数");
					}
					else {
						ERROUT(ILLEGAL_CONST_ASSIGN);
						skiptocs();
					}
					if (SYMBOL_TYPE == COMMA) {
						OUTSYM;
						if (!redefine)	sementic_Analysis.constDefine(name, type, value);
					}
					else {
						OUT("常量定义");
						OUTSYM;
						if (!redefine)	sementic_Analysis.constDefine(name, type, value);
					}
				}
				else ERROR;
			}
		}

	}
	else if (SYMBOL_TYPE == CHARTK) {
		type = CHAR;
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == IDENFR) {
			name = SYMBOL_VALUE;
			if (table.isHere(SYMBOL_VALUE)) {
				ERROUT(REDEFINE);
				redefine = true;
			}
			NEXTSYM;
			OUTSYM;
			if (SYMBOL_TYPE == ASSIGN) {
				NEXTSYM;
				OUTSYM;
				if (SYMBOL_TYPE == CHARCON) {
					value = SYMBOL_VALUE[0];
					NEXTSYM;
					if (SYMBOL_TYPE == COMMA) {
						OUTSYM;
						if (!redefine)	sementic_Analysis.constDefine(name, type, value);
					}
					else {
						OUT("常量定义");
						OUTSYM;
						if (!redefine)	sementic_Analysis.constDefine(name, type, value);
					}
				}
				else {
					ERROUT(ILLEGAL_CONST_ASSIGN);
					skiptocs();
				}
			}
			else ERROR;
		}
		else ERROR;
		while (SYMBOL_TYPE == COMMA) {
			redefine = false;
			NEXTSYM;
			OUTSYM;
			if (SYMBOL_TYPE == IDENFR) {
				name = SYMBOL_VALUE;
				if (table.isHere(SYMBOL_VALUE)) {
					ERROUT(REDEFINE);
					redefine = true;
				}
				NEXTSYM;
				OUTSYM;
				if (SYMBOL_TYPE == ASSIGN) {
					NEXTSYM;
					OUTSYM;
					if (SYMBOL_TYPE == CHARCON) {
						value = SYMBOL_VALUE[0];
						NEXTSYM;
						if (SYMBOL_TYPE == COMMA) {
							OUTSYM;
							if (!redefine)	sementic_Analysis.constDefine(name, type, value);
						}
						else {
							OUT("常量定义");
							OUTSYM;
							if (!redefine)	sementic_Analysis.constDefine(name, type, value);
						}
					}
					else {
						ERROUT(ILLEGAL_CONST_ASSIGN);
						skiptocs();
					}
				}
				else ERROR;
			}
		}
	}
	else {
		ERROR;
	}

}
//常量说明 1
void Grammar_analysis::constlistAnalysis() {
	//NEXTSYM;
	//OUTSYM;
	while (SYMBOL_TYPE == CONSTTK) {
		NEXTSYM;
		OUTSYM;
		constdefineAnalysis();
		if (SYMBOL_TYPE == SEMICN) {
			NEXTSYM;
		WITHOUTSEMI:
			if (SYMBOL_TYPE == CONSTTK) {}
			else {
				OUT("常量说明");
			}
			OUTSYM;
		}
		else {
			ERROUT(SHOULD_BE_SEMICN);
			goto WITHOUTSEMI;
		}
	}
}
//声明头部 1
void Grammar_analysis::nameAnalysis(string& funcname, BasicType& type)
{
	if (SYMBOL_TYPE == INTTK || SYMBOL_TYPE == CHARTK) {
		int tmptype = 0;
		type = (SYMBOL_TYPE == CHARTK) ? CHAR : INTEGER;
		if (SYMBOL_TYPE == INTTK) tmptype = 1;
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == IDENFR) {
			funcname = SYMBOL_VALUE;
			SymbolTable.insert(pair<string, int>(SYMBOL_VALUE, tmptype));
			OUT("声明头部");
		}
		else {
			ERROR;
		}
	}
	else {
		ERROR;
	}
	NEXTSYM;
	OUTSYM;
}
//变量定义 1
void Grammar_analysis::vardefineAnalysis()
{
	string name;
	int limit;
	BasicType type;
	bool redefine = false;
	if (SYMBOL_TYPE == INTTK || SYMBOL_TYPE == CHARTK) {
		type = (SYMBOL_TYPE == CHARTK) ? CHAR : INTEGER;
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == IDENFR) {
			name = SYMBOL_VALUE;
			if (table.isHere(SYMBOL_VALUE)) {
				ERROUT(REDEFINE);
				redefine = false;
			}
			NEXTSYM;
			if (SYMBOL_TYPE == LBRACK) {
				OUTSYM;
				NEXTSYM;
				OUTSYM;
				numAnalysis(limit);
				if (!redefine)	sementic_Analysis.arrayDefine(name, type, limit);
				OUTSYM;
				if (SYMBOL_TYPE == RBRACK) {
					NEXTSYM;
				WITHOUTRB:
					if (SYMBOL_TYPE == COMMA) {
						OUTSYM;
					}
					else {
						OUT("变量定义");
						OUTSYM;
					}

				}
				else {
					ERROUT(SHOULD_BE_RBRACK);
					goto WITHOUTRB;
				}
			}
			else {
				if (SYMBOL_TYPE == COMMA) {
					if (!redefine)	sementic_Analysis.varDefine(name, type);
				}
				else {
					OUT("变量定义");
					if (!redefine)	sementic_Analysis.varDefine(name, type);
				}
				OUTSYM;
			}
		}
		else ERROR;
	}
	else ERROR;
	while (SYMBOL_TYPE == COMMA) {
		redefine = false;
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == IDENFR) {
			name = SYMBOL_VALUE;
			if (table.isHere(SYMBOL_VALUE)) {
				ERROUT(REDEFINE);
				redefine = true;
			}
			NEXTSYM;
			if (SYMBOL_TYPE == LBRACK) {
				OUTSYM;
				NEXTSYM;
				OUTSYM;
				numAnalysis(limit);
				if (!redefine)	sementic_Analysis.arrayDefine(name, type, limit);
				OUTSYM;
				if (SYMBOL_TYPE == RBRACK) {
					NEXTSYM;
				WITHOUTRB1:
					if (SYMBOL_TYPE == COMMA) {
						OUTSYM;
					}
					else {
						OUT("变量定义");
						OUTSYM;
					}

				}
				else {
					ERROUT(SHOULD_BE_RBRACK);
					goto WITHOUTRB1;
				}
			}
			else {
				if (SYMBOL_TYPE == COMMA) {
					if (!redefine)	sementic_Analysis.varDefine(name, type);
				}
				else {
					OUT("变量定义");
					if (!redefine)	sementic_Analysis.varDefine(name, type);
				}
				OUTSYM;
			}
		}
		else ERROR;
	}
}
//变量说明 1
void Grammar_analysis::varlistAnalysis()
{
	//NEXTSYM;
	//OUTSYM;
	while (SYMBOL_TYPE == INTTK || SYMBOL_TYPE == CHARTK) {
		vardefineAnalysis();
		if (SYMBOL_TYPE == SEMICN) {
			NEXTSYM;
		WITHOUTSEMI:
			if (SYMBOL_TYPE == INTTK || SYMBOL_TYPE == CHARTK) {}
			else {
				OUT("变量说明");
			}
			OUTSYM;
		}
		else {
			ERROUT(SHOULD_BE_SEMICN);
			goto WITHOUTSEMI;
		}
	}
	//OUTSYM;

}
//表达式
void Grammar_analysis::exprAnalysis(int& exprtype, string& value)
{
	bool neg = false;
	string oper1, oper2, dest;
	char op;
	exprtype = 0;
	int filltype;
	if (SYMBOL_TYPE == PLUS || SYMBOL_TYPE == MINU) {
		if (SYMBOL_TYPE == MINU) neg = true;
		NEXTSYM;
		OUTSYM;
		termAnalysis(exprtype, dest);
		if (SYMBOL_TYPE == PLUS || SYMBOL_TYPE == MINU) {
		}
		else {
			OUT("表达式");
			//OUTSYM;
		}
		exprtype = 0;
	}
	else {
		termAnalysis(exprtype, dest);
		if (SYMBOL_TYPE == PLUS || SYMBOL_TYPE == MINU) {
		}
		else {
			OUT("表达式");
			//OUTSYM;
		}
	}
	if (neg) {
		oper1 = dest;
		//sementic_Analysis.negative(oper1, dest, table.position);
		dest = sementic_Analysis.newvar();
		ADDMIDCODE("minus", "0", oper1, dest);
		exprtype = 0;

	}
	while (SYMBOL_TYPE == PLUS || SYMBOL_TYPE == MINU) {
		op = (SYMBOL_TYPE == PLUS) ? '+' : '-';
		OUTSYM;
		NEXTSYM;
		OUTSYM;
		oper1 = dest;
		termAnalysis(filltype, oper2);
		switch (op) {
		case '+':
			sementic_Analysis.addition(oper1, oper2, dest, table.position);
			break;
		case '-':
			sementic_Analysis.subtraction(oper1, oper2, dest, table.position);
			break;
		default:
			break;
		}
		if (SYMBOL_TYPE == PLUS || SYMBOL_TYPE == MINU) {}
		else {
			OUT("表达式");
			//OUTSYM;
		}
		exprtype = 0;
	}
	value = dest;
}
//项
void Grammar_analysis::termAnalysis(int& termtype, string& value)
{
	string oper1, oper2, dest;
	char op;
	int filltype;
	factorAnalysis(termtype, dest);
	if (SYMBOL_TYPE == MULT || SYMBOL_TYPE == DIV) {
		termtype = 0;
	}
	else {
		OUT("项");
	}
	while (SYMBOL_TYPE == MULT || SYMBOL_TYPE == DIV) {
		op = (SYMBOL_TYPE == MULT) ? '*' : '/';
		OUTSYM;
		NEXTSYM;
		OUTSYM;
		oper1 = dest;
		factorAnalysis(filltype, oper2);
		switch (op) {
		case '*':
			sementic_Analysis.multiplicatioin(oper1, oper2, dest, table.position);
			break;
		case '/':
			sementic_Analysis.division(oper1, oper2, dest, table.position);
			break;
		default:
			break;
		}
		if (SYMBOL_TYPE == MULT || SYMBOL_TYPE == DIV) {}
		else {
			OUT("项");
			//OUTSYM;
		}
	}
	value = dest;
}
//因子
void Grammar_analysis::factorAnalysis(int& factortype, string& value)
{
	int op = 1;
	int filltype;
	BasicType valuetype = INTEGER;
	if (SYMBOL_TYPE == IDENFR) {
		string funcname = SYMBOL_VALUE;
		if (table.find(SYMBOL_VALUE) == false) {
			ERROUT(UNDEFINED);
		}
		else valuetype = table.ste->valueType;
		string name = table.encodeKey();
		int count = SymbolTable.count(SYMBOL_VALUE);

		//cout << SYMBOL_TYPE << " " << SYMBOL_VALUE << " " << count << endl;
		NEXTSYM;
		if (SYMBOL_TYPE == LBRACK) {
			OUTSYM;
			NEXTSYM;
			OUTSYM;
			//numAnalysis();
			string offset;
			exprAnalysis(filltype, offset);
			OUTSYM;
			if (SYMBOL_TYPE == RBRACK) {
				OUT("因子");
				NEXTSYM;
				//OUTSYM;
				//数组
				sementic_Analysis.assignfromArray(name, offset, value, table.position);
			}
			else {
				ERROUT(SHOULD_BE_RBRACK);
			}
		}
		else if (SYMBOL_TYPE == LPARENT) {

			OUTSYM;
			NEXTSYM;
			//OUTSYM;
			vector<int> paravec;
			funcparavalueAnalysis(paravec, funcname);
			table.find(funcname);
			//cout << table.ste->valueType << endl;
			if (table.ste != NULL) {
				sementic_Analysis.callfunc(name, value, table.position);
				//cout << "bugggggggggg" << endl;
				//sementic_Analysis.jumpAnyway(table.encodeKey(), table.position);
				//sementic_Analysis.callfuncreturn(value, table.position);
			}
			if (table.find(funcname) == true) {
				if (table.ste->paranum != paravec.size()) {
					ERROUT(WRONG_ARG_NUM);
				}
				else if (table.ste->arguments != NULL) {
					for (int i = 0; i < table.ste->paranum; i++) {
						if (table.ste->arguments->at(i).type == INTEGER) {
							if (paravec.at(i) == 0);
							else {
								ERROUT(WRONG_ARG_TYPE);
								break;
							}
						}
						else {
							if (paravec.at(i) == 1);
							else {
								ERROUT(WRONG_ARG_TYPE);
								break;
							}
						}
					}
				}
			}
			if (SYMBOL_TYPE == RPARENT) {
				if (count == 1) {
					OUT("有返回值函数调用语句");
					OUT("因子");
					NEXTSYM;

				}
				else {
					OUT("无返回值函数调用语句");
					OUT("因子");
					NEXTSYM;
					//exit(1);
				}
				//OUTSYM;
			WITHOUTRP:;
			}
			else {
				ERROUT(SHOULD_BE_RPARENT);
				goto WITHOUTRP;
			}
		}
		else {
			OUT("因子");
			//value = table.encodeKey();
			value = name;
		}
		factortype = (valuetype == INTEGER) ? 0 : 1;
	}
	else if (SYMBOL_TYPE == LPARENT) {
		NEXTSYM;
		OUTSYM;
		//numAnalysis();
		exprAnalysis(factortype, value);
		if (SYMBOL_TYPE == RPARENT) {
			OUTSYM;
			OUT("因子");
			NEXTSYM;
			//OUTSYM;
		}
		else {
			ERROUT(SHOULD_BE_RPARENT);
		}
		factortype = 0;
	}
	else if (SYMBOL_TYPE == INTCON) {
		sementic_Analysis.intToString(SYMBOL_NUMBER, value);
		OUT("无符号整数");
		OUT("整数");
		OUT("因子");
		NEXTSYM;
		factortype = 0;

	}
	else if (SYMBOL_TYPE == CHARCON) {
		sementic_Analysis.charToString(SYMBOL_VALUE[0], value);
		OUT("因子");
		NEXTSYM;
		factortype = 1;
	}
	else if (SYMBOL_TYPE == PLUS || SYMBOL_TYPE == MINU) {
		op = (SYMBOL_TYPE == PLUS) ? 1 : -1;
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == INTCON) {
			sementic_Analysis.intToString(op * SYMBOL_NUMBER, value);
			OUT("无符号整数");
			OUT("整数");
			OUT("因子");
		}
		factortype = 0;
		NEXTSYM;
	}
	else ERROR;
}
//值参数表 
void Grammar_analysis::funcparavalueAnalysis(vector<int>& paravec, string funcname)
{
	string value;
	vector<string> savefuncname;
	vector<string> saveparaname;
	vector<string> savevalue;
	int paraid = 0;
	if (SYMBOL_TYPE == RPARENT) {
		OUT("值参数表");
		OUTSYM;
	}
	else if (SYMBOL_TYPE == IDENFR || SYMBOL_TYPE == LPARENT || SYMBOL_TYPE == INTCON || SYMBOL_TYPE == CHARCON || SYMBOL_TYPE == PLUS || SYMBOL_TYPE == MINU) {
		//numAnalysis();
		OUTSYM;
		int exprtype = 0;
		exprAnalysis(exprtype, value);
		table.find(funcname);
		savefuncname.push_back(funcname);
		saveparaname.push_back(table.ste->arguments->at(paraid).name);
		savevalue.push_back(value);
		//cout << " *" << value << endl;
		//sementic_Analysis.passParameter(value, table.position, table.ste->arguments->at(paraid).name, funcname);
		paraid++;
		paravec.push_back(exprtype);
		if (SYMBOL_TYPE == COMMA) {

		}
		else {
			OUT("值参数表");
			OUTSYM;
			//NEXTSYM;
		}

		while (SYMBOL_TYPE == COMMA) {
			OUTSYM;
			NEXTSYM;
			OUTSYM;
			//numAnalysis();
			exprAnalysis(exprtype, value);
			table.find(funcname);
			savefuncname.push_back(funcname);
			saveparaname.push_back(table.ste->arguments->at(paraid).name);
			savevalue.push_back(value);
			//cout << " *" << value << endl;
			//sementic_Analysis.passParameter(value, table.position, table.ste->arguments->at(paraid).name, funcname);
			paraid++;
			paravec.push_back(exprtype);
			if (SYMBOL_TYPE == COMMA) {}
			else {
				OUT("值参数表");
				OUTSYM;
			}

		}
	}
	else ERROR;
	for (int i = 0; i < savefuncname.size(); i++) {
		sementic_Analysis.passParameter(savevalue.at(i), table.position, saveparaname.at(i), savefuncname.at(i));
	}
}
//条件语句 
void Grammar_analysis::ifstatementAnalysis(vector<pair<int, int>>& returnvec)
{
	SymbolType condition;
	string elseLabel = sementic_Analysis.newLabel("condition_else");
	string endLabel = sementic_Analysis.newLabel("condition_end");

	string lab1 = sementic_Analysis.newlabel();
	string lab2 = sementic_Analysis.newlabel();
	string result;
	if (SYMBOL_TYPE == IFTK) {
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == LPARENT) {
			NEXTSYM;
			OUTSYM;
			ifconditionAnalysis(condition, result);
			ADDMIDCODE("BZ", "", "", lab1);//lab1是else lab2是结束
			//sementic_Analysis.dissatisfyJump(condition, elseLabel, table.position);
			if (SYMBOL_TYPE == RPARENT) {
				NEXTSYM;
				OUTSYM;
			WITHOUTRP:
				statementAnalysis(returnvec);
				//sementic_Analysis.jumpAnyway(endLabel, table.position);
				//sementic_Analysis.label(elseLabel);
				ADDMIDCODE("GOTO", "", "", lab2);
				ADDMIDCODE("lab:", "", "", lab1);
				if (SYMBOL_TYPE == ELSETK) {
					OUTSYM;
					NEXTSYM;
					OUTSYM;
					statementAnalysis(returnvec);
					OUT("条件语句");
					//OUTSYM;
				}
				else {
					OUT("条件语句");
					//OUTSYM;
				}
			}
			else {
				ERROUT(SHOULD_BE_RPARENT);
				goto WITHOUTRP;
			}
		}
		else ERROR;
	}
	else ERROR;
	//sementic_Analysis.label(endLabel);
	ADDMIDCODE("lab:", "", "", lab2);
}
//条件 
void Grammar_analysis::ifconditionAnalysis(SymbolType& condition, string& value)
{
	string value1, value2;
	string var3;
	int expr1type = 0;
	int expr2type = 0;
	exprAnalysis(expr1type, value1);
	if (SYMBOL_TYPE == LSS || SYMBOL_TYPE == LEQ || SYMBOL_TYPE == GRE || SYMBOL_TYPE == GEQ || SYMBOL_TYPE == EQL || SYMBOL_TYPE == NEQ) {
		condition = SYMBOL_TYPE;
		OUTSYM;
		NEXTSYM;
		OUTSYM;
		exprAnalysis(expr2type, value2);
		OUT("条件");
		OUTSYM;
		string op;
		switch (condition) {
		case LSS: op = "<"; break;
		case LEQ: op = "<="; break;
		case GRE:op = ">"; break;
		case GEQ:op = ">="; break;
		case EQL:op = "=="; break;
		case NEQ:op = "!="; break;
		default:
			break;
		}
		var3 = sementic_Analysis.newvar();
		ADDMIDCODE(op, value1, value2, var3);
	}
	else {
		OUT("条件");
		OUTSYM;
		condition = NEQ;
		sementic_Analysis.intToString(0, value2);
		var3 = sementic_Analysis.newvar();
		ADDMIDCODE("!=", value1, value2, var3);

	}
	sementic_Analysis.compare(value1, value2, table.position);

	if (expr1type != 0 || expr2type != 0) {
		ERROUT(ILLEGAL_IFCONDITION);
	}
	value = var3;
}
//循环语句
void Grammar_analysis::loopstatementAnalysis(vector<pair<int, int>>& returnvec)
{
	string result; // 记录结果
	string forvar;
	int filltype;
	string value;
	int step;
	SymbolType condition;
	string whilebegin = sementic_Analysis.newLabel("while-begin");
	string whileend = sementic_Analysis.newLabel("while-end");
	string dowhilebegin = sementic_Analysis.newLabel("dowhile-begin");
	string forbegin = sementic_Analysis.newLabel("for-begin");
	string forend = sementic_Analysis.newLabel("for-end");



	if (SYMBOL_TYPE == WHILETK) {
		string wb = sementic_Analysis.newlabel();
		string we = sementic_Analysis.newlabel();
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == LPARENT) {
			NEXTSYM;
			OUTSYM;
			//sementic_Analysis.label(whilebegin);
			ADDMIDCODE("lab:", "", "", wb);
			ifconditionAnalysis(condition, result);
			ADDMIDCODE("BZ", "", "", we);
			//sementic_Analysis.dissatisfyJump(condition, whileend, table.position);
			if (SYMBOL_TYPE == RPARENT) {
				NEXTSYM;
			WHILEWITHOUTRP:
				OUTSYM;
				statementAnalysis(returnvec);
				OUT("循环语句");
				//sementic_Analysis.jumpAnyway(whilebegin, table.position);
				ADDMIDCODE("GOTO", "", "", wb);
				ADDMIDCODE("lab:", "", "", we);
				//OUTSYM;
			}
			else {
				ERROUT(SHOULD_BE_RPARENT);
				goto WHILEWITHOUTRP;
			}
		}
		else ERROR;
		sementic_Analysis.label(whileend);
	}
	else if (SYMBOL_TYPE == DOTK) {
		string dwb = sementic_Analysis.newlabel();
		string dwe = sementic_Analysis.newlabel();

		NEXTSYM;
		OUTSYM;
		sementic_Analysis.label(dowhilebegin);
		ADDMIDCODE("lab:", "", "", dwb);
		statementAnalysis(returnvec);
		OUTSYM;
		if (SYMBOL_TYPE == WHILETK) {
			NEXTSYM;
			OUTSYM;
		WITHOUTWHILE:
			if (SYMBOL_TYPE == LPARENT) {
				NEXTSYM;
				OUTSYM;
				ifconditionAnalysis(condition, result);
				ADDMIDCODE("BZ", "", "", dwe);
				ADDMIDCODE("GOTO", "", "", dwb);
				ADDMIDCODE("lab:", "", "", dwe);
				sementic_Analysis.satisfyJump(condition, dowhilebegin, table.position);
				if (SYMBOL_TYPE == RPARENT) {
					NEXTSYM;
				DOWITHOUTRP:
					OUT("循环语句");
					//OUTSYM;
				}
				else {
					ERROUT(SHOULD_BE_RPARENT);
					goto DOWITHOUTRP;
				}
			}
			else ERROR;
		}
		else {
			ERROUT(WITHOUT_WHILE);
			goto WITHOUTWHILE;
		}
	}
	else if (SYMBOL_TYPE == FORTK) {
		string fb = sementic_Analysis.newlabel();
		string fe = sementic_Analysis.newlabel();
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == LPARENT) {
			NEXTSYM;
			OUTSYM;
			if (SYMBOL_TYPE == IDENFR) {
				if (table.find(SYMBOL_VALUE) == false) {
					ERROUT(UNDEFINED);
				}
				forvar = table.encodeKey();
				NEXTSYM;
				OUTSYM;
				if (SYMBOL_TYPE == ASSIGN) {
					NEXTSYM;
					OUTSYM;
					exprAnalysis(filltype, value);
					sementic_Analysis.assign(forvar, value, table.position);
					OUTSYM;
					if (SYMBOL_TYPE == SEMICN) {
						NEXTSYM;
						OUTSYM;
					SEMI1:
						sementic_Analysis.label(forbegin);
						ADDMIDCODE("lab:", "", "", fb);
						ifconditionAnalysis(condition, result);
						ADDMIDCODE("BZ", "", "", fe);
						sementic_Analysis.dissatisfyJump(condition, forend, table.position);
						if (SYMBOL_TYPE == SEMICN) {
							NEXTSYM;
							OUTSYM;
						SEMI2:
							if (SYMBOL_TYPE == IDENFR) {
								string var1 = SYMBOL_VALUE;
								if (table.find(SYMBOL_VALUE) == false) {
									ERROUT(UNDEFINED);
								}
								string encodename = table.encodeKey();
								NEXTSYM;
								OUTSYM;
								if (SYMBOL_TYPE == ASSIGN) {
									NEXTSYM;
									OUTSYM;
									if (SYMBOL_TYPE == IDENFR) {
										string var2 = SYMBOL_VALUE;
										if (table.find(SYMBOL_VALUE) == false) {
											ERROUT(UNDEFINED);
										}

										NEXTSYM;
										OUTSYM;
										if (SYMBOL_TYPE == PLUS || SYMBOL_TYPE == MINU) {
											SymbolType op = SYMBOL_TYPE;
											NEXTSYM;
											OUTSYM;
											loopintAnalysis(step);
											string stepstr;
											sementic_Analysis.intToString(step, stepstr);
											if (SYMBOL_TYPE == RPARENT) {
												NEXTSYM;
												OUTSYM;
											FORWITHOUTRP:
												statementAnalysis(returnvec);

												string tmp;

												if (op == PLUS) {
													ADDMIDCODE("plus", var2, stepstr, var1);
												}
												else {
													ADDMIDCODE("minus", var2, stepstr, var1);
												}
												sementic_Analysis.jumpAnyway(forbegin, table.position);
												ADDMIDCODE("GOTO", "", "", fb);
												ADDMIDCODE("lab:", "", "", fe);
												OUT("循环语句");
												//OUTSYM;
											}
											else {
												ERROUT(SHOULD_BE_RPARENT);
												goto FORWITHOUTRP;
											}
										}
										else ERROR;
									}
									else ERROR;
								}
								else ERROR;
							}
							else ERROR;

						}
						else {
							ERROUT(SHOULD_BE_SEMICN);
							goto SEMI2;
						}
					}
					else {
						ERROUT(SHOULD_BE_SEMICN);
						goto SEMI1;
					}
				}
				else ERROR;
			}
			else ERROR;
		}
		else ERROR;
		sementic_Analysis.label(forend);
	}
	else ERROR;

}
//步长 
void Grammar_analysis::loopintAnalysis(int& value)
{
	numAnalysis(value);
	OUT("步长");
	OUTSYM;
}
//语句列 
void Grammar_analysis::statementsAnalysis(vector<pair<int, int>>& returnvec)
{
	while (SYMBOL_TYPE == IFTK || SYMBOL_TYPE == DOTK || SYMBOL_TYPE == WHILETK || SYMBOL_TYPE == FORTK || SYMBOL_TYPE == LBRACE || SYMBOL_TYPE == IDENFR || SYMBOL_TYPE == SCANFTK || SYMBOL_TYPE == PRINTFTK || SYMBOL_TYPE == RETURNTK || SYMBOL_TYPE == SEMICN) {
		statementAnalysis(returnvec);
		if (SYMBOL_TYPE == IFTK || SYMBOL_TYPE == DOTK || SYMBOL_TYPE == WHILETK || SYMBOL_TYPE == FORTK || SYMBOL_TYPE == LBRACE || SYMBOL_TYPE == IDENFR || SYMBOL_TYPE == SCANFTK || SYMBOL_TYPE == PRINTFTK || SYMBOL_TYPE == RETURNTK || SYMBOL_TYPE == SEMICN) {
			OUTSYM;
		}
		else {
			OUT("语句列");
			//OUTSYM;
		}
	}
}
//(无)有返回值函数调用语句 
void Grammar_analysis::funcstatementAnalysis()
{
	if (SYMBOL_TYPE == IDENFR) {
		string funcname = SYMBOL_VALUE;
		if (table.find(SYMBOL_VALUE) == false) {
			ERROUT(UNDEFINED);
		}
		int count = SymbolTable.count(SYMBOL_VALUE);
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == LPARENT) {
			NEXTSYM;
			//OUTSYM;
			vector<int> paravec;
			funcparavalueAnalysis(paravec, funcname);
			if (table.ste->paranum != paravec.size()) {
				ERROUT(WRONG_ARG_NUM);
			}
			else if (table.ste->arguments != NULL) {
				for (int i = 0; i < table.ste->paranum; i++) {
					if (table.ste->arguments->at(i).type == INTEGER) {
						if (paravec.at(i) == 0);
						else {
							ERROUT(WRONG_ARG_TYPE);
							break;
						}
					}
					else {
						if (paravec.at(i) == 1);
						else {
							ERROUT(WRONG_ARG_TYPE);
							break;
						}
					}
				}
			}
			if (SYMBOL_TYPE == RPARENT) {
				if (count == 0) {
					OUT("无返回值函数调用语句");
				}
				else {
					OUT("有返回值函数调用语句");
				}
				NEXTSYM;
				//OUTSYM;
			WITHOUTRP:
				if (SYMBOL_TYPE == SEMICN) {
					OUTSYM;
					OUT("语句");
					//OUTSYM;
					NEXTSYM;
				WITHOUTSEMI:;
					//OUTSYM;
				}
				else {
					ERROUT(SHOULD_BE_SEMICN);
					goto WITHOUTSEMI;
				}
			}
			else {
				ERROUT(SHOULD_BE_RPARENT);
				goto WITHOUTRP;
			}
		}
		else ERROR;
	}
	else ERROR;
}
//赋值语句 
void Grammar_analysis::assignstatementAnalysis()
{
	/*if (SYMBOL_TYPE == IDENFR) {
		if (table.find(SYMBOL_VALUE) == false) {
			ERROUT(UNDEFINED);
		}
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == LBRACK) {
			NEXTSYM;
			OUTSYM;
			int exprtype = 0;
			exprAnalysis(exprtype);
			if (exprtype != 0) ERROUT(ILLEGAL_ARRAYNUM);
			OUTSYM;
			if (SYMBOL_TYPE == RBRACK) {
				NEXTSYM;
				OUTSYM;
			WITHOUTRB:
				if (SYMBOL_TYPE == ASSIGN) {
					NEXTSYM;
					OUTSYM;
					exprAnalysis();
					OUT("赋值语句");
					OUTSYM;
				}
				else ERROR;
			}
			else {
				ERROUT(SHOULD_BE_RBRACK);
				goto WITHOUTRB;
			}
		}
		else if (SYMBOL_TYPE == ASSIGN) {
			NEXTSYM;
			OUTSYM;
			exprAnalysis();
			OUT("赋值语句");
			OUTSYM;
		}
		else ERROR;
	}
	else ERROR;
	*/
}
//读语句
void Grammar_analysis::scanfAnalysis()
{
	if (SYMBOL_TYPE == SCANFTK) {
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == LPARENT) {
			NEXTSYM;
			OUTSYM;
			if (SYMBOL_TYPE == IDENFR) {
				if (table.find(SYMBOL_VALUE) == false) {
					ERROUT(UNDEFINED);
				}
				string encodename = table.encodeKey();
				if (table.ste != NULL && table.ste->valueType == CHAR)	sementic_Analysis.readChar(encodename, table.position);
				else if (table.ste != NULL && table.ste->valueType == INTEGER)sementic_Analysis.readInteger(encodename, table.position);
				NEXTSYM;
				if (SYMBOL_TYPE == COMMA) {
					OUTSYM;
				}
				else {
					if (SYMBOL_TYPE == RPARENT) {
						OUTSYM;
						OUT("读语句");
						NEXTSYM;
					SCANFWITHOUTRP:
						OUTSYM;
					}
					else {
						ERROUT(SHOULD_BE_RPARENT);
						goto SCANFWITHOUTRP;
					}
				}
			}
			else ERROR;
			while (SYMBOL_TYPE == COMMA) {
				NEXTSYM;
				OUTSYM;
				if (SYMBOL_TYPE == IDENFR) {
					if (table.find(SYMBOL_VALUE) == false) {
						ERROUT(UNDEFINED);
					}
					string encodename = table.encodeKey();
					if (table.ste != NULL && table.ste->valueType == CHAR)	sementic_Analysis.readChar(encodename, table.position);
					else if (table.ste != NULL && table.ste->valueType == INTEGER)sementic_Analysis.readInteger(encodename, table.position);
					NEXTSYM;
					if (SYMBOL_TYPE == COMMA) {
						OUTSYM;
					}
					else {
						if (SYMBOL_TYPE == RPARENT) {
							OUTSYM;
							OUT("读语句");
							NEXTSYM;
						SCANWITHOUTRP:
							OUTSYM;
						}
						else {
							ERROUT(SHOULD_BE_RPARENT);
							goto SCANWITHOUTRP;
						}
					}
				}
			}
		}
		else ERROR;
	}
	else ERROR;
}
//写语句
void Grammar_analysis::printfAnalysis()
{
	int filltype = 0;
	string stringname;
	string value;
	string var1;
	string var2;
	int sint = 0;

	if (SYMBOL_TYPE == PRINTFTK) {
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == LPARENT) {
			NEXTSYM;
			OUTSYM;
			if (SYMBOL_TYPE == STRCON) {
				OUT("字符串");
				stringname = SYMBOL_VALUE;
				sint = table.addString(SYMBOL_VALUE);
				var1 = SYMBOL_VALUE;
				//cout << "((((" << var1 << endl;
				//sementic_Analysis.writeString(sementic_Analysis.stringDefine(stringname, table.position), table.position);
				NEXTSYM;
				OUTSYM;
				if (SYMBOL_TYPE == COMMA) {
					NEXTSYM;
					OUTSYM;
					exprAnalysis(filltype, value);
					var2 = value;
					if (filltype == 0) {
						sementic_Analysis.writeInteger(value, table.position);
					}
					else if (filltype == 1) {
						sementic_Analysis.writeChar(value, table.position);
					}
					OUTSYM;
				}
				else if (SYMBOL_TYPE == RPARENT) {
				}
				else {
					ERROUT(SHOULD_BE_RPARENT);
					goto WITHOUTRP2;
				}
			}
			else if (SYMBOL_TYPE == ILLEGAL) {
				//cout << SYMBOL_TYPE << endl;
				goto END;
			}
			else {
				exprAnalysis(filltype, value);
				var2 = value;
				if (filltype == 0) {
					sementic_Analysis.writeInteger(value, table.position);
				}
				else if (filltype == 1) {
					sementic_Analysis.writeChar(value, table.position);
				}
				OUTSYM;
			}
			cout << filltype << " " << value << endl;
			if (filltype == 0) {
				//ADDMIDCODE("prt", to_string(var1), var2, "int");
				ADDMIDCODE("prt", to_string(sint), var2, "int");
			}
			else {
				//ADDMIDCODE("prt", to_string(var1), var2, "char");
				ADDMIDCODE("prt", to_string(sint), var2, "char");
			}
			if (SYMBOL_TYPE == RPARENT) {
				OUT("写语句");
				NEXTSYM;
			WITHOUTRP2:
				OUTSYM;
			}
			else {
				ERROUT(SHOULD_BE_RPARENT);
				goto WITHOUTRP2;
			}

		}
		else ERROR;
	}
	else ERROR;
	sementic_Analysis.newLine();
	return;
END:;
	SYMBOL_TYPE = SEMICN;
}
//返回语句
void Grammar_analysis::returnAnalysis(vector<pair<int, int>>& returnvec)
{
	string value;
	if (SYMBOL_TYPE == RETURNTK) {
		NEXTSYM;
		if (SYMBOL_TYPE == LPARENT) {
			OUTSYM;
			NEXTSYM;
			OUTSYM;
			int exprtype = 0;
			exprAnalysis(exprtype, value);
			ADDMIDCODE("ret", "", "", value);
			sementic_Analysis.funcreturnvalue(value, table.position);
			returnvec.push_back(pair<int, int>(lexical_analysis.lineNumber, exprtype));
			OUTSYM;
			if (SYMBOL_TYPE == RPARENT) {
				NEXTSYM;
			WITHOUTRP:
				OUT("返回语句");
				OUTSYM;
			}
			else {
				ERROUT(SHOULD_BE_RPARENT);
				goto WITHOUTRP;
			}
		}
		else if (SYMBOL_TYPE == SEMICN) {
			returnvec.push_back(pair<int, int>(lexical_analysis.lineNumber, -1));
			OUT("返回语句");
			ADDMIDCODE("ret", "", "", "");
			OUTSYM;
		}
		if (SYMBOL_TYPE != SEMICN) ERROUT(SHOULD_BE_SEMICN);
	}
	else ERROR;
}
//语句
void Grammar_analysis::statementAnalysis(vector<pair<int, int>>& returnvec)
{
	string value;
	int filltype;
	if (SYMBOL_TYPE == IFTK) {
		//OUTSYM;
		ifstatementAnalysis(returnvec);
		OUT("语句");
		//OUTSYM;
	}
	else if (SYMBOL_TYPE == DOTK || SYMBOL_TYPE == WHILETK || SYMBOL_TYPE == FORTK) {
		//OUTSYM;
		loopstatementAnalysis(returnvec);
		OUT("语句");
		//OUTSYM;
	}
	else if (SYMBOL_TYPE == LBRACE) {
		NEXTSYM;
		if (SYMBOL_TYPE == RBRACE) {
			OUT("语句列");
			OUTSYM;
			OUT("语句");
			NEXTSYM;
		}
		else {
			//OUTSYM;
			//NEXTSYM;
			OUTSYM;
			statementsAnalysis(returnvec);
			if (SYMBOL_TYPE == RBRACE) {
				//OUT("语句列");
				OUTSYM;
				OUT("语句");
				NEXTSYM;
				//OUTSYM;
			}
			else ERROR;
		}
	}
	else if (SYMBOL_TYPE == IDENFR) {
		if (table.find(SYMBOL_VALUE) == false) {
			ERROUT(UNDEFINED);
		}
		string paraname = SYMBOL_VALUE;
		string funcname = SYMBOL_VALUE;
		string encodename = table.encodeKey();
		//OUTSYM;
		int count = SymbolTable.count(SYMBOL_VALUE);
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == LPARENT) {
			NEXTSYM;
			//OUTSYM;
			vector<int> paravec;
			funcparavalueAnalysis(paravec, funcname);
			if (table.find(funcname) == true) {
				if (table.ste->paranum != paravec.size()) {
					ERROUT(WRONG_ARG_NUM);
				}
				else if (table.ste->arguments != NULL) {
					for (int i = 0; i < table.ste->paranum; i++) {
						if (table.ste->arguments->at(i).type == INTEGER) {
							if (paravec.at(i) == 0);
							else {
								ERROUT(WRONG_ARG_TYPE);
								break;
							}
						}
						else {
							if (paravec.at(i) == 1);
							else {
								ERROUT(WRONG_ARG_TYPE);
								break;
							}
						}
					}
				}
				//sementic_Analysis.jumpAnyway(table.encodeKey(), table.position);
				sementic_Analysis.callfunc(funcname, value, table.position);
			}
			if (SYMBOL_TYPE == RPARENT) {
				if (count == 0) {
					OUT("无返回值函数调用语句");
				}
				else {
					OUT("有返回值函数调用语句");
				}
				NEXTSYM;
			WITHOUTRP:
				if (SYMBOL_TYPE == SEMICN) {
					OUTSYM;
					OUT("语句");
					//OUTSYM;
					NEXTSYM;
					//OUTSYM;
				}
				else {
					ERROUT(SHOULD_BE_SEMICN);
				}
				//OUTSYM;
			}
			else {
				ERROUT(SHOULD_BE_RPARENT);
				goto WITHOUTRP;
			}
		}
		else if (SYMBOL_TYPE == LBRACK) {
			if (table.find(paraname) == true) {
				if (table.ste->type == CONST) {
					ERROUT(CONST_CHANGE);
				}
			}
			NEXTSYM;
			OUTSYM;
			int exprtype = 0;
			exprAnalysis(exprtype, value);
			string offset = value;
			if (exprtype != 0) ERROUT(ILLEGAL_ARRAYNUM);
			OUTSYM;
			if (SYMBOL_TYPE == RBRACK) {
				NEXTSYM;
				OUTSYM;
			WITHOUTRB:
				if (SYMBOL_TYPE == ASSIGN) {
					NEXTSYM;
					OUTSYM;
					exprAnalysis(filltype, value);
					sementic_Analysis.assignToArray(encodename, offset, value, table.position);
					OUT("赋值语句");
					OUTSYM;
					if (SYMBOL_TYPE == SEMICN) {
						OUT("语句");
						//OUTSYM;
						NEXTSYM;
						//OUTSYM;
					}
					else {
						ERROUT(SHOULD_BE_SEMICN);
					}

				}
				else ERROR;
			}
			else {
				ERROUT(SHOULD_BE_RBRACK);
				goto WITHOUTRB;
			}
		}
		else if (SYMBOL_TYPE == ASSIGN) {
			if (table.find(paraname) == true) {
				if (table.ste->type == CONST) {
					ERROUT(CONST_CHANGE);
				}
			}
			NEXTSYM;
			OUTSYM;
			exprAnalysis(filltype, value);
			sementic_Analysis.assign(encodename, value, table.position);
			OUT("赋值语句");
			OUTSYM;
			if (SYMBOL_TYPE == SEMICN) {
				OUT("语句");
				//OUTSYM;
				NEXTSYM;
				//OUTSYM;
			}
			else {
				ERROUT(SHOULD_BE_SEMICN);
			}
		}
		else ERROR;

	}
	else if (SYMBOL_TYPE == SCANFTK) {
		//OUTSYM;
		scanfAnalysis();
		if (SYMBOL_TYPE == SEMICN) {
			//OUTSYM;
			OUT("语句");
			//OUTSYM;
			NEXTSYM;
			//OUTSYM;
		}
		else {
			ERROUT(SHOULD_BE_SEMICN);
		}
	}
	else if (SYMBOL_TYPE == PRINTFTK) {
		//OUTSYM;
		printfAnalysis();
		if (SYMBOL_TYPE == SEMICN) {
			//OUTSYM;
			OUT("语句");

			NEXTSYM;
			//OUTSYM;
		}
		else {
			ERROUT(SHOULD_BE_SEMICN);
		}
	}
	else if (SYMBOL_TYPE == RETURNTK) {
		//OUTSYM;
		returnAnalysis(returnvec);
		if (SYMBOL_TYPE == SEMICN) {
			//OUTSYM;
			OUT("语句");
			//OUTSYM;
			NEXTSYM;
			//OUTSYM;
		}
		//else ERROUT(SHOULD_BE_SEMICN);
	}
	else if (SYMBOL_TYPE == SEMICN) {
		//OUTSYM;
		OUT("语句");
		NEXTSYM;
	WITHOUTSEMI:
		if (SYMBOL_TYPE == SEMICN) {
			OUTSYM;
			statementAnalysis(returnvec);
		}
	}
	else {
		ERROUT(SHOULD_BE_SEMICN);
		goto WITHOUTSEMI;
	}
}

//有返回值函数定义
void Grammar_analysis::funcdefinewithReturnAnalysis()
{
	bool redefine = false;
	string funcname;
	vector<Argument> args;
	BasicType type;
	nameAnalysis(funcname, type);
	cout << "***" << type << endl;
	vector<pair<int, int>> returnvec;
	if (table.isHere(funcname)) {
		ERROUT(REDEFINE);
		redefine = true;
		skip();
		goto END;
	}
	if (SYMBOL_TYPE == LPARENT) {
		NEXTSYM;
		//OUTSYM;
		funcparaAnalysis(args);
		if (!redefine)	sementic_Analysis.functionDefine(funcname, args, type);
		if (SYMBOL_TYPE == RPARENT) {
			NEXTSYM;
			OUTSYM;
		WITHOUTRP:
			if (SYMBOL_TYPE == LBRACE) {
				NEXTSYM;
				//OUTSYM;
				complexstatementAnalysis(funcname, returnvec);
				if (returnvec.size() == 1 && returnvec[0].second == -2) {
					ERROUT(ILLEGAL_FUNCRETURN, returnvec[0].first);
				}
				else if (returnvec.size() > 1) {
					int funcvaluetype = (type == INTEGER) ? 0 : (type == CHAR) ? 1 : -1;
					for (int i = 0; i < returnvec.size() - 1; i++) {
						if (returnvec.at(i).second != funcvaluetype) {
							ERROUT(ILLEGAL_FUNCRETURN, returnvec.at(i).first);
						}
					}
				}
				if (SYMBOL_TYPE == RBRACE) {
				END:
					NEXTSYM;
					OUT("有返回值函数定义");
					OUTSYM;
				}
				else ERROR;
			}
			else ERROR;
		}
		else {
			ERROUT(SHOULD_BE_RPARENT);
			goto WITHOUTRP;
		}
	}
	else ERROR;
}
//无返回值函数定义
void Grammar_analysis::funcdefinewithoutReturnAnalysis()
{
	bool redefine = false;
	string funcname;
	vector<Argument> args;
	BasicType type = VOID;
	vector<pair<int, int>> returnvec;
	if (SYMBOL_TYPE == VOIDTK) {
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == IDENFR) {
			funcname = SYMBOL_VALUE;
			if (table.isHere(funcname)) {
				ERROUT(REDEFINE);
				redefine = true;
				skip();
				goto END;
			}
			NEXTSYM;
			OUTSYM;
			if (SYMBOL_TYPE == LPARENT) {
				NEXTSYM;
				//OUTSYM;
				funcparaAnalysis(args);
				if (!redefine)	sementic_Analysis.functionDefine(funcname, args, type);
				if (SYMBOL_TYPE == RPARENT) {
					NEXTSYM;
					OUTSYM;
				WITHOUTRP:
					if (SYMBOL_TYPE == LBRACE) {
						NEXTSYM;
						//OUTSYM;
						complexstatementAnalysis(funcname, returnvec);
						if (returnvec.size() > 1) {
							int funcvaluetype = -1;
							for (int i = 0; i < returnvec.size() - 1; i++) {
								if (returnvec.at(i).second != funcvaluetype) {
									ERROUT(ILLEGAL_VOIDFUNCRETURN, returnvec.at(i).first);
								}
							}
						}
						if (SYMBOL_TYPE == RBRACE) {
						END:
							NEXTSYM;
							OUT("无返回值函数定义");
							OUTSYM;
						}
						else ERROR;
					}
					else ERROR;
				}
				else {
					ERROUT(SHOULD_BE_RPARENT);
					goto WITHOUTRP;
				}
			}
			else ERROR;
		}
		else ERROR;
	}
	else ERROR;
}
//复合语句
void Grammar_analysis::complexstatementAnalysis(string funcname, vector<pair<int, int>>& returnvec)
{
	if (SYMBOL_TYPE == RBRACE) {
		table.goIn(funcname);
		sementic_Analysis.funcStart();
		int tmpcount = sementic_Analysis.getTemporaryCount();
		tmpcount = sementic_Analysis.getTemporaryCount() - tmpcount;
		table.setTempcount(tmpcount);
		sementic_Analysis.funcReturn();
		ADDMIDCODE("end", "", "", funcname);
		table.goOut();
		OUT("语句列");
		OUT("复合语句");
		OUTSYM;
	}
	else {
		table.goIn(funcname);
		sementic_Analysis.funcStart();
		int tmpcount = sementic_Analysis.getTemporaryCount();
		OUTSYM;
		if (SYMBOL_TYPE == CONSTTK) constlistAnalysis();
		if (SYMBOL_TYPE == CHARTK || SYMBOL_TYPE == INTTK)	varlistAnalysis();
		statementsAnalysis(returnvec);
		returnvec.push_back(pair<int, int>(lexical_analysis.lineNumber, -2));
		OUT("复合语句");
		OUTSYM;
		tmpcount = sementic_Analysis.getTemporaryCount() - tmpcount;
		table.setTempcount(tmpcount);
		sementic_Analysis.funcReturn();
		ADDMIDCODE("end", "", "", funcname);
		table.goOut();
	}
}

//参数表
void Grammar_analysis::funcparaAnalysis(vector<Argument>& args)
{
	bool redefine = false;
	string name;
	BasicType type;
	bool vary = false;
	args.clear();
	vector<string> names;
	if (SYMBOL_TYPE == INTTK || SYMBOL_TYPE == CHARTK) {
		type = (SYMBOL_TYPE == CHARTK) ? CHAR : INTEGER;
		OUTSYM;
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == IDENFR) {
			name = SYMBOL_VALUE;
			if (!searchVector(names, name)) {
				names.push_back(name);
			}
			else {
				ERROUT(REDEFINE);
				redefine = true;
			}
			NEXTSYM;
			if (SYMBOL_TYPE == COMMA) {
				OUTSYM;
				Argument tmp = Argument(name, type, vary);
				if (!redefine)	args.push_back(tmp);
			}
			else {
				OUT("参数表");
				OUTSYM;
				Argument tmp = Argument(name, type, vary);
				if (!redefine)	args.push_back(tmp);
			}

		}
		else ERROR;
	}
	else if (SYMBOL_TYPE == RPARENT) {
		OUT("参数表");
		OUTSYM;
	}
	else ERROR;
	while (SYMBOL_TYPE == COMMA) {
		redefine = false;
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == INTTK || SYMBOL_TYPE == CHARTK) {
			type = (SYMBOL_TYPE == CHARTK) ? CHAR : INTEGER;
			NEXTSYM;
			OUTSYM;
			if (SYMBOL_TYPE == IDENFR) {
				name = SYMBOL_VALUE;
				if (!searchVector(names, name)) {
					names.push_back(name);
				}
				else {
					ERROUT(REDEFINE);
					redefine = true;
				}
				NEXTSYM;
				if (SYMBOL_TYPE == COMMA) {
					OUTSYM;
					Argument tmp = Argument(name, type, vary);
					if (!redefine)	args.push_back(tmp);
				}
				else {
					OUT("参数表");
					OUTSYM;
					Argument tmp = Argument(name, type, vary);
					if (!redefine)	args.push_back(tmp);
				}
			}
			else ERROR;
		}
		else ERROR;
	}
}
//主函数
void Grammar_analysis::mainAnalysis()
{
	vector<pair<int, int>> returnvec;
	if (SYMBOL_TYPE == VOIDTK) {
		NEXTSYM;
		OUTSYM;
		if (SYMBOL_TYPE == MAINTK) {
			NEXTSYM;
			OUTSYM;
			if (SYMBOL_TYPE == LPARENT) {
				NEXTSYM;
				OUTSYM;
				if (SYMBOL_TYPE == RPARENT) {
					NEXTSYM;
					OUTSYM;
				WITHOUTRP:
					if (SYMBOL_TYPE == LBRACE) {
						NEXTSYM;
						//OUTSYM;
						complexstatementAnalysis("main", returnvec);
						if (returnvec.size() > 1) {
							int funcvaluetype = -1;
							for (int i = 0; i < returnvec.size() - 1; i++) {
								if (returnvec.at(i).second != funcvaluetype) {
									ERROUT(ILLEGAL_VOIDFUNCRETURN, returnvec.at(i).first);
								}
							}
						}
						if (SYMBOL_TYPE == RBRACE) {
							NEXTSYM;
							OUT("主函数");
							OUTSYM;
						}
						else ERROR;
					}
					else ERROR;
				}
				else {
					ERROUT(SHOULD_BE_RPARENT);
					goto WITHOUTRP;
				}
			}
			else ERROR;
		}
		else ERROR;
	}
	else ERROR;
}
//程序

void Grammar_analysis::programAnalysis()
{
	string name;
	int limit;
	BasicType type;
	bool redefine = false;
	string funcname;
	vector<Argument> args;
	BasicType functype;
	vector<pair<int, int>> returnvec;
	if (SYMBOL_TYPE == CONSTTK) {
		constlistAnalysis();
	}
	while (SYMBOL_TYPE == INTTK || SYMBOL_TYPE == CHARTK || SYMBOL_TYPE == VOIDTK) {
		while (SYMBOL_TYPE == INTTK || SYMBOL_TYPE == CHARTK) {
			type = (SYMBOL_TYPE == CHARTK) ? CHAR : INTEGER;
			functype = (SYMBOL_TYPE == CHARTK) ? CHAR : INTEGER;
			int tmptype = 1;
			NEXTSYM;
			SymbolType pre1 = SYMBOL_TYPE;
			string pre1str = SYMBOL_VALUE;
			NEXTSYM;
			SymbolType pre2 = SYMBOL_TYPE;
			string pre2str = SYMBOL_VALUE;
		part1:
			redefine = false;
			if (pre2 == LPARENT) {
				fout << pre1 << ' ' << pre1str << endl;
				funcname = pre1str;
				if (table.isHere(funcname)) {
					ERROUT(REDEFINE);
					redefine = true;
					skip();
					goto END;
				}
				SymbolTable.insert(pair<string, int>(pre1str, tmptype));
				OUT("声明头部");
				fout << pre2 << ' ' << pre2str << endl;

				NEXTSYM;
				//OUTSYM;
				funcparaAnalysis(args);
				if (!redefine)	sementic_Analysis.functionDefine(funcname, args, functype);
				if (SYMBOL_TYPE == RPARENT) {
					NEXTSYM;
					OUTSYM;
				WITHOUTRP:
					if (SYMBOL_TYPE == LBRACE) {
						NEXTSYM;
						//OUTSYM;
						returnvec.clear();
						complexstatementAnalysis(funcname, returnvec);
						if (returnvec.size() == 1 && returnvec[0].second == -2) {
							ERROUT(ILLEGAL_FUNCRETURN, returnvec[0].first);
						}
						else if (returnvec.size() > 1) {
							int funcvaluetype = (functype == INTEGER) ? 0 : (type == CHAR) ? 1 : -1;
							for (int i = 0; i < returnvec.size() - 1; i++) {
								if (returnvec.at(i).second != funcvaluetype) {
									ERROUT(ILLEGAL_FUNCRETURN, returnvec.at(i).first);
								}
							}
						}
						if (SYMBOL_TYPE == RBRACE) {
						END:
							NEXTSYM;
							OUT("有返回值函数定义");
							OUTSYM;
						}
						else ERROR;
					}
					else ERROR;
				}
				else {
					ERROUT(SHOULD_BE_RPARENT);
					goto WITHOUTRP;
				}
			}
			else {
			part2:
				redefine = false;
				fout << pre1 << ' ' << pre1str << endl;
				name = pre1str;
				if (table.isHere(name)) {
					ERROUT(REDEFINE);
					redefine = true;
				}
				//fout << pre2 << ' ' << pre2str << endl;
				if (pre2 == LBRACK) {
					OUTSYM;
					NEXTSYM;
					OUTSYM;
					numAnalysis(limit);
					if (!redefine)	sementic_Analysis.arrayDefine(name, type, limit);
					OUTSYM;
					if (SYMBOL_TYPE == RBRACK) {
						NEXTSYM;
					WITHOUTRB:
						if (SYMBOL_TYPE == COMMA) {
							OUTSYM;
						}
						else {
							OUT("变量定义");
							OUTSYM;
						}

					}
					else {
						ERROUT(SHOULD_BE_RBRACK);
						goto WITHOUTRB;
					}
				}
				else {
					if (pre2 == COMMA) {
						//OUTSYM;
						if (!redefine)	sementic_Analysis.varDefine(name, type);
					}
					else {
						OUT("变量定义");
						//OUTSYM;
						if (!redefine)	sementic_Analysis.varDefine(name, type);
					}
					OUTSYM;
				}
				while (SYMBOL_TYPE == COMMA) {
					redefine = false;
					NEXTSYM;
					OUTSYM;
					if (SYMBOL_TYPE == IDENFR) {
						name = SYMBOL_VALUE;
						if (table.isHere(name)) {
							ERROUT(REDEFINE);
							redefine = true;
						}
						NEXTSYM;
						if (SYMBOL_TYPE == LBRACK) {
							OUTSYM;
							NEXTSYM;
							OUTSYM;
							numAnalysis(limit);
							if (!redefine)	sementic_Analysis.arrayDefine(name, type, limit);
							OUTSYM;
							if (SYMBOL_TYPE == RBRACK) {
								NEXTSYM;
							WITHOUTRB1:
								if (SYMBOL_TYPE == COMMA) {
									OUTSYM;
								}
								else {
									OUT("变量定义");
									OUTSYM;
								}


							}
							else {
								ERROUT(SHOULD_BE_RBRACK);
								goto WITHOUTRB1;
							}
						}
						else {
							if (SYMBOL_TYPE == COMMA) {
								//OUTSYM;
								if (!redefine)	sementic_Analysis.varDefine(name, type);
							}
							else {
								OUT("变量定义");
								if (!redefine)	sementic_Analysis.varDefine(name, type);
							}
							OUTSYM;
						}
					}
					else ERROR;
				}
				if (SYMBOL_TYPE == SEMICN) {
					NEXTSYM;
				WITHOUTSEMI:
					SymbolType save = SYMBOL_TYPE;
					string savestr = SYMBOL_VALUE;
					if (SYMBOL_TYPE == INTTK || SYMBOL_TYPE == CHARTK) {
						type = (SYMBOL_TYPE == CHARTK) ? CHAR : INTEGER;
						functype = (SYMBOL_TYPE == CHARTK) ? CHAR : INTEGER;
						NEXTSYM;
						pre1 = SYMBOL_TYPE;
						pre1str = SYMBOL_VALUE;
						NEXTSYM;
						pre2 = SYMBOL_TYPE;
						pre2str = SYMBOL_VALUE;
						if (pre2 == LPARENT) {
							OUT("变量说明");
							fout << save << ' ' << savestr << endl;
							//OUTSYM;
							goto part1;

						}
						else {
							//OUTSYM;
							fout << save << ' ' << savestr << endl;
							goto part2;
						}
					}
					else {
						OUT("变量说明");
						OUTSYM;
					}

				}
				else {
					ERROUT(SHOULD_BE_SEMICN);	goto WITHOUTSEMI;
				}
			}

		}
		while (SYMBOL_TYPE == VOIDTK) {
			redefine = false;
			NEXTSYM;
			SymbolType pre1 = SYMBOL_TYPE;
			string pre1str = SYMBOL_VALUE;
			if (pre1 == IDENFR) {
				funcname = pre1str;
				functype = VOID;
				if (table.isHere(funcname)) {
					ERROUT(REDEFINE);
					redefine = true;
					skip();
					goto WFEND;
				}
				fout << pre1 << ' ' << pre1str << endl;
				NEXTSYM;
				OUTSYM;
				if (SYMBOL_TYPE == LPARENT) {
					NEXTSYM;
					//OUTSYM;
					funcparaAnalysis(args);
					if (!redefine)	sementic_Analysis.functionDefine(funcname, args, VOID);
					if (SYMBOL_TYPE == RPARENT) {
						NEXTSYM;
						OUTSYM;
					FUNCWITHOUTRP:
						if (SYMBOL_TYPE == LBRACE) {
							NEXTSYM;
							//OUTSYM;
							returnvec.clear();
							complexstatementAnalysis(funcname, returnvec);
							if (returnvec.size() > 1) {
								int funcvaluetype = -1;
								for (int i = 0; i < returnvec.size() - 1; i++) {
									if (returnvec.at(i).second != funcvaluetype) {
										ERROUT(ILLEGAL_VOIDFUNCRETURN, returnvec.at(i).first);
									}
								}
							}
							if (SYMBOL_TYPE == RBRACE) {
							WFEND:
								NEXTSYM;
								OUT("无返回值函数定义");
								OUTSYM;
							}
							else ERROR;
						}
						else ERROR;
					}
					else {
						ERROUT(SHOULD_BE_RPARENT);
						goto FUNCWITHOUTRP;
					}
				}
				else ERROR;
			}
			else if (pre1 == MAINTK) {
				funcname = "main";
				bool redefine = table.isHere(funcname);
				if (redefine) {
					skip();
					goto MAINEND;
				}
				functype = VOID;
				fout << pre1 << ' ' << pre1str << endl;
				NEXTSYM;
				OUTSYM;
				if (SYMBOL_TYPE == LPARENT) {
					NEXTSYM;
					OUTSYM;
					//funcparaAnalysis();
					args.clear();
					if (!redefine)	sementic_Analysis.functionDefine(funcname, args, functype);
					if (SYMBOL_TYPE == RPARENT) {
						//OUTSYM;
						NEXTSYM;
						OUTSYM;
					MAINWITHOUTRP:
						if (SYMBOL_TYPE == LBRACE) {
							NEXTSYM;
							//OUTSYM;
							returnvec.clear();
							complexstatementAnalysis("main", returnvec);
							if (returnvec.size() > 1) {
								int funcvaluetype = -1;
								for (int i = 0; i < returnvec.size() - 1; i++) {
									if (returnvec.at(i).second != funcvaluetype) {
										ERROUT(ILLEGAL_VOIDFUNCRETURN, returnvec.at(i).first);
									}
								}
							}
							if (SYMBOL_TYPE == RBRACE) {
							MAINEND:
								//NEXTSYM;
								OUT("主函数");
								//OUTSYM;
							}
							else ERROR;
						}
						else ERROR;
					}
					else {
						ERROUT(SHOULD_BE_RPARENT);
						goto MAINWITHOUTRP;
					}
				}
				else ERROR;
			}
			else ERROR;
		}
	}
	OUT("程序");
}

void Grammar_analysis::skip()
{
	while (SYMBOL_TYPE != LBRACE) {
		NEXTSYM;
	}
	int l = 1;
	while (l != 0) {
		NEXTSYM;
		if (SYMBOL_TYPE == LBRACE)	l++;
		else if (SYMBOL_TYPE == RBRACE) l--;
	}
}

void Grammar_analysis::skiptocs()
{
	while (SYMBOL_TYPE != COMMA && SYMBOL_TYPE != SEMICN) {
		NEXTSYM;
	}
}



bool Grammar_analysis::searchVector(vector<string> names, string name)
{
	for (vector<string>::iterator it = names.begin(); it != names.end(); it++) {
		if (*it == name) {
			return true;
		}
	}
	return false;
}
