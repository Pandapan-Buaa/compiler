#include "Sementic_Analysis.h"

#include <iostream>
using namespace std;
#define ADDSET(st)  errorHandler.addEndset(SymbolType::st)
#define DELSET      errorHandler.removeEndset()
#define SYMBOL_NUMBER    (lexical_analysis.number)   
#define SYMBOL_VALUE	(lexical_analysis.save)   
#define SYMBOL_TYPE     (lexical_analysis.symbolType) 
#define NEXTSYM lexical_analysis.nextSymbol()
#define ERROUT  errorHandler.printError

#define ADDMIDCODE generator.addmidcode


Sementic_Analysis& Sementic_Analysis::initSementic(ErrorHandler& eh, SymbolTable_Analysis& st, Lexical_analysis& la, Quaternions& q, ofstream& midcode, Code_Generator& generator)
{
	static Sementic_Analysis theSementic_Analysis(eh, st, la, q, midcode, generator);
	return theSementic_Analysis;
}

//产生变量
string Sementic_Analysis::newTemporary()
{
	temporaryCount++;
	return stringPlusInt("t_", temporaryCount);
}

Sementic_Analysis::Sementic_Analysis(ErrorHandler& eh, SymbolTable_Analysis& st, Lexical_analysis& la, Quaternions& q, ofstream& midcode, Code_Generator& generator) :errorHandler(eh), table(st), lexical_analysis(la), quaternions(q), midcode(midcode), generator(generator) {
	temporaryCount = 0;
	labelCount = 0;

	varcnt = 0;
	labelcnt = 0;
}





//常量填表
void Sementic_Analysis::constDefine(const string& id, BasicType type, int value)
{
#ifdef OUTST
	if (type == CHAR)	cout << table.p << " " << id << " " << type << " " << (char)value << endl;
	else cout << table.p << " " << id << " " << type << " " << value << endl;
#endif
	//if (type == INTEGER)	midcode << "const int " << id << " = " << value << endl;
	//else if (type == CHAR) midcode << "const char " << id << " = " << (char)value << endl;
	table.ste = new STE;
	table.ste->name = id;
	table.ste->type = CONST;
	table.ste->valueType = type;
	table.ste->multiInt = value;
	table.insert();
	string a = "";
	a.push_back((char)value);
	if (type == INTEGER)	ADDMIDCODE("const", "int", to_string(value), id);
	else ADDMIDCODE("const", "char", a, id);
}
//变量填表 变量
void Sementic_Analysis::varDefine(const string& id, BasicType bType)
{
#ifdef OUTST
	cout << table.p << " " << bType << " " << VAR << " " << id << endl;
#endif
	//if (bType == INTEGER)	midcode << "var int " << id << endl;
	//else if (bType == CHAR) midcode << "var char " << id << endl;
	table.ste = new STE;
	table.ste->name = id;
	table.ste->type = VAR;
	table.ste->valueType = bType;
	table.insert();
	if (bType == INTEGER)	ADDMIDCODE("int", "", "", id);
	else ADDMIDCODE("char", "", "", id);
}
//变量填表 数组
void Sementic_Analysis::arrayDefine(const string& id, BasicType bType, int limit)
{
#ifdef OUTST
	cout << table.p << " " << bType << " " << ARRAY << " " << id << " " << limit << endl;
#endif
	//if (bType == INTEGER)	midcode << "var int " << id << "[" << limit << "]" << endl;
	//else if (bType == CHAR) midcode << "var char " << id << "[" << limit << "]" << endl;
	table.ste = new STE;
	table.ste->name = id;
	table.ste->type = ARRAY;
	table.ste->valueType = bType;
	table.ste->multiInt = limit;
	table.insert();
	if (bType == INTEGER)	ADDMIDCODE("ints", "", to_string(limit), id);
	else ADDMIDCODE("chars", "", to_string(limit), id);
}
//函数填表
void Sementic_Analysis::functionDefine(const string& funcname, vector<Argument>& args, BasicType type)
{
#ifdef OUTST
	cout << table.p << " " << type << " " << FUNCTION << " " << funcname << " " << endl;
	for (vector<Argument>::iterator it = args.begin(); it != args.end(); it++) {
		cout << table.p + 1 << " " << funcname << "'s para" << " " << it->type << " " << it->name << endl;
	}

#endif
	/*if (type == INTEGER) {
		midcode << "int " << funcname << "()" << endl;
		for (vector<Argument>::iterator it = args.begin(); it != args.end(); it++) {
			if ((*it).type == INTEGER)	midcode << "para int " << (*it).name << endl;
			else if ((*it).type == CHAR)midcode << "para char " << (*it).name << endl;
		}
	}
	else if (type == CHAR) {
		midcode << "char " << funcname << "()" << endl;
		for (vector<Argument>::iterator it = args.begin(); it != args.end(); it++) {
			if ((*it).type == INTEGER)	midcode << "para int " << (*it).name << endl;
			else if ((*it).type == CHAR)midcode << "para char " << (*it).name << endl;
		}
	}
	else if (type == VOID) {
		midcode << "void " << funcname << "()" << endl;
		for (vector<Argument>::iterator it = args.begin(); it != args.end(); it++) {
			if ((*it).type == INTEGER)	midcode << "para int " << (*it).name << endl;
			else if ((*it).type == CHAR)midcode << "para char " << (*it).name << endl;
		}
	}*/
	table.ste = new STE;
	table.ste->name = funcname;
	table.ste->type = FUNCTION;
	table.ste->valueType = type;
	table.ste->paranum = 0;
	if (!args.empty()) {
		table.defineArguments(table.ste, args);

	}
#ifdef OUTST
	if (table.ste->arguments != NULL) {
		for (vector<Argument>::iterator it = (*table.ste->arguments).begin(); it != (*table.ste->arguments).end(); it++) {
			cout << "###" << (*it).name << " " << (*it).type << endl;
		}
	}
#endif
	if (type == INTEGER)	ADDMIDCODE("func", "int", "", funcname);
	else if (type == CHAR)	ADDMIDCODE("func", "char", "", funcname);
	else if (type == VOID)	ADDMIDCODE("func", "void", "", funcname);
	if (table.ste->arguments != NULL) {
		for (vector<Argument>::iterator it = (*table.ste->arguments).begin(); it != (*table.ste->arguments).end(); it++) {
			if (it->type == INTEGER)	ADDMIDCODE("para", "int", "", it->name);
			else if (it->type == CHAR) ADDMIDCODE("para", "char", "", it->name);
		}
	}
	table.insert();
}

