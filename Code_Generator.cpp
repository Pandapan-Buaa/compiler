#include "Code_Generator.h"
#include <cstdio>


Pnode8 keepaddr[2000]; //符号表
int totalkeepaddr = 0;
int offset = 4;
Pnode8 keeptemp[2000];
int totalkeeptemp = 0;

Pnode1 keepmidcode[10000]; //中间代码
int totalmidcode = 0;


int order = 0;
string funcname;
int ismain = 0;


Code_Generator::Code_Generator(ofstream& target, FILE* file, ofstream& midcode, Quaternions& quaternions, SymbolTable_Analysis& table) : target(target), out2(file), midcode(midcode), quaternions(quaternions), table(table)
{
	out2 = fopen("mips.txt", "w");
}

void Code_Generator::getasm()
{
	fprintf(out2, ".data\n");
	fprintf(out2, "keep1:.space 10000\n");
	fprintf(out2, "nextline: .asciiz \"\\n\"\n");
	for (int i = 0; i < table.getStringMap().size(); i++) {
		string elm = table.getStringMap().at(i);
		string elm1;
		for (int j = 0; j < elm.size(); j++) {
			if (elm.at(j) != '\\') {
				elm1.push_back(elm.at(j));
			}
			else {
				elm1.push_back(elm.at(j));
				elm1.push_back(elm.at(j));
			}
		}
		fprintf(out2, "slabel_%d: ", i);
		fprintf(out2, ".asciiz \"%s\"\n", elm1.c_str());
		//cout << i << " " << elm << endl;
	}
	fprintf(out2, ".text\n");
	int size = 0;
	int i = 0;
	order = 0;
	//算出全局变量的位置
	while (order < totalmidcode) {
		while (keepmidcode[i].op == "const") {
			size++;
			i++;
		}
		while (keepmidcode[i].op == "int" || keepmidcode[i].op == "char"
			|| keepmidcode[i].op == "ints" || keepmidcode[i].op == "chars") {
			if (keepmidcode[i].op == "int" || keepmidcode[i].op == "char") {
				size++;
				i++;
			}
			else if (keepmidcode[i].op == "ints" || keepmidcode[i].op == "chars") {
				//size += atoi(keepmidcode[i].var2);
				size += atoi(keepmidcode[i].var2.c_str());
				i++;
			}
		}

		fprintf(out2, "subi $sp $sp %d\n", 4 * size);
		fprintf(out2, "addi $t8 $sp 0\n");
		//全局变量的基地址是t8，非全局变量基地址是sp，

		while (keepmidcode[order].op == "const") {
			if (keepmidcode[order].var1 == "int" || keepmidcode[order].var1 == "ints") {
				inaddr(0, 1, 1);//中间位暂时无用，第一位代表int/char,第三位代表是否为全局变量
				arrayin2(keepmidcode[order].var2, 1, 0);
			}
			else {
				inaddr(1, 1, 1);
				arrayin2(keepmidcode[order].var2, 1, 1);
			}

			order++;
		}
		while (keepmidcode[order].op == "int" || keepmidcode[order].op == "ints"
			|| keepmidcode[order].op == "char" || keepmidcode[order].op == "chars") {

			if (keepmidcode[order].op == "int" || keepmidcode[order].op == "ints") {
				inaddr(0, 1, 1);
			}
			else {
				inaddr(1, 1, 1);
			}
			if (keepmidcode[order].var2 == "") {
				offset += 4;
			}
			else {
				arrayin2("0", atoi(keepmidcode[order].var2.c_str()), 0);
				//单个初始化为0，数组不初始化，只记录offset变化
			}
			order++;
		}
		fprintf(out2, "j main\n");

		while (keepmidcode[order].op == "func") {
			fprintf(out2, "%s:\n", keepmidcode[order].var3.c_str());
			funcname = keepmidcode[order].var3;
			if (funcname == "main") {//main函数始终在末尾
				ismain = 1;
			}
			order++;
			funcop();
		}
	}

}
//符号表建立（addr储存）
void Code_Generator::inaddr(int type, int tmi, int glob)
{
	if (tmi == 1) {
		keepaddr[totalkeepaddr].name = keepmidcode[order].var3;
	}
	else {
		keepaddr[totalkeepaddr].name = keepmidcode[tmi].var3;
	}
	keepaddr[totalkeepaddr].addr = offset;
	keepaddr[totalkeepaddr].isglobal = glob;
	keepaddr[totalkeepaddr].type = type;
	//cout << "&" << keepaddr[totalkeepaddr].name << endl;
	totalkeepaddr++;
}
void Code_Generator::tempinaddr(int type, int glob, string name)
{
	keeptemp[totalkeeptemp].name = name;
	keeptemp[totalkeeptemp].addr = offset;
	keeptemp[totalkeeptemp].isglobal = glob;
	keeptemp[totalkeeptemp].type = type;
	totalkeeptemp++;
}
//sp压栈
void Code_Generator::arrayin(string var, int size, int type)
{
	if (size == 1) {
		if (type == 0)fprintf(out2, "li $t0 %d\n", atoi(var.c_str()));
		else fprintf(out2, "li $t0 %d\n", (int)var[0]);

		fprintf(out2, "sw $t0 0x%x($sp)\n", offset);
	}
	offset = offset + 4 * size;
}
//t8压栈
void Code_Generator::arrayin2(string var, int size, int type)
{
	if (size == 1) {
		if (type == 0)fprintf(out2, "li $t0 %d\n", atoi(var.c_str()));
		else fprintf(out2, "li $t0 %d\n", (int)var[0]);
		//cout << "char value" << var << " " << (int)var[0] << endl;
		fprintf(out2, "sw $t0 0x%x($t8)\n", offset);
	}
	offset = offset + 4 * size;
}

