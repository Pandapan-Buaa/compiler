#ifndef TYPE__H
#define TYPE__H

#include <string>
#include <vector>
#include <map>
#include<iostream>
using std::string;
using std::vector;
using std::map;
using namespace std;
enum SymbolType {
	ILLEGAL,            // 非法字符
	FINISH,             // 文件结尾

	IDENFR,     // 标识符
	INTCON,     // 整数常量
	CHARCON,    // 字符常量
	STRCON,     // 字符串

	CONSTTK,    //const关键字
	INTTK,      //int关键字
	CHARTK,     //char关键字
	VOIDTK,     //void关键字
	MAINTK,     //main关键字
	IFTK,       //if关键字
	ELSETK,     //else关键字
	DOTK,       //do关键字
	WHILETK,    //while关键字
	FORTK,      //for关键字
	SCANFTK,    //scanf关键字
	PRINTFTK,   //printf关键字
	RETURNTK,   //return关键字

	PLUS,       //加号 +
	MINU,       //减号 -
	MULT,       //乘号 *
	DIV,        //除号 /
	LSS,        //小于号 <
	LEQ,        //小于等于号 <=
	GRE,        //大于号 >
	GEQ,        //大于等于号 >=
	EQL,        //等于号 ==
	NEQ,        //不等号 !=
	ASSIGN,     //赋值号 =
	SEMICN,     //分号 ;
	COMMA,      //逗号 ,
	LPARENT,    //左圆括号 (
	RPARENT,    //右园括号 )
	LBRACK,     //左方括号 [
	RBRACK,     //右方括号 ]
	LBRACE,     //左尖括号 {
	RBRACE      //右尖括号 }
};

namespace name_OP {
	enum Operator {
		PLUS,    // 加法
		MINU,    // 减法
		MULT,    // 乘法
		DIV,    // 除法
		NEG,    // 求负
		ASSIGN,    // 赋值
		ARAS,   // 给数组元素赋值
		ASAR,   // 将数组元素赋值给变量

		LAB,    // 标签
		CMP,    // 比较
		JG,     // 大于跳转
		JGE,    // 大于或等于跳转
		JL,     // 小于跳转
		JLE,    // 小于或等于跳转
		JE,     // 等于跳转
		JNE,    // 不等于跳转
		JMP,    // 无条件跳转

		CALF,   // 函数调用，带返回值
		RET,    // 函数返回
		FRET,    //返回语句,
		PUSH,   // 压栈
		FUNCEND,

		RI,     // 读整数
		RC,     // 读字符
		WS,     // 写字符串
		WI,     // 写整数
		WC,     // 写字符
		NL     //新行
	};
}

enum ErrorType {
	ILLEGAL_CHAR,   // a 非法符号或不符合词法
	REDEFINE,       // b 名字重定义
	UNDEFINED,      // c 未定义的名字
	WRONG_ARG_NUM,   //d 函数参数个数不匹配
	WRONG_ARG_TYPE,   //e 函数参数类型不匹配
	ILLEGAL_IFCONDITION, //f 条件判断中出现不合法的类型
	ILLEGAL_VOIDFUNCRETURN, //g 无返回值的函数存在不匹配的return语句
	ILLEGAL_FUNCRETURN, //h 有返回值的函数缺少return语句或存在不匹配的return语句
	ILLEGAL_ARRAYNUM, //i 数组元素的下标只能是整型表达式
	CONST_CHANGE, //j 不能改变常量的值
	SHOULD_BE_SEMICN, // k 应为分号
	SHOULD_BE_RPARENT, // l 应为右小括号’)’
	SHOULD_BE_RBRACK, // m 应为右中括号’]’
	WITHOUT_WHILE, // n do-while应为语句中缺少while
	ILLEGAL_CONST_ASSIGN //o 常量定义中=后面只能是整型或字符型常量
};

// 标识符的类型
enum IdentifierType {
	VAR,
	CONST,
	ARRAY,
	FUNCTION
};
// 基本类型
enum BasicType {
	INTEGER,
	CHAR,
	VOID
};

// 每个参数的信息
struct Argument {
	string name;    // 参数名
	BasicType type; // 类型
	bool vary;      // true 表示传地址，false 表示传值

	// 构造函数
	Argument(const string& n, BasicType t, bool v) : name(n), type(t), vary(v) {}
};

// 符号表结构
struct STE {
	string name;                    // 标识符名称
	IdentifierType type;            // 标识符类型
	BasicType valueType;            // 常量、变量：值的类型；
	// 数组：元素的类型；
	// 函数：返回值类型
	int multiInt;                   // 常量：值；// 数组：容量限 //函数 临时变量个数
	int paranum;

	int location;                   // 变量/数组在运行栈上的相对地址
	vector<Argument>* arguments;    // 函数/过程的参数列表
	////////////////////////////////////////////////////////////////
private:    // 以下，为构建树形符号表而必须的指针
	STE* father;                    // 当前符号的父节点（所属函数/过程）
	map<string, STE*>* childs;     // 当前符号(函数/过程)下的符号
	friend class SymbolTable_Analysis;// 把数据结构相关的指针暴露给符号表管理器
	////////////////////////////////////////////////////////////////
public:
	STE() :
		arguments(NULL),
		father(NULL),
		childs(NULL),
		location(0)
	{}     // 构造函数：给参数列表置为 NULL
	~STE() {
		delete childs;      // 先删除子树，再删除当前节点
		delete arguments;
	}                       // 析构函数：先删除参数列表
	// 慎用！析构时会删除以当前节点为根的子树！！
	STE(const STE& ste) {
		name = ste.name;
		type = ste.type;
		valueType = ste.valueType;
		multiInt = ste.multiInt;
		location = ste.location;
		paranum = ste.paranum;
		// 对这个指针要格外小心！！！
		arguments = ste.arguments == NULL ? NULL :
			new vector<Argument>(*ste.arguments);
		// 数据结构相关的指针留着以后处理。
		father = NULL;
		childs = NULL;
	}                       // 拷贝构造
};


#endif