void Sementic_Analysis::intToString(int n, string& str)
{
	str = stringPlusInt("", n);
}
void Sementic_Analysis::charToString(char n, string& str)
{
	str = stringPlusInt("", n);
}

string Sementic_Analysis::stringPlusInt(const string& str, int n)
{
	string newstr;
	char num[20];
	newstr = str;
	sprintf(num, "%d", n);
	newstr.append(num);
	return newstr;
}

void Sementic_Analysis::negative(const string& oper1, string& dest, STE* ste)
{
	//dest = newTemporary();
	//quaternions.addTail(new Quaternion(Operator::NEG, oper1, "", dest, ste));
	//dest = newvar();
	//ADDMIDCODE("", "var1", "var2", dest);
}

void Sementic_Analysis::addition(const string& oper1, const string& oper2, string& dest, STE* ste)
{
	//dest = newTemporary();
	//quaternions.addTail(new Quaternion(Operator::PLUS, oper1, oper2, dest, ste));
	dest = newvar();
	ADDMIDCODE("plus", oper1, oper2, dest);
}

void Sementic_Analysis::subtraction(const string& oper1, const string& oper2, string& dest, STE* ste)
{
	//dest = newTemporary();
	//quaternions.addTail(new Quaternion(Operator::MINU, oper1, oper2, dest, ste));

	dest = newvar();
	ADDMIDCODE("minus", oper1, oper2, dest);
}

void Sementic_Analysis::multiplicatioin(const string& oper1, const string& oper2, string& dest, STE* ste)
{
	//dest = newTemporary();
	//quaternions.addTail(new Quaternion(Operator::MULT, oper1, oper2, dest, ste));

	dest = newvar();
	ADDMIDCODE("mul", oper1, oper2, dest);
}

void Sementic_Analysis::division(const string& oper1, const string& oper2, string& dest, STE* ste)
{
	//dest = newTemporary();
	//quaternions.addTail(new Quaternion(Operator::DIV, oper1, oper2, dest, ste));
	dest = newvar();
	ADDMIDCODE("div", oper1, oper2, dest);
}

void Sementic_Analysis::callfunc(const string& name, string& value, STE* ste)
{
	//value = newTemporary();
	//quaternions.addTail(new Quaternion(Operator::CALF, name, "", value, ste));

	value = newvar();
	ADDMIDCODE("call", name, "", value);
}

void Sementic_Analysis::callfuncreturn(string& value, STE* ste)
{
	quaternions.addTail(new Quaternion(Operator::RET, "", "", value, ste));
}

void Sementic_Analysis::funcreturnvalue(string& value, STE* ste)
{
	quaternions.addTail(new Quaternion(Operator::FRET, "", "", value, ste));
}

void Sementic_Analysis::assignfromArray(const string& arrayname, const string& offset, string& value, STE* ste)
{
	//value = newTemporary();
	//quaternions.addTail(new Quaternion(Operator::ASAR, arrayname, offset, value, ste));

	value = newvar();
	ADDMIDCODE("geta", arrayname, offset, value);

}

void Sementic_Analysis::assignToArray(const string& arrayname, const string& offset, const string& value, STE* ste)
{
	//quaternions.addTail(new Quaternion(Operator::ARAS, value, offset, arrayname, ste));
	ADDMIDCODE("[]=", value, offset, arrayname);
}