void Code_Generator::funcop()
{
	int keeps1 = totalkeepaddr;
	int keeps2 = totalkeeptemp;
	offset = 4;
	fprintf(out2, "subi $sp $sp 8\n");
	fprintf(out2, "sw $ra 4($sp)\n");
	fprintf(out2, "sw $t9 8($sp)\n");
	fprintf(out2, "addi $t9 $sp 0\n");
	fprintf(out2, "subi $sp $sp %d\n", 4 * 100);
	//保存现场，并为下个函数提供100个栈空间
	if (keepmidcode[order].op == "para") {
		paraop();//函数变量入表（addr储存）
	}
	while (keepmidcode[order].op == "const") {
		if (keepmidcode[order].var1 == "int" || keepmidcode[order].var1 == "ints") {
			inaddr(0, 1, 0);
			arrayin(keepmidcode[order].var2, 1, 0);
		}
		else {
			inaddr(1, 1, 0);
			arrayin(keepmidcode[order].var2, 1, 1);
		}

		order++;
	}
	while (keepmidcode[order].op == "int" || keepmidcode[order].op == "ints"
		|| keepmidcode[order].op == "char" || keepmidcode[order].op == "chars") {

		if (keepmidcode[order].op == "int" || keepmidcode[order].op == "ints") {
			inaddr(0, 1, 0);
		}
		else {
			inaddr(1, 1, 0);
		}
		if (keepmidcode[order].var2 == "") {
			offset += 4;
		}
		else {
			arrayin("0", atoi(keepmidcode[order].var2.c_str()), 0);
		}
		order++;
	}
	//同上toasm中
	//以下为函数语句逐一翻译
	//修改，便利到end把临时变量,存入sp
	//问题： 临时变量的type?后续都不需要改
	//重复，建vector统一插入
	int scannum = order;
	tempvar.clear();
	while (keepmidcode[scannum].op != "end") {
		if (keepmidcode[scannum].var1[0] == '$') {
			addtempvar(keepmidcode[scannum].var1);
		}
		if (keepmidcode[scannum].var2[0] == '$') {
			addtempvar(keepmidcode[scannum].var2);
		}
		if (keepmidcode[scannum].var3[0] == '$') {
			addtempvar(keepmidcode[scannum].var3);
		}
		scannum++;
	}
	for (int i = 0; i < tempvar.size(); i++) {
		tempinaddr(1, 0, tempvar.at(i));
		offset += 4;
	}
	//cout << getNameback("$1") << endl;
	//fprintf(out2, "subi $sp $sp %d\n", 4 * ((totalkeeptemp - keeps2) + (totalkeepaddr - keeps1) + 2));


	while (keepmidcode[order].op != "end") {
		if (keepmidcode[order].op == "plus") {
			plusop();
		}
		if (keepmidcode[order].op == "minus") {
			minusop();
		}
		if (keepmidcode[order].op == "mul") {
			mulop();
		}
		if (keepmidcode[order].op == "div") {
			divop();
		}
		if (keepmidcode[order].op == ">") {
			greop();
		}
		if (keepmidcode[order].op == ">=") {
			geqop();
		}
		if (keepmidcode[order].op == "<") {
			lssop();
		}
		if (keepmidcode[order].op == "<=") {
			leqop();
		}
		if (keepmidcode[order].op == "!=") {
			neqop();
		}
		if (keepmidcode[order].op == "==") {
			eqlop();
		}
		if (keepmidcode[order].op == "=") {
			assignop();
		}
		if (keepmidcode[order].op == "[]=") {
			arrayassignop();
		}
		if (keepmidcode[order].op == "geta") {
			getaop();
		}
		if (keepmidcode[order].op == "lab:") {
			labop();
		}
		if (keepmidcode[order].op == "scf") {
			scfop();
		}
		if (keepmidcode[order].op == "prt") {
			prtop();
		}
		if (keepmidcode[order].op == "BZ") {
			BZop();
		}
		if (keepmidcode[order].op == "GOTO") {
			GOTOop();
		}
		if (keepmidcode[order].op == "push") {
			pushop();
			//cout << "push" << endl;
		}
		if (keepmidcode[order].op == "call") {
			callop();
		}
		if (keepmidcode[order].op == "ret") {
			retop();
		}
		order++;
	}
	fprintf(out2, "fend_%s:\n", funcname.c_str());
	fprintf(out2, "addi $sp $t9 0\n");//t9保存了sp现场位置
	fprintf(out2, "lw $ra 4($sp)\n");
	fprintf(out2, "lw $t9 8($sp)\n");
	fprintf(out2, "addi $sp $sp 8\n");
	//恢复现场



	if (ismain) {
		fprintf(out2, "li $v0 10\n");
		fprintf(out2, "syscall\n");
	}
	else {
		fprintf(out2, "jr $ra\n");
	}
	order++;
	/*for (int i = 0; i < totalkeepaddr; i++) {
		cout << keepaddr[i].name << " " << keepaddr[i].addr << " " << keepaddr[i].isglobal << endl;
	}*/
	totalkeepaddr = keeps1;
	totalkeeptemp = keeps2;
}

void Code_Generator::paraop()
{
	int type = 0;
	while (keepmidcode[order].op == "para") {
		if (keepmidcode[order].var1 == "int") {
			type = 0;
		}
		else {
			type = 1;
		}
		inaddr(type, 1, 0);
		//cout << "paaaaaaaaaaaa " << keepmidcode[order].var1 << type << endl;
		offset = offset + 4;
		order++;
	}
}

