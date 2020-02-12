#include "SymbolTable_Analysis.h"

using namespace std;
static STE root;
static vector<string> stringMap;

SymbolTable_Analysis& SymbolTable_Analysis::initSymbolTable() {
	static SymbolTable_Analysis theSymbolTable_Analysis;
	stringMap.push_back("");
	return theSymbolTable_Analysis;
}

SymbolTable_Analysis::SymbolTable_Analysis() : ste(NULL), p(0), position(&root) {}

// 将 position 往里推进一层，必须保证 block 已经正确插入到符号表中
void SymbolTable_Analysis::goIn(const string& block) {
	map<string, STE*>::iterator it = position->childs->find(block);
	if (it != position->childs->end()) {
		position = it->second;
		p++;
	}
}

// 将 position 往外退回一层
void SymbolTable_Analysis::goOut() {
	if (position->father != NULL) {
		position = position->father;
		p--;
	}

}

// 判断表中当前层是否有这个符号
bool SymbolTable_Analysis::isHere(const string& id) {
	if (position->childs != NULL)
		return position->childs->count(id) > 0 || id == position->name;
	return id == position->name;
}


// 查表，查到返回 true,并把查到的表项的指针赋给 ste 若没有查到，则将 ste 置为 NULL 并返回 false
bool SymbolTable_Analysis::find(const string& id) {
	STE* ps = position->childs != NULL ? position : position->father;
	while (ps != NULL) {
		map<string, STE*>::iterator it = ps->childs->find(id);
		if (it != ps->childs->end()) {
			ste = it->second;
			return true;
		}
		ps = ps->father;
	}
	ste = NULL;
	return false;
}

// 填表, 将 ste 插入到表中当前层（position层）
void SymbolTable_Analysis::insert() {
	ste->father = position;
	if (position->childs == NULL)
		position->childs = new map<string, STE*>;
	position->childs->insert(pair<string, STE*>(ste->name, ste));
}

void SymbolTable_Analysis::defineArguments(STE* te, vector<Argument>& args) {
	// 清空原参数表
	if (te->arguments != NULL)
		delete te->arguments;
	te->arguments = new vector<Argument>(args);
	// 确保子节点 map 有效
	if (te->childs == NULL)
		te->childs = new map<string, STE*>;
	// 逐个把参数作为 te 的局部变量
	int count = 0;
	for (vector<Argument>::iterator it = args.begin(); it != args.end(); it++) {
		count++;
		STE* nte = new STE;
		nte->name = it->name;
		nte->type = IdentifierType::VAR;
		nte->valueType = it->type;
		nte->father = te;
		te->childs->insert(pair<string, STE*>(it->name, nte));
	}
	te->paranum = count;
}

int SymbolTable_Analysis::addString(const string& str)
{
	int i;
	for (i = 0; i < stringMap.size(); i++) {
		if (stringMap.at(i) == str)
			return i;
	}
	if (i == stringMap.size()) {
		stringMap.push_back(str);
		return i;
	}
}

vector<string> SymbolTable_Analysis::getStringMap()
{
	return stringMap;
}

string& SymbolTable_Analysis::encodeKey()
{
	static string idkey;
	idkey.clear();
	if (ste != NULL) {
		/*switch (ste->type) {
		case IdentifierType::ARRAY:
			idkey.append("array_");
			break;
		case IdentifierType::FUNCTION:
			idkey.append("func_");
			break;
		case IdentifierType::VAR:
			idkey.append("var_");
			break;
		default:
			idkey.append("const_");
			break;
		}*/
		idkey.append(ste->name);
		//idkey.append("_");
		//idkey.append(pointToStr(ste));
	}
	return idkey;
}

string& SymbolTable_Analysis::encodePosition()
{
	static string key;
	if (position == &root) {
		key = "main";
		return key;
	}
	else {
		ste = position;
		return encodeKey();
	}
}

/*void SymbolTable_Analysis::decodeKey(const string& idkey)
{
	if (idkey == "_main")
		ste = &root;
	else {
		string s = idkey.substr(idkey.size() - 8);
		unsigned int res = 0;
		for (int i = 0; i < 8; i++) {
			char ch = s[i];
			res <<= 4;
			if (isdigit(ch))
				ch -= '0';
			else if (isupper(ch))
				ch = ch - 'A' + 10;
			else
				ch = ch - 'a' + 10;
			res |= ch;
		}
		ste = (STE*)res;
	}
}*/

void SymbolTable_Analysis::setTempcount(int count)
{
	position->multiInt = count;
}

char* SymbolTable_Analysis::pointToStr(const void* point)
{
	static char str[9];
	std::sprintf(str, "%08X", point);
	return str;
}