void Sementic_Analysis::push(const string& value, STE* ste)
{
	//quaternions.addTail(new Quaternion(Operator::PUSH, value, "", "", ste));

}

void Sementic_Analysis::passParameter(const string& value, STE* ste, string paraname, string funcname)
{

	ADDMIDCODE("push", funcname, paraname, value);
}

void Sementic_Analysis::assign(const string& name, const string& value, STE* ste)
{
	//quaternions.addTail(new Quaternion(Operator::ASSIGN, value, "", name, ste));
	ADDMIDCODE("=", value, "", name);
}

void Sementic_Analysis::compare(const string& value1, const string& value2, STE* ste)
{
	quaternions.addTail(new Quaternion(Operator::CMP, value1, value2, "", ste));
}

string& Sementic_Analysis::stringDefine(const string& str, STE* ste)
{
	static int count = 0;
	static string name;
	table.addString(str);
	return name;
}

void Sementic_Analysis::readInteger(const string& name, STE* ste)
{
	quaternions.addTail(new Quaternion(Operator::RI, name, "", "", ste));
	ADDMIDCODE("scf", "", "", name);
}

void Sementic_Analysis::readChar(const string& name, STE* ste)
{
	quaternions.addTail(new Quaternion(Operator::RC, name, "", "", ste));
	ADDMIDCODE("scf", "", "", name);
}

void Sementic_Analysis::writeString(const string& name, STE* ste)
{
	quaternions.addTail(new Quaternion(Operator::WS, name, "", "", ste));
}

void Sementic_Analysis::writeInteger(const string& value, STE* ste)
{
	quaternions.addTail(new Quaternion(Operator::WI, value, "", "", ste));
}

void Sementic_Analysis::writeChar(const string& ch, STE* ste)
{
	quaternions.addTail(new Quaternion(Operator::WC, ch, "", "", ste));
}

void Sementic_Analysis::newLine()
{
	quaternions.addTail(new Quaternion(Operator::NL, "", "", "", NULL));
}

void Sementic_Analysis::outmidcode()
{
	quaternions.output(midcode);
}

void Sementic_Analysis::label(const string& lab)
{
	quaternions.addTail(new Quaternion(Operator::LAB, lab, "", "", NULL));
}

string& Sementic_Analysis::newLabel()
{
	return newLabel("");
}

string& Sementic_Analysis::newLabel(const string& forehead)
{
	static string newlab;
	labelCount++;
	newlab = forehead;
	newlab += stringPlusInt("_label_", labelCount);
	return newlab;
}

/*
LSS,        //小于号 <
LEQ,        //小于等于号 <=
GRE,        //大于号 >
GEQ,        //大于等于号 >=
EQL,        //等于号 ==
NEQ,        //不等号 !=
*/

void Sementic_Analysis::dissatisfyJump(SymbolType op, const string& lab, STE* ste)
{
	Operator jmpop;
	switch (op) {
	case SymbolType::LSS:
		jmpop = Operator::JGE;
		break;
	case SymbolType::LEQ:
		jmpop = Operator::JG;
		break;
	case SymbolType::GRE:
		jmpop = Operator::JLE;
		break;
	case SymbolType::GEQ:
		jmpop = Operator::JL;
		break;
	case SymbolType::EQL:
		jmpop = Operator::JNE;
		break;
	case SymbolType::NEQ:
		jmpop = Operator::JE;
		break;
	default:
		exit(-2441);
		break;
	}
	quaternions.addTail(new Quaternion(jmpop, "", "", lab, ste));
}

void Sementic_Analysis::satisfyJump(SymbolType op, const string& lab, STE* ste)
{
	Operator jmpop;
	switch (op) {
	case SymbolType::LSS:
		jmpop = Operator::JL;
		break;
	case SymbolType::LEQ:
		jmpop = Operator::JLE;
		break;
	case SymbolType::GRE:
		jmpop = Operator::JG;
		break;
	case SymbolType::GEQ:
		jmpop = Operator::JGE;
		break;
	case SymbolType::EQL:
		jmpop = Operator::JE;
		break;
	case SymbolType::NEQ:
		jmpop = Operator::JNE;
		break;
	default:
		exit(-2441);
		break;
	}
	quaternions.addTail(new Quaternion(jmpop, "", "", lab, ste));
}

void Sementic_Analysis::jumpAnyway(string lab, STE* ste)
{
	quaternions.addTail(new Quaternion(Operator::JMP, "", "", lab, ste));
}

void Sementic_Analysis::funcStart()
{
	label(table.encodePosition());
}

void Sementic_Analysis::funcReturn()
{
	quaternions.addTail(new Quaternion(Operator::FUNCEND, "", "", "", NULL));

}

string Sementic_Analysis::newvar()
{
	char var[20];
	sprintf(var, "$%d", varcnt++);
	return var;
}

string Sementic_Analysis::newlabel()
{
	char label[20];
	sprintf(label, "LAB%d", labelcnt++);
	return label;
}