void Code_Generator::plusop()
{
	int i = 0, j = 0, w = 0;
	int n1 = 0, n2 = 0, n3 = 0;
	//判断var1是否为 常量 ，常量则加载到t0
	//不是常量则在符号表中查找，若找到则加载到t0（根据isglob选择t8或者sp）
	//否则为临时变量，
	//t0 t1相加，存至t0，var3将t0存入对应位置
	//var1,var2,var3完全一致，除var3不可能为常量，且应为sw
	//+-*/完全一致
	if (isnum(keepmidcode[order].var1)) {
		fprintf(out2, "li $t0 %d\n", atoi(keepmidcode[order].var1.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var1);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[i].addr);

			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[i].addr);

			}
		}
		else {
			fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var1)].addr);

		}
	}
	if (isnum(keepmidcode[order].var2)) {
		fprintf(out2, "li $t1 %d\n", atoi(keepmidcode[order].var2.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var2);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t1 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t1 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t1 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
		}
	}
	fprintf(out2, "add $t0 $t0 $t1\n");
	j = getNameback(keepmidcode[order].var3);
	if (j != -1) {
		if (keepaddr[j].isglobal) {
			fprintf(out2, "sw $t0 0x%x($t8)\n", keepaddr[j].addr);
		}
		else {
			fprintf(out2, "sw $t0 0x%x($sp)\n", keepaddr[j].addr);
		}
	}
	else {
		fprintf(out2, "sw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var3)].addr);
	}
}

void Code_Generator::minusop()
{
	int i = 0, j = 0, w = 0;
	int n1 = 0, n2 = 0, n3 = 0;
	if (isnum(keepmidcode[order].var1)) {
		fprintf(out2, "li $t0 %d\n", atoi(keepmidcode[order].var1.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var1);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var1)].addr);
		}
	}
	if (isnum(keepmidcode[order].var2)) {
		fprintf(out2, "li $t1 %d\n", atoi(keepmidcode[order].var2.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var2);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t1 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t1 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t1 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
		}
	}
	fprintf(out2, "sub $t0 $t0 $t1\n");
	j = getNameback(keepmidcode[order].var3);
	if (j != -1) {
		if (keepaddr[j].isglobal) {
			fprintf(out2, "sw $t0 0x%x($t8)\n", keepaddr[j].addr);
		}
		else {
			fprintf(out2, "sw $t0 0x%x($sp)\n", keepaddr[j].addr);
		}
	}
	else {
		fprintf(out2, "sw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var3)].addr);
	}
}

void Code_Generator::mulop()
{
	int i = 0, j = 0, w = 0;
	int n1 = 0, n2 = 0, n3 = 0;
	if (isnum(keepmidcode[order].var1)) {
		fprintf(out2, "li $t0 %d\n", atoi(keepmidcode[order].var1.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var1);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var1)].addr);
		}
	}
	if (isnum(keepmidcode[order].var2)) {
		fprintf(out2, "li $t1 %d\n", atoi(keepmidcode[order].var2.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var2);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t1 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t1 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t1 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
		}
	}
	fprintf(out2, "mul $t0 $t0 $t1\n");
	j = getNameback(keepmidcode[order].var3);
	if (j != -1) {
		if (keepaddr[j].isglobal) {
			fprintf(out2, "sw $t0 0x%x($t8)\n", keepaddr[j].addr);
		}
		else {
			fprintf(out2, "sw $t0 0x%x($sp)\n", keepaddr[j].addr);
		}
	}
	else {
		fprintf(out2, "sw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var3)].addr);
	}
}

void Code_Generator::divop()
{
	int i = 0, j = 0, w = 0;
	int n1 = 0, n2 = 0, n3 = 0;
	if (isnum(keepmidcode[order].var1)) {
		fprintf(out2, "li $t0 %d\n", atoi(keepmidcode[order].var1.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var1);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var1)].addr);
		}
	}
	if (isnum(keepmidcode[order].var2)) {
		fprintf(out2, "li $t1 %d\n", atoi(keepmidcode[order].var2.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var2);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t1 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t1 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t1 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
		}
	}
	fprintf(out2, "div $t0 $t0 $t1\n");
	j = getNameback(keepmidcode[order].var3);
	if (j != -1) {
		if (keepaddr[j].isglobal) {
			fprintf(out2, "sw $t0 0x%x($t8)\n", keepaddr[j].addr);
		}
		else {
			fprintf(out2, "sw $t0 0x%x($sp)\n", keepaddr[j].addr);
		}
	}
	else {
		fprintf(out2, "sw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var3)].addr);
	}
}

void Code_Generator::greop()
{
	int i = 0, j = 0, w = 0;
	int n1 = 0, n2 = 0, n3 = 0;
	if (isnum(keepmidcode[order].var1)) {
		fprintf(out2, "li $t0 %d\n", atoi(keepmidcode[order].var1.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var1);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var1)].addr);
		}
	}
	if (isnum(keepmidcode[order].var2)) {
		fprintf(out2, "li $t1 %d\n", atoi(keepmidcode[order].var2.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var2);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t1 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t1 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t1 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
		}
	}
	fprintf(out2, "sgt $t0 $t0 $t1\n");

	//t0 > t1 
}

void Code_Generator::geqop()
{
	int i = 0, j = 0, w = 0;
	int n1 = 0, n2 = 0, n3 = 0;
	if (isnum(keepmidcode[order].var1)) {
		fprintf(out2, "li $t0 %d\n", atoi(keepmidcode[order].var1.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var1);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var1)].addr);
		}
	}
	if (isnum(keepmidcode[order].var2)) {
		fprintf(out2, "li $t1 %d\n", atoi(keepmidcode[order].var2.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var2);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t1 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t1 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t1 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
		}
	}
	//fprintf(out2, "slt $t0 $t0 $t1\n");
	//fprintf(out2, "slti $t0 $t0 1\n");
	fprintf(out2, "sge $t0 $t0 $t1\n");
	//>= 
	//t0 < t1 t0 = 1  t0 >= t1 t0 = 0
	//再和1比较
}

