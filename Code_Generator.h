#ifndef TARGET_CODE_GENERATOR__H
#define TARGET_CODE_GENERATOR__H
#define _CRT_SECURE_NO_WARNINGS
#include "Quaternions.h"
#include "SymbolTable_Analysis.h"
#include <fstream>
#include <string>
using std::ofstream;
using std::string;
using namespace std;

typedef struct node1 {
	string op;
	string var1;
	string var2;
	string var3;
}Pnode1;

typedef struct node8
{
	string name;
	int addr;
	int type;
	int isglobal;
}Pnode8;


class Code_Generator
{
public:
	Code_Generator(ofstream& target, FILE*, ofstream&, Quaternions&, SymbolTable_Analysis&);

	void getasm();
	void inaddr(int type, int tmi, int glob);
	void tempinaddr(int type, int glob, string name);
	void arrayin(string var, int size, int type);
	void arrayin2(string var, int size, int type);
	void funcop();
	void paraop();
	void plusop();
	void minusop();
	void mulop();
	void divop();
	void greop();
	void geqop();
	void lssop();
	void leqop();
	void neqop();
	void eqlop();
	void assignop();
	void arrayassignop();
	void getaop();
	void labop();
	void scfop();
	void prtop();
	void BZop();
	void GOTOop();
	void pushop();
	void callop();
	void retop();
	int isnum(string);
	int getNameback(string);
	int getSback(string);
	void addtempvar(string name);

	void addmidcode(string op1, string var1, string var2, string var3);
	void outmidcode();
	void outsymbol();

private:
	ofstream& midcode;
	ofstream& target;
	FILE* out2;
	vector<string> tempvar;
	Quaternions& quaternions;
	SymbolTable_Analysis& table;

};
#endif
