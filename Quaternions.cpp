#include "Quaternions.h"
using namespace std;
using namespace name_OP;

Quaternions::Quaternions() {
	head = new Quaternion(Operator::LAB, "Hello", "", "", NULL);
	tail = head;
}

Quaternions::~Quaternions() {
	while (head != NULL) {
		tail = head->sucnode;
		delete head;
		head = tail;
	}
}

ostream& operator<< (ostream& os, Operator op) {
#define case(op) case Operator::op: \
    os << #op; break
	switch (op) {
		case(PLUS);    // 加法
			case(MINU);    // 减法
				case(MULT);    // 乘法
					case(DIV);    // 除法
						case(NEG);    // 求负
							case(ASSIGN);    // 赋值
								case(ARAS);   // 给数组元素赋值
									case(ASAR);   // 将数组元素赋值给变量
										case(LAB);    // 标签
											case(CMP);    // 比较
												case(JG);     // 大于跳转
													case(JGE);    // 大于或等于跳转
														case(JL);     // 小于跳转
															case(JLE);    // 小于或等于跳转
																case(JE);     // 等于跳转
																	case(JNE);    // 不等于跳转
																		case(JMP);    // 无条件跳转
																			case(CALF);   // 函数调用，带返回值
																				case(RET);    // 函数或过程返回
																					case(PUSH);   // 压栈
																						case(RI);     // 读整数
																							case(RC);     // 读字符
																								case(WS);     // 写字符串
																									case(WI);     // 写整数
																										case(WC);     // 写字符
																											case(FUNCEND);
																												case(NL);
																													case(FRET);
																													default:
																														break;
	}
	return os;
#undef case
}

void Quaternions::addTail(Quaternion* Quaternion) {
	Quaternion->prenode = tail;
	Quaternion->sucnode = tail->sucnode;
	tail->sucnode = Quaternion;
	tail = Quaternion;
	/////////////////////////////////////
	//    static ofstream ofs("fuck~~~~~~~~~~~~~~~~~~~~.txt");
	//    Quaternion *pq = Quaternion;
	//    ofs << pq->op << " ";
	//    bool out = false;
	//    if (!pq->oper1.empty()) {
	//        ofs << pq->oper1;
	//        out = true;
	//    }
	//    if (!pq->oper2.empty()) {
	//        ofs << (out ? ", " : "");
	//        ofs << pq->oper2;
	//        out = true;
	//    }
	//    if (!pq->dest.empty()) {
	//        ofs << (out ? ", " : "");
	//        ofs << pq->dest;
	//    }
	//    ofs << endl;
	/////////////////////////////////////
}

void Quaternions::output(ofstream& ofs) {
	/*
	Quaternion* pq;
	for (pq = head->sucnode; pq != NULL; pq = pq->sucnode) {
		ofs << pq->op << " ";
		bool out = false;
		if (!pq->oper1.empty()) {
			ofs << pq->oper1;
			out = true;
		}
		if (!pq->oper2.empty()) {
			ofs << (out ? ", " : "");
			ofs << pq->oper2;
			out = true;
		}
		if (!pq->dest.empty()) {
			ofs << (out ? ", " : "");
			ofs << pq->dest;
		}
		/*if (pq->ste != NULL)
			ofs << "   ste*" << pq->ste->name;
	ofs << endl;
}	*/
}