void Code_Generator::lssop()
{
	int i = 0, j = 0, w = 0;
	int n1 = 0, n2 = 0, n3 = 0;
	if (isnum(keepmidcode[order].var1)) {
		fprintf(out2, "li $t0 %d\n", atoi(keepmidcode[order].var1.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var1);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var1)].addr);
		}
	}
	if (isnum(keepmidcode[order].var2)) {
		fprintf(out2, "li $t1 %d\n", atoi(keepmidcode[order].var2.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var2);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t1 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t1 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t1 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
		}
	}
	fprintf(out2, "slt $t0 $t0 $t1\n");
}

void Code_Generator::leqop()
{
	int i = 0, j = 0, w = 0;
	int n1 = 0, n2 = 0, n3 = 0;
	if (isnum(keepmidcode[order].var1)) {
		fprintf(out2, "li $t0 %d\n", atoi(keepmidcode[order].var1.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var1);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var1)].addr);
		}
	}
	if (isnum(keepmidcode[order].var2)) {
		fprintf(out2, "li $t1 %d\n", atoi(keepmidcode[order].var2.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var2);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t1 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t1 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t1 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
		}
	}
	//fprintf(out2, "slt $t0 $t1 $t0\n");
	//fprintf(out2, "slti $t0 $t0 1\n");
	fprintf(out2, "sle $t0 $t0 $t1\n");
}

void Code_Generator::neqop()
{
	int i = 0, j = 0, w = 0;
	int n1 = 0, n2 = 0, n3 = 0;
	if (isnum(keepmidcode[order].var1)) {
		fprintf(out2, "li $t0 %d\n", atoi(keepmidcode[order].var1.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var1);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var1)].addr);
		}
	}
	if (isnum(keepmidcode[order].var2)) {
		fprintf(out2, "li $t1 %d\n", atoi(keepmidcode[order].var2.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var2);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t1 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t1 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t1 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
		}
	}
	//fprintf(out2, "slt $t2 $t1 $t0\n");
	//fprintf(out2, "slt $t3 $t0 $t1\n");
	//fprintf(out2, "or $t0 $t2 $t3\n");
	fprintf(out2, "sne $t0 $t0 $t1\n");
}

void Code_Generator::eqlop()
{
	int i = 0, j = 0, w = 0;
	int n1 = 0, n2 = 0, n3 = 0;
	if (isnum(keepmidcode[order].var1)) {
		fprintf(out2, "li $t0 %d\n", atoi(keepmidcode[order].var1.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var1);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var1)].addr);
		}
	}
	if (isnum(keepmidcode[order].var2)) {
		fprintf(out2, "li $t1 %d\n", atoi(keepmidcode[order].var2.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var2);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t1 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t1 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t1 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
		}
	}
	//fprintf(out2, "slt $t2 $t0 $t1\n");
	//fprintf(out2, "slti $t2 $t2 1\n");
	//fprintf(out2, "slt $t3 $t1 $t0\n");
	//fprintf(out2, "slti $t3 $t3 1\n");
	//fprintf(out2, "and $t0 $t2 $t3\n");
	fprintf(out2, "seq $t0 $t0 $t1\n");
}

void Code_Generator::assignop()
{
	int i = 0, j = 0, w = 0;
	int n1 = 0, n2 = 0, n3 = 0;
	if (isnum(keepmidcode[order].var1)) {
		fprintf(out2, "li $t0 %d\n", atoi(keepmidcode[order].var1.c_str()));
	}
	else {
		i = getNameback(keepmidcode[order].var1);
		if (i != -1) {
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var1)].addr);
		}
	}
	j = getNameback(keepmidcode[order].var3);
	if (j != -1) {
		if (keepaddr[j].isglobal == 1) {
			fprintf(out2, "sw $t0 0x%x($t8)\n", keepaddr[j].addr);
		}
		else {
			fprintf(out2, "sw $t0 0x%x($sp)\n", keepaddr[j].addr);
		}
	}
	else {
		fprintf(out2, "sw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var3)].addr);
	}
}


