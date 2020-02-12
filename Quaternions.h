#ifndef QUATERNIONS__H
#define QUATERNIONS__H
#include "type.h"
#include <fstream>
using std::ofstream;
using name_OP::Operator;

struct Quaternion {
	Operator op;
	string oper1;
	string oper2;
	string dest;
	STE* ste;
	Quaternion(Operator ope, const string& o1, const string& o2, const string& d, STE* ste) :
		op(ope), oper1(o1), oper2(o2), dest(d), prenode(NULL), sucnode(NULL), ste(ste) {}
private:
	// 与链表结构相关的指针域
	Quaternion* prenode; // 前驱节点
	Quaternion* sucnode; // 后继节点
	// 友元类，可以操作链表结构
	friend class Quaternions;
};
class Quaternions
{
public:
	void addTail(Quaternion*);      // 往结尾加一个四元式
	void output(ofstream&);        // 输出四元式到文件
	Quaternions();                   // 构造函数
	~Quaternions();                  // 析构函数
	class iterator {
	private:
		Quaternion* point;
		iterator(Quaternion* p) : point(p) {}
		friend class Quaternions;
	public:
		bool operator== (const iterator& b) const {
			return point == b.point;
		}
		bool operator!= (const iterator& b) const {
			return point != b.point;
		}
		iterator& operator++ (int i) {
			return operator++();
		}
		iterator& operator++ () {
			point = point->sucnode;
			return *this;
		}
		iterator& operator-- (int i) {
			return operator--();
		}
		iterator& operator-- () {
			point = point->prenode;
			return *this;
		}
		Quaternion& operator* () {
			return *point;
		}
		Quaternion* operator-> () {
			return point;
		}
	};
	iterator& begin() {
		static iterator it(head);
		it.point = head->sucnode;
		return it;
	}
	iterator& end() {
		static iterator it(NULL);
		return it;
	}

private:
	Quaternion* head;    // 四元式链表的头节点
	Quaternion* tail;    // 四元式链表的尾节点
	// 禁用拷贝构造
	Quaternions(const Quaternions&);
};

#endif
