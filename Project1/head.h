#pragma once
#ifndef _TEST_H_
#define _TEST_H_
#pragma once
#include<iostream>
#include<fstream>
#include <sstream>
#include<string.h>
#include<stack>
#include<vector>
#include<regex>
#include<map>
#include<list>
#include<set>
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<iomanip>
//#include "Syn2_detail.cpp"
using namespace std;
#define C_RULES "Cgrammer.txt"
///==============================================================================
extern string kt[33]; //�ؼ��ֱ�
extern string pt[45]; //�����
extern string it[100];                                              //�����ʶ��
extern string ct[100];                                              //���峣��
extern string st[100];                                              //�����ַ���
extern string zt[100];                                              //�����ַ� 	


enum tokenType {  //�����Ӧֵ
	_keyword = 1, //�ؼ���
	_identifier,  //��ʶ��
	_delimiter,   //���
	_constnum,    //��������
	_character,   //�ַ�
	_string,     //�ַ���
	//_notes, //ע��(���������Token��)	_space  //�հ׷�(���������Token��)
};

typedef struct token {
	tokenType type;   //����
	int id;			  //��Ӧ���ű��е��±�
	int row;         //�õ�����Դ�����ļ��е����������ڼ��ʱ���������ֵ�����

}token_struct;

extern vector < pair<string, token_struct> > tokenList;
extern int numkt;
extern int numit;
extern int numpt;
extern int numct;
extern int numst;
extern int numzt;
extern int line_num;
extern int _charnum;//��¼��ȡ�ļ�ʱ�����λ�� 
extern int get_tag;
extern string in_path;
///===========================================================================



typedef struct fund
{
	string token;
	char ty;
	int num;
} Fund;

extern vector<Fund> obj;
extern Fund x, w;
extern stack<Fund> s1;//

extern int k, l;                 //Ϊ�˲������ 

extern int flag1;
extern int flag2, flag3;            //��Ϊ����ѭ��������

typedef struct symbolTa {   //���ű���ʶ����
	string name;       //��ʶ��Դ��
	int ty;           //ָ�����ͱ��±�
	int offset;        //ƫ����
	int prelayer;        //ǰһ�����ű�
	string domainName; //����
	string mark;      //��Ծ��Ϣ��ע   "Y"/"N"
	string if_temp;      //��־���Ƿ�Ϊ��ʱ����     "t"/"nt"
} SymbolTa;
extern SymbolTa tmpsym; //��ʼ��ȫ�ַ��ű�


extern vector<vector<SymbolTa> > symb;       //�������ű�



typedef struct fund1
{
	string p;
	string a1;
	string a2;
	string ob;
} Fund1;

extern Fund1 f1;
extern vector<Fund1> obj1;//��Ԫʽ��
extern vector<vector<Fund1>> oobj,oobj2,oobj3;//??

extern stack<pair<string, int>> SEEM;            //����ջ






extern string j, t1, t2;
extern int f;

extern int off;

extern int s[17][15];

extern stack<string> SEM;
extern stack<string> Sy;
extern stack<string> Type;

extern int t;
//==========================

extern int value;              //������ĸ�����ʽ 
extern map<pair<string, string>, int> tmp;                 //�Զ����ɷ�����Ĳ��� 
extern int a[100][100];//LL1������
extern set<string> ter, non_ter;
extern int qtID;

//==============================���������Ż�
typedef struct node
{
	int num;
	int left, right;//��Ӧ�� 
	vector<string> v;                //��¼���е�����ǣ��±�Ϊ0��+�α�ǣ��±��0�� 
	int ty;               //����Ƿ���Ч 
	string op;
} Node;

extern vector <Node> DAG;
extern int kk1, kk2;
extern int k1, l1, l2, k2;
extern int k, l;
extern int nn;

//==============================������������Ŀ�����
typedef struct code                      //���ɵ�Ŀ����� 
{
	string p;
	string source;
	string target;
} Code;
extern vector<Code> targe;
extern vector<Code> tar;

extern pair<string, string> RX;                 //�Ĵ����Լ���Ӧֵ��Ӧ�Ļ�Ծ��Ϣ
extern stack<int> SEMM;                    //��ӦĿ��������ɵ�����ջ�����ڷ��


void start();
void equal();
void action_fun(string Act, int Tokenstation, string nowstr);
void better1();
bool isnumm(string s);
bool is_exist_nsy(string k);

void start2();
#endif
