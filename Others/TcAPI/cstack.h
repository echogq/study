#include <iostream>
#include <sstream>
#include <list>
#include <numeric>
#include <algorithm>

using namespace std;

//创建一个list容器的实例LISTINT
//typedef list<int> LISTINT;

//创建一个list容器的实例LISTSTRING
typedef list<string> LISTSTRING;

class CStackStr //这里第一个字母应该是小写
{
private:
	//用LISTSTRING创建一个名为listOne的list对象
	LISTSTRING listTwo;
	//声明i为迭代器
	LISTSTRING::iterator j;
public:
	CStackStr(){}; //top初始化在这里完成
	void push(string str);
	string pop();
	bool isEmpty();
	bool isFull();
	int size();
	void dump();
};
void CStackStr::push(string str) //这里应该写明返回类型
{
	listTwo.push_front (str);
}
string CStackStr::pop() //这里返回类型应该放在句首
{
	string retStr = string(*(listTwo.begin()));
	listTwo.pop_front ();

	return retStr; //返回栈顶
}
int CStackStr::size() 
{
	return listTwo.size();
}
bool CStackStr::isEmpty() //判断是否为空 //这里返回类型应该放在句首
{
	if(listTwo.size()<=0)return 1;
	else return 0;
}

void CStackStr::dump() //这里返回类型应该放在句首
{
	//从前向后显示listTwo中的数据
	cout<<"listTwo.begin()---listTwo.end():"<<endl;
	for (j = listTwo.begin(); j != listTwo.end(); ++j)
		::OutputDebugString(string(*j).c_str());
	cout << endl;
}

CStackStr myStack;