//var3[var2] = var1
void Code_Generator::arrayassignop()
{
	int i = 0, j = 0, k = 0;
	int adr1 = 0;
	i = getNameback(keepmidcode[order].var3);
	if (i != -1) {
		adr1 = keepaddr[i].addr;
	}//数组必然存在

	//lenth为数字
	if (isnum(keepmidcode[order].var2) == 1) {
		adr1 += atoi(keepmidcode[order].var2.c_str()) * 4;
		//var1是数字
		if (isnum(keepmidcode[order].var1) == 1) {
			fprintf(out2, "li $t0 %s\n", keepmidcode[order].var1.c_str());
			if (keepaddr[i].isglobal) {
				fprintf(out2, "sw $t0 0x%x($t8)\n", adr1);
			}
			else {
				fprintf(out2, "sw $t0 0x%x($sp)\n", adr1);
			}
		}
		else {
			j = getNameback(keepmidcode[order].var1);
			if (j != -1) {
				if (keepaddr[j].isglobal == 1) {
					fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[j].addr);
				}
				else {
					fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[j].addr);
				}
				if (keepaddr[i].isglobal) {
					fprintf(out2, "sw $t0 0x%x($t8)\n", adr1);
				}
				else {
					fprintf(out2, "sw $t0 0x%x($sp)\n", adr1);
				}
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var1)].addr);
				if (keepaddr[i].isglobal) {
					fprintf(out2, "sw $t0 0x%x($t8)\n", adr1);
				}
				else {
					fprintf(out2, "sw $t0 0x%x($sp)\n", adr1);
				}
			}
		}
	}
	else {//lenth为变量，查表
		k = getNameback(keepmidcode[order].var2);
		if (k != -1) {//全局或局部定义
			if (keepaddr[k].isglobal) {
				fprintf(out2, "lw $t1 0x%x($t8)\n", keepaddr[k].addr);
			}
			else {
				fprintf(out2, "lw $t1 0x%x($sp)\n", keepaddr[k].addr);
			}
		}
		else {//临时
			fprintf(out2, "lw $t1 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
		}
		fprintf(out2, "mul $t1 $t1 4\naddi $t1 $t1 %d\n", adr1);//t1为偏移量
		//计算在栈中位置
		if (isnum(keepmidcode[order].var1) == 1) {
			fprintf(out2, "li $t0 %s\n", keepmidcode[order].var1.c_str());
			//fprintf(out2, "sw $t0 ($t1)\n"); //判断数组是全局还是局部，对应赋值
			if (keepaddr[i].isglobal) {
				fprintf(out2, "add $t1 $t1 $t8\n");
				fprintf(out2, "sw $t0 ($t1)\n", adr1);
			}
			else {
				fprintf(out2, "add $t1 $t1 $sp\n");
				fprintf(out2, "sw $t0 ($t1)\n", adr1);
			}
		}
		else {
			j = getNameback(keepmidcode[order].var1);
			if (j != -1) {
				if (keepaddr[j].isglobal == 1) {
					fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[j].addr);
				}
				else {
					fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[j].addr);
				}
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var1)].addr);
			}
			//t0为赋值 t1为栈中位置
			//fprintf(out2, "sw $t0 ($t1)\n");
			if (keepaddr[i].isglobal) {
				fprintf(out2, "add $t1 $t1 $t8\n");
				fprintf(out2, "sw $t0 ($t1)\n", adr1);
			}
			else {
				fprintf(out2, "add $t1 $t1 $sp\n");
				fprintf(out2, "sw $t0 ($t1)\n", adr1);
			}
		}
	}
}

//var = name[t]
// name t var
void Code_Generator::getaop()
{
	int i = 0, j = 0, k = 0;
	int adr1 = 0, adr2 = 0, adr3 = 0;
	i = getNameback(keepmidcode[order].var1);//name
	if (i != -1) {
		adr1 = keepaddr[i].addr;
	}
	//i name的位置
	if (isnum(keepmidcode[order].var2)) {
		adr1 += atoi(keepmidcode[order].var2.c_str()) * 4;//数组偏移
		j = getNameback(keepmidcode[order].var3); //var
		if (j != -1) {
			adr2 = keepaddr[j].addr;//var偏移
			if (keepaddr[i].isglobal) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", adr1);
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", adr1);
			}
			if (keepaddr[j].isglobal) {
				fprintf(out2, "sw $t0 0x%x($t8)\n", adr2);
			}
			else {
				fprintf(out2, "sw $t0 0x%x($sp)\n", adr2);
			}
		}
		else {
			if (keepaddr[i].isglobal) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", adr1);
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", adr1);
			}
			fprintf(out2, "sw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var3)].addr);
		}

	}
	else {
		k = getNameback(keepmidcode[order].var2);
		if (k != -1) {//不是常量，复杂一些，加载lenth到t1
			if (keepaddr[k].isglobal) {
				fprintf(out2, "lw $t1 0x%x($t8)\n", keepaddr[k].addr);
			}
			else {
				fprintf(out2, "lw $t1 0x%x($sp)\n", keepaddr[k].addr);
			}
		}
		else {
			fprintf(out2, "lw $t1 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
		}
		fprintf(out2, "mul $t1 $t1 4\naddi $t1 $t1 %d\n", adr1);
		//计算数组元素位置
		if (keepaddr[i].isglobal) {
			fprintf(out2, "add $t1 $t1 $t8\n");
		}
		else {
			fprintf(out2, "add $t1 $t1 $sp\n");
		}
		fprintf(out2, "lw $t0 ($t1)\n");
		j = getNameback(keepmidcode[order].var3);
		//根据var3位置存储
		if (j != -1) {
			adr2 = keepaddr[j].addr;
			if (keepaddr[j].isglobal) {
				fprintf(out2, "sw $t0 0x%x($t8)\n", adr2);
			}
			else {
				fprintf(out2, "sw $t0 0x%x($sp)\n", adr2);
			}
		}
		else {
			fprintf(out2, "sw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var3)].addr);
		}

	}
}

void Code_Generator::labop()
{
	fprintf(out2, "%s:\n", keepmidcode[order].var3.c_str());
}

void Code_Generator::scfop()
{
	int i = 0, adr1 = 0;
	//0为int,1为char
	int type = 0;
	i = getNameback(keepmidcode[order].var3);
	if (i != -1) {
		adr1 = keepaddr[i].addr;
		type = keepaddr[i].type;
		if (type == 0) {
			fprintf(out2, "li $v0 5\nsyscall\n");
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "sw $v0 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "sw $v0 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "li $v0 12\nsyscall\n");
			if (keepaddr[i].isglobal == 1) {
				fprintf(out2, "sw $v0 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "sw $v0 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
	}
}

//可修改，根据符号表，在.data产生label并将label名字存到var2，进而减少语句li
void Code_Generator::prtop()
{
	int i = 0, j = 0, adr1 = 0;
	int type = 0;
	if (keepmidcode[order].var1 != "") {
		if (atoi(keepmidcode[order].var1.c_str()) != 0)
			fprintf(out2, "la $a0 %s%d\nli $v0 4\nsyscall\n", "slabel_", atoi(keepmidcode[order].var1.c_str()));
	}
	/*while (keepmidcode[order].var1[i] != '\0') {
		fprintf(out2, "li $v0 11\nli $a0 %d\nsyscall\n", keepmidcode[order].var1[i]);
		i++;
	}*/
	if (keepmidcode[order].var2 != "") {
		if (isnum(keepmidcode[order].var2) && keepmidcode[order].var3 == "char") {
			fprintf(out2, "li $v0 11\nli $a0 %d\nsyscall\n", atoi(keepmidcode[order].var2.c_str()));
		}
		else if (isnum(keepmidcode[order].var2)) {
			fprintf(out2, "li $v0 1\nli $a0 %d\nsyscall\n", atoi(keepmidcode[order].var2.c_str()));
		}
		else {
			j = getNameback(keepmidcode[order].var2);
			if (j != -1) {
				adr1 = keepaddr[j].addr;
				type = keepaddr[j].type;
				//cout << "bug " << keepaddr[j].name << type << endl;
				if (type == 0) {
					if (keepaddr[j].isglobal) {
						fprintf(out2, "li $v0 1\nlw $a0 0x%x($t8)\nsyscall\n", keepaddr[j].addr);
					}
					else {
						fprintf(out2, "li $v0 1\nlw $a0 0x%x($sp)\nsyscall\n", keepaddr[j].addr);
					}
				}
				else {
					if (keepaddr[j].isglobal) {
						fprintf(out2, "li $v0 11\nlw $a0 0x%x($t8)\nsyscall\n", keepaddr[j].addr);
					}
					else {
						fprintf(out2, "li $v0 11\nlw $a0 0x%x($sp)\nsyscall\n", keepaddr[j].addr);
					}
				}
			}
			else {
				if (keepmidcode[order].var3 == "int") {
					type = 0;
				}
				else {
					type = 1;
				}
				if (type == 0) {
					fprintf(out2, "li $v0 1\nlw $a0 0x%x($sp)\nsyscall\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
				}
				else {
					fprintf(out2, "li $v0 11\nlw $a0 0x%x($sp)\nsyscall\n", keeptemp[getSback(keepmidcode[order].var2)].addr);
				}
			}

		}
	}
	fprintf(out2, "la $a0 nextline\nli $v0 4\nsyscall\n");
}

void Code_Generator::BZop()
{
	fprintf(out2, "bne $t0 1 %s\n", keepmidcode[order].var3.c_str());
}

void Code_Generator::GOTOop()
{
	fprintf(out2, "j %s\n", keepmidcode[order].var3.c_str());
}

//未完成
void Code_Generator::pushop()
{
	/*int i = 0, adr1 = 0;
	int totalpush = 0;
	int keeps = order
	int size = 100;
	while (keepmidcode[keeps].op != "call") {
		if (keepmidcode[keeps].op == "push") {
			totalpush++;
		}
		keeps++;
	}*/
	int i, adr1;
	int j;
	string fname = keepmidcode[order].var1;
	string pname = keepmidcode[order].var2;
	table.find(fname);
	for (i = 0; i < table.ste->arguments->size(); i++) {
		if (table.ste->arguments->at(i).name == pname)
			break;
	}
	adr1 = (i + 1) * 4;
	//cout << fname << " " << pname<<" paranum " << adr1 << endl;
	if (isnum(keepmidcode[order].var3) == 1) {
		fprintf(out2, "li $t0 %s\n", keepmidcode[order].var3.c_str());
		fprintf(out2, "addi $t1 $sp -408\n");
		fprintf(out2, "sw $t0 0x%x($t1)\n", adr1);
	}
	else {
		j = getNameback(keepmidcode[order].var3);
		if (j != -1) {
			if (keepaddr[j].isglobal == 1) {
				fprintf(out2, "lw $t0 0x%x($t8)\n", keepaddr[j].addr);
			}
			else {
				fprintf(out2, "lw $t0 0x%x($sp)\n", keepaddr[j].addr);
			}
		}
		else {
			fprintf(out2, "lw $t0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var3)].addr);
		}
		//t0为var3 t1为栈中位置
		//fprintf(out2, "sw $t0 0x%x($sp)\n", adr1);
		fprintf(out2, "addi $t1 $sp -408\n", adr1);
		fprintf(out2, "sw $t0 0x%x($t1)\n", adr1);
	}
}

void Code_Generator::callop()
{
	int i = 0, j = 0;
	fprintf(out2, "jal %s\n", keepmidcode[order].var1.c_str());
	if (keepmidcode[order].var3 != "") {
		i = getNameback(keepmidcode[order].var3);
		if (i != -1) {
			if (keepaddr[i].isglobal) {
				fprintf(out2, "sw $v0 0x%x($t8)\n", keepaddr[i].addr);
			}
			else {
				fprintf(out2, "sw $v0 0x%x($sp)\n", keepaddr[i].addr);
			}
		}
		else {
			fprintf(out2, "sw $v0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var3)].addr);
		}
	}
	/*cout << "*********" << endl;
	for (i = 0; i < totalkeepaddr; i++) {
		cout << keepaddr[i].name << " " << keepaddr[i].isglobal << " " << keepaddr[i].addr << endl;
	}*/
}

void Code_Generator::retop()
{
	int i = 0;
	if (keepmidcode[order].var3 != "") {
		if (isnum(keepmidcode[order].var3)) {
			fprintf(out2, "li $v0 %d\n", atoi(keepmidcode[order].var3.c_str()));
		}
		else {
			i = getNameback(keepmidcode[order].var3);
			if (i != -1) {
				if (keepaddr[i].isglobal) {
					fprintf(out2, "lw $v0 0x%x($t8)\n", keepaddr[i].addr);
				}
				else {
					fprintf(out2, "lw $v0 0x%x($sp)\n", keepaddr[i].addr);
				}
			}
			else {
				fprintf(out2, "lw $v0 0x%x($sp)\n", keeptemp[getSback(keepmidcode[order].var3)].addr);
			}
		}
	}
	fprintf(out2, "j fend_%s\n", funcname.c_str());
}

int Code_Generator::isnum(string str)
{
	int i = 0;
	for (i = 0; i < str.size(); i++) {
		if (('0' <= str[i] && str[i] <= '9') || str[i] == '+' || str[i] == '-') {
			return 1;
		}
		return 0;
	}
	return 0;
}
//
int Code_Generator::getNameback(string str)
{
	int i = 0;
	for (i = totalkeepaddr - 1; i >= 0; i--) {
		if (keepaddr[i].name == str) {
			return i;
		}
	}
	return -1;
}
//临时变量addr
int Code_Generator::getSback(string str)
{
	/*int i = 1;
	int count = 0;
	for (i = 1; i < str.size(); i++) {
		count = count * 10 + (str.at(i) - '0') * 4;
	}
	return count;*/
	int i = 0;
	for (i = totalkeeptemp - 1; i >= 0; i--) {
		if (keeptemp[i].name == str) {
			return i;
		}
	}
	cout << "cant happen" << endl;
	return -1;
}

void Code_Generator::addtempvar(string name)
{
	int i;
	for (i = 0; i < tempvar.size(); i++) {
		if (tempvar.at(i) == name) {
			break;
		}
	}
	if (i == tempvar.size())	tempvar.push_back(name);
}



void Code_Generator::addmidcode(string op1, string var1, string var2, string var3)
{
	keepmidcode[totalmidcode].op = op1;
	keepmidcode[totalmidcode].var1 = var1;
	keepmidcode[totalmidcode].var2 = var2;
	keepmidcode[totalmidcode].var3 = var3;
	totalmidcode++;
}

void Code_Generator::outmidcode()
{
	for (int i = 0; i < totalmidcode; i++) {
		midcode << keepmidcode[i].op << " " << keepmidcode[i].var1 << " " << keepmidcode[i].var2 << " " << keepmidcode[i].var3 << endl;
	}
	ofstream midcode1;
	midcode1.open("15061025_钟瑞豪_优化前中间代码.txt");
	for (int i = 0; i < totalmidcode; i++) {
		if (keepmidcode[i].op == "func") {
			midcode1 << keepmidcode[i].var1 << " " << keepmidcode[i].var3 << "()" << endl;
		}
		else if (keepmidcode[i].op == "para") {
			midcode1 << keepmidcode[i].op << " " << keepmidcode[i].var1 << " " << keepmidcode[i].var3 << endl;
		}
		else if (keepmidcode[i].op == "push") {
			midcode1 << keepmidcode[i].op << " " << keepmidcode[i].var1 << endl;
		}
		else if (keepmidcode[i].op == "call") {
			midcode1 << keepmidcode[i].op << " " << keepmidcode[i].var1 << endl;
			midcode1 << keepmidcode[i].var3 << " = RET" << endl;
		}
		else if (keepmidcode[i].op == "ret") {
			midcode1 << "ret " << keepmidcode[i].var3 << endl;
		}
		else if (keepmidcode[i].op == "int" || keepmidcode[i].op == "char") {
			midcode1 << "var " << keepmidcode[i].op << " " << keepmidcode[i].var3 << endl;
		}
		else if (keepmidcode[i].op == "ints" || keepmidcode[i].op == "chars") {
			midcode1 << "var " << keepmidcode[i].op << " " << keepmidcode[i].var3 << "[" << keepmidcode[i].var2 << "]" << endl;
		}
		else if (keepmidcode[i].op == "const") {
			if (keepmidcode[i].var1 == "char")
				midcode1 << "const " << keepmidcode[i].var1 << " " << keepmidcode[i].var3 << " = " << (char)(stoi(keepmidcode[i].var2)) << endl;
			else
				midcode1 << "const " << keepmidcode[i].var1 << " " << keepmidcode[i].var3 << " = " << keepmidcode[i].var2 << endl;
		}
		else if (keepmidcode[i].op == "plus") {
			midcode1 << keepmidcode[i].var3 << " = " << keepmidcode[i].var1 << " + " << keepmidcode[i].var2 << endl;
		}
		else if (keepmidcode[i].op == "minus") {
			midcode1 << keepmidcode[i].var3 << " = " << keepmidcode[i].var1 << " - " << keepmidcode[i].var2 << endl;
		}
		else if (keepmidcode[i].op == "mul") {
			midcode1 << keepmidcode[i].var3 << " = " << keepmidcode[i].var1 << " * " << keepmidcode[i].var2 << endl;
		}
		else if (keepmidcode[i].op == "div") {
			midcode1 << keepmidcode[i].var3 << " = " << keepmidcode[i].var1 << " / " << keepmidcode[i].var2 << endl;
		}
		else if (keepmidcode[i].op == "=") {
			midcode1 << keepmidcode[i].var3 << " = " << keepmidcode[i].var1 << endl;
		}
		else if (keepmidcode[i].op == "geta") {
			midcode1 << keepmidcode[i].var3 << " = " << keepmidcode[i].var1 << "[" << keepmidcode[i].var2 << "]" << endl;
		}
		else if (keepmidcode[i].op == "[]=") {
			midcode1 << keepmidcode[i].var3 << "[" << keepmidcode[i].var2 << "]" << " = " << keepmidcode[i].var1 << endl;
		}
		else if (keepmidcode[i].op == ">" || keepmidcode[i].op == ">=" || keepmidcode[i].op == "<" || keepmidcode[i].op == "<=" || keepmidcode[i].op == "==" || keepmidcode[i].op == "!=") {
			midcode1 << keepmidcode[i].var1 << " " << keepmidcode[i].op << " " << keepmidcode[i].var2 << endl;
		}
		else if (keepmidcode[i].op == "BZ" || keepmidcode[i].op == "GOTO") {
			midcode1 << keepmidcode[i].op << " " << keepmidcode[i].var3 << endl;
		}
		else if (keepmidcode[i].op == "lab:") {
			midcode1 << keepmidcode[i].var3 << ":" << endl;
		}
		else {
			midcode1 << keepmidcode[i].op << " " << keepmidcode[i].var1 << " " << keepmidcode[i].var2 << " " << keepmidcode[i].var3 << endl;
		}
	}
	ofstream midcode2;
	midcode2.open("15061025_钟瑞豪_优化后中间代码.txt");
	for (int i = 0; i < totalmidcode; i++) {
		if (keepmidcode[i].op == "func") {
			midcode2 << keepmidcode[i].var1 << " " << keepmidcode[i].var3 << "()" << endl;
		}
		else if (keepmidcode[i].op == "para") {
			midcode2 << keepmidcode[i].op << " " << keepmidcode[i].var1 << " " << keepmidcode[i].var3 << endl;
		}
		else if (keepmidcode[i].op == "push") {
			midcode2 << keepmidcode[i].op << " " << keepmidcode[i].var1 << endl;
		}
		else if (keepmidcode[i].op == "call") {
			midcode2 << keepmidcode[i].op << " " << keepmidcode[i].var1 << endl;
			midcode2 << keepmidcode[i].var3 << " = RET" << endl;
		}
		else if (keepmidcode[i].op == "ret") {
			midcode2 << "ret " << keepmidcode[i].var3 << endl;
		}
		else if (keepmidcode[i].op == "int" || keepmidcode[i].op == "char") {
			midcode2 << "var " << keepmidcode[i].op << " " << keepmidcode[i].var3 << endl;
		}
		else if (keepmidcode[i].op == "ints" || keepmidcode[i].op == "chars") {
			midcode2 << "var " << keepmidcode[i].op << " " << keepmidcode[i].var3 << "[" << keepmidcode[i].var2 << "]" << endl;
		}
		else if (keepmidcode[i].op == "const") {
			if (keepmidcode[i].var1 == "char")
				midcode2 << "const " << keepmidcode[i].var1 << " " << keepmidcode[i].var3 << " = " << (char)(stoi(keepmidcode[i].var2)) << endl;
			else
				midcode2 << "const " << keepmidcode[i].var1 << " " << keepmidcode[i].var3 << " = " << keepmidcode[i].var2 << endl;
		}
		else if (keepmidcode[i].op == "plus") {
			midcode2 << keepmidcode[i].var3 << " = " << keepmidcode[i].var1 << " + " << keepmidcode[i].var2 << endl;
		}
		else if (keepmidcode[i].op == "minus") {
			midcode2 << keepmidcode[i].var3 << " = " << keepmidcode[i].var1 << " - " << keepmidcode[i].var2 << endl;
		}
		else if (keepmidcode[i].op == "mul") {
			midcode2 << keepmidcode[i].var3 << " = " << keepmidcode[i].var1 << " * " << keepmidcode[i].var2 << endl;
		}
		else if (keepmidcode[i].op == "div") {
			midcode2 << keepmidcode[i].var3 << " = " << keepmidcode[i].var1 << " / " << keepmidcode[i].var2 << endl;
		}
		else if (keepmidcode[i].op == "=") {
			midcode2 << keepmidcode[i].var3 << " = " << keepmidcode[i].var1 << endl;
		}
		else if (keepmidcode[i].op == "geta") {
			midcode2 << keepmidcode[i].var3 << " = " << keepmidcode[i].var1 << "[" << keepmidcode[i].var2 << "]" << endl;
		}
		else if (keepmidcode[i].op == "[]=") {
			midcode2 << keepmidcode[i].var3 << "[" << keepmidcode[i].var2 << "]" << " = " << keepmidcode[i].var1 << endl;
		}
		else if (keepmidcode[i].op == ">" || keepmidcode[i].op == ">=" || keepmidcode[i].op == "<" || keepmidcode[i].op == "<=" || keepmidcode[i].op == "==" || keepmidcode[i].op == "!=") {
			midcode2 << keepmidcode[i].var1 << " " << keepmidcode[i].op << " " << keepmidcode[i].var2 << endl;
		}
		else if (keepmidcode[i].op == "BZ" || keepmidcode[i].op == "GOTO") {
			midcode2 << keepmidcode[i].op << " " << keepmidcode[i].var3 << endl;
		}
		else if (keepmidcode[i].op == "lab:") {
			midcode2 << keepmidcode[i].var3 << ":" << endl;
		}
		else {
			midcode2 << keepmidcode[i].op << " " << keepmidcode[i].var1 << " " << keepmidcode[i].var2 << " " << keepmidcode[i].var3 << endl;
		}
	}
}

void Code_Generator::outsymbol()
{
	ofstream output;
	output.open("symbols.txt");
	cout << totalkeepaddr << endl;
	for (int i = 0; i < totalkeepaddr; i++) {
		output << keepaddr[i].name << " " << keepaddr[i].isglobal << " " << keepaddr[i].addr << endl;
	}
}
