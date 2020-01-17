#include "head.h"
#include "lex.cpp"

using namespace std;

typedef struct production {
	string left; //����ʽ��
	vector<string> right;// [10]; //����ʽ�Ҳ�
	vector<string> right1;
}_production;
_production prod1[500];

class Token {		  //Token��
public:
	enum TokenType {  //�����Ӧֵ
		_keyword = 1, //�ؼ���
		_identifier,  //��ʶ��
		_delimiter,   //���
		_constnum,    //��������
		_character,   //�ַ�
		_string,      //�ַ���
		_space
	};
	TokenType type;   //����
	int id;			  //��Ӧ���ű��е��±�
	int row;          //�õ�����Դ�����ļ��е����������ڼ��ʱ���������ֵ�����
	Token() { row = 0; } 		    //�޲ι��췽��
	Token(TokenType type, int id) { //�вι��췽��
		this->type = type, this->id = id;
		this->row = 0;
	}
};

class Lex : public Token { //�﷨�������������ַ��ű�
public:
	struct re_table {
		re_table() {}
		vector<pair<regex, TokenType>> table{
			make_pair(regex("void|char|int|float"), _keyword),
			make_pair(regex("[a-zA-Z_][0-9a-zA-Z_]*"), _identifier),
			make_pair(regex("-?[0-9]+((\\.[0-9]+)([eE]-?[0-9]+)|(\\.[0-9]+)|([eE]-?[0-9]+))?[fF]?"), _constnum), //ƥ�両����
			make_pair(regex("-?(0|[1-9][0-9]*|0[xX][0-9a-fA-F]+|0[0-7]+)[uU]?[lL]{0,2}"), _constnum), //ƥ������������ʮ���ơ�ʮ�����ơ��˽��ƣ�
			make_pair(regex("\\+\\+|--|\\+=|-=|\\*=|/=|%=|<<=|>>=|\\|=|\\&=|\\^=|\\+|-|\\*|/|==|>>|<<|=|>=|>|<=|<|;|\\{|\\}|\\(|\\)|\\[|\\]|,|!|%|\\|\\||\\&\\&|\\||\\&|:|#|\\.|->|\\?|~|\\^"), _delimiter),
			make_pair(regex("['][\\s\\S^']?'"), _character),
			make_pair(regex("\".*\""), _string),
			make_pair(regex("\\s+"), _space) //ƥ��հ׷�
		};
	};
	Token tokenSeq[100]; //����token����
	int index_of_token = 0;
	string keyword_table[100] = { "void","float","char","int" }; //�ؼ��ֱ�
	string delimiter_table[100] = { "++","--","+=","-=","*=", "/=","%=", "<<=", ">>=","|=","&=",
		"^=","+","-","*","/","==",">>","<<","=",">=",">","<=","<",";","{","}","(",")","[","]",",","!","%",
		"||","&&","|","&",":","#",".","->","?","~","^" }; //�����
	int index_of_idtfer = 0;//��ʶ����
	string constant_table[100];
	int index_of_const = 0;//������
	string character_table[100];
	int index_of_char = 0;//�ַ���
	string string_table[100];
	int index_of_string = 0;//�ַ�����
};
class Syntax : public Lex {
public:
	typedef struct production {
		string left; //����ʽ��
		vector<string> right;// [10]; //����ʽ�Ҳ�
		vector<string> right1;
	}production;
	production prod[500];      //����ʽ���飬���ڴ��Ԥ�����Ĳ���ʽ
	int size_of_prod = 0;                    //����ʽ�����±�
	set<string> terminators, non_terminators;//�ս�����Ϻͷ��ս������	 
	map<string, set<string>> First, Follow;  //first���ϡ�follow���� [for example: First(alpha), map(alpha, {First(alpha})
	map<int, set<string>> Select;            //Select set
	map<string, bool> is_deduced_epsilon;    //���ս���Ƿ�����ƿ�
	map<string, vector<string>> tmpStorage;  //��ʱ�洢���һ���󲿷��ս����first����������Ҳ����ս��first����
	map<string, bool> visited;               //���ʱ�ǣ���������
	map<string, int> Action_map;             //���嶯����־ӳ��
	stack<pair<string, int>> SEM;            //����ջ      
	int TapelIndex;                          //���ͱ�����±�
	Syntax();                                //���캯��
	void preProcess();
	void dataRevision();
	void DFS_First();
	void getFirst(string curSymbol);
	set<string> int_first(vector<string> alpha);
	void DFS_Follow();
	void getFollow(string curSymbol);
	void getSelect();
	int list_of_LL1(string A, string a);
	map<pair<string, string>, int>  CreateLL1List();
	void Enter();
	void neigui();
	void file_enter();
};
Syntax::Syntax()
{
}
/*---------------------------------------------------------------------------------------------------------------
											Ԥ������(ɨ��һ���ķ������ļ���
����: 1. ����ս�����ϡ����ս������; 2. ���ս����first����; 3. �ж�ÿ�����ս���Ƿ�����ƿ�,���; 4. �ָ����ʽ�����Ҳ�.
----------------------------------------------------------------------------------------------------------------*/
void Syntax::preProcess() {
	ifstream in(C_RULES, ios::in); //��ȡC�ķ��ļ�
	if (!in.is_open()) { //�ķ��ļ��򿪳��������Ǹ��ļ����û�����ɾ������
		cout << "�ķ������ļ���ȡʧ�ܣ��Ҳ������ļ���" << endl;
		return;
	}
	string line_in_rules;
	int index_of_prod = 0;
	while (getline(in, line_in_rules))
	{
		int ptr_of_rules = 0; //һ���ķ�������ÿ���ַ��ķ���ָ��
		while (line_in_rules[ptr_of_rules] == ' ' || line_in_rules[ptr_of_rules] == '\t') ptr_of_rules++;//�˵��հ׷�
		if (line_in_rules[ptr_of_rules] == '#') continue; //ÿһ���ķ�����Ľ���
		if (isalpha(line_in_rules[ptr_of_rules]) == 2)
		{ //��ȡ����ʽ�󲿵ķ��ս��
			string nonTerm = ""; //��ʱ�洢���ս��
			while (line_in_rules[ptr_of_rules] != ' ' && line_in_rules[ptr_of_rules] != '\t')
			{	//�ָ��һ�����ս��
				nonTerm += line_in_rules[ptr_of_rules];
				ptr_of_rules++;
			}
			non_terminators.insert(nonTerm); //���÷��ս�����뵽���ս������
			prod[index_of_prod].left = nonTerm; //��¼�ò���ʽ����
			size_of_prod++; //����ʽ����+1
			while (line_in_rules[ptr_of_rules] == ' ' || line_in_rules[ptr_of_rules] == '\t' || line_in_rules[ptr_of_rules] == ':')
				ptr_of_rules++; //�˵�ð�źͿհ׷�
			while (ptr_of_rules < line_in_rules.size()) { //��¼����ʽ�Ҳ����ţ�˳����д�ս������
														  //һֱ�ߵ����н���
				string tmp = ""; //ƴ���Ҳ�һ������
				if (line_in_rules[ptr_of_rules] == '\'') {
					//��ǰ�������ս������Ҫ��¼���ս��
					ptr_of_rules++; //��ȥ������
					while (line_in_rules[ptr_of_rules] != '\'') {
						tmp += line_in_rules[ptr_of_rules];
						ptr_of_rules++;
					}//ƴ��һ���ս��
					terminators.insert(tmp); //�����ս����¼���ս������
					set<string> tmpset; //��ʱ���ϣ����ڴ洢�ս����First��
					tmpset.insert(tmp); //�ݴ��ս��
					First.insert(make_pair(tmp, tmpset)); //�ս����First�������䱾��
					prod[index_of_prod].right.push_back(tmp); //����ʽ�Ҳ���¼�÷���

					ptr_of_rules += 2; //��ȥ�ս������ĵ����źͿո�
				}
				else { //��ǰ������һ�����ս������һ�����嶯����־
					while (ptr_of_rules < line_in_rules.size() && line_in_rules[ptr_of_rules] != ' ') {
						tmp += line_in_rules[ptr_of_rules];
						ptr_of_rules++;
					}//ƴ��һ�����ս��
					ptr_of_rules++; //��ȥ�ķ�����֮��Ŀո�
					prod[index_of_prod].right.push_back(tmp); //�����ķ����Ż����嶯�����Ŵ浽����ʽ�Ҳ�vector��

					if (tmp == "epsilon") {
						//�ò���ʽ�󲿵ķ��ս������ֱ���ƿ�
						is_deduced_epsilon[prod[index_of_prod].left] = true;
					}
				}
			}
			index_of_prod++;
		}
		else if (line_in_rules[ptr_of_rules] == '|') {
			//һ�����ս���Ƴ�����������ʽ
			ptr_of_rules += 2; //��ȥ'|'�Ϳո�
			prod[index_of_prod].left = prod[index_of_prod - 1].left; //�ò���ʽ�󲿱ض�����һ������ʽ��һ����	
			size_of_prod++;
			while (ptr_of_rules < line_in_rules.size()) { //��¼����ʽ�Ҳ����ţ�˳����д�ս������
														  //һֱ�ߵ����н���
				string tmp = ""; //ƴ���Ҳ�һ������
				if (line_in_rules[ptr_of_rules] == '\'') {
					//��ǰ�������ս������Ҫ��¼���ս��
					ptr_of_rules++; //��ȥ������
					while (line_in_rules[ptr_of_rules] != '\'') {
						tmp += line_in_rules[ptr_of_rules];
						ptr_of_rules++;
					}//ƴ��һ���ս��
					terminators.insert(tmp); //�����ս����¼���ս������
					set<string> tmpset;
					tmpset.insert(tmp);
					First.insert(make_pair(tmp, tmpset));
					prod[index_of_prod].right.push_back(tmp); //����ʽ�Ҳ���¼�÷���

					ptr_of_rules += 2; //��ȥ�ս������ĵ����źͿո�
				}
				else { //��ǰ������һ�����ս�������嶯����־
					while (ptr_of_rules < line_in_rules.size() && line_in_rules[ptr_of_rules] != ' ') {
						tmp += line_in_rules[ptr_of_rules];
						ptr_of_rules++;
					}//ƴ��һ�����ս��
					ptr_of_rules++; //��ȥ�ķ�����֮��Ŀո�
					prod[index_of_prod].right.push_back(tmp); //�����ķ����Ŵ浽����ʽ�Ҳ�vector��
					string tmp1 = " ";
					prod[index_of_prod].right1.push_back(tmp1);
					if (tmp == "epsilon") {
						//�ò���ʽ�󲿵ķ��ս������ֱ���ƿ�
						is_deduced_epsilon[prod[index_of_prod].left] = true;
					}
				}
			}
			index_of_prod++;
		}
	}
	in.close(); //�ر��ļ�
	/* terminators.insert("int_const"); //������Ӽ����ս��
	First["int_const"].insert("int_const");
	terminators.insert("char_const");
	First["char_const"].insert("char_const");
	terminators.insert("id");
	First["id"].insert("id");
	terminators.insert("float_const");
	First["float_const"].insert("float_const");
	terminators.insert("string");
	First["string"].insert("string"); */

}

//�ж�һ���ַ����Ƿ�ȫΪ��д��ĸ,�Դ������ķ����ź����嶯������
bool isUpper(string str) {
	if (regex_match(str, regex("[A-Z_0-9]+")))return true;
	else return false;
}
/*---------------------------------------------------------------------------------------------------------------
						�ڶ���ɨ�辭��Ԥ����Ĳ���ʽ���飬���������пձ����ɿ��������first����.
----------------------------------------------------------------------------------------------------------------*/
void Syntax::dataRevision() { //��������--�ڶ���Ԥ���������һ��û�н��������
	//�ȵ���ɨһ��
	for (int i = size_of_prod - 1; i >= 0; i--) {
		if (!is_deduced_epsilon[prod[i].left]) { //ԭ���ƿձ�־Ϊfalse
			int index_of_right = 0;
			//bool isAction = false; //�Ƿ�Ϊ���嶯����־
			while (index_of_right < prod[i].right.size() &&
				(non_terminators.find(prod[i].right[index_of_right]) != non_terminators.end()
					|| isUpper(prod[i].right[index_of_right]))) {
				//�������ʽ�Ҷ�Ϊ���ս�������嶯����־
				if (isUpper(prod[i].right[index_of_right])) index_of_right++; //���嶯����־ֱ������
				else { //�Ƿ��ս��
					if (!is_deduced_epsilon[prod[i].right[index_of_right]]) {
						is_deduced_epsilon[prod[i].left] = false;
						break;
					}
					else index_of_right++;
				}
			}
			if (index_of_right >= prod[i].right.size()) { //����ʽ�Ҷ����з��Ŷ������ƿ�
				is_deduced_epsilon[prod[i].left] = true;
				First[prod[i].left].insert("epsilon");
			}
		}
	}
	//������ɨһ��
	for (int i = 0; i < size_of_prod; i++) {
		if (!is_deduced_epsilon[prod[i].left]) { //ԭ���ƿձ�־Ϊfalse
			int index_of_right = 0;
			while (index_of_right < prod[i].right.size() &&
				(non_terminators.find(prod[i].right[index_of_right]) != non_terminators.end()
					|| isUpper(prod[i].right[index_of_right]))) {
				//�������ʽ�Ҷ�Ϊ���ս�������嶯����־
				if (isUpper(prod[i].right[index_of_right])) index_of_right++; //���嶯����־ֱ������
				else { //�Ƿ��ս��
					if (!is_deduced_epsilon[prod[i].right[index_of_right]]) {
						is_deduced_epsilon[prod[i].left] = false;
						break;
					}
					else index_of_right++;
				}
			}
			if (index_of_right >= prod[i].right.size()) { //����ʽ�Ҷ����з��Ŷ������ƿ�
				is_deduced_epsilon[prod[i].left] = true;
				First[prod[i].left].insert("epsilon");
			}
		}
	}
}
/*---------------------------------------------------------------------------------------------------------------
					���һ���������������ɨ�����ʽ����first���������, ����������DFS_First()
										��First(T), T -> Vn or Vt
----------------------------------------------------------------------------------------------------------------*/
void Syntax::DFS_First() {
	for (set<string>::iterator it = non_terminators.begin(); it != non_terminators.end(); it++)
		visited.insert(make_pair(*it, false)); //��ʼ��
	for (set<string>::iterator it = non_terminators.begin(); it != non_terminators.end(); it++)
	{
		if (!visited[*it]) //����÷���δ������
			getFirst(*it);//����֮
	}
}
void Syntax::getFirst(string curSymbol) {

	visited[curSymbol] = true; //���÷��ŵķ��ʱ�־��Ϊtrue�������ѱ�����
	for (int index_prod = 0; index_prod < size_of_prod; index_prod++)
	{	//��������ʽ��

		if (prod[index_prod].left == curSymbol) { //�ҵ��÷��Ŷ�Ӧ�Ĳ���ʽ
			int index_right = 0; //��ǰ����ʽ�Ҳ����ż�������
			if (isUpper(prod[index_prod].right[index_right])) index_right++; //����Ҳ��׷�����һ�������־��������
			if (non_terminators.find(prod[index_prod].right[index_right]) != non_terminators.end()) {
				//����ʽ�Ҳ��׷���Ϊ���ս��
				vector<string> tmpvec;
				tmpvec.push_back(prod[index_prod].right[index_right]); //��ʽת�� string -> vector<string>
				tmpStorage.insert(make_pair(prod[index_prod].left, tmpvec)); //�ݴ�֮
				if (!visited[prod[index_prod].right[index_right]]) //����÷���δ�����ʹ�!!!!!!!!
					getFirst(prod[index_prod].right[index_right]); //��ȡ�÷��ŵ�first����
				set<string> t_set;
				t_set = First[prod[index_prod].right[index_right]];
				if (is_deduced_epsilon[prod[index_prod].right[index_right]]) {
					//����÷��ſ����ƿ�  First type is: map<string, set<string>>
					t_set.erase("epsilon");
				}
				First[prod[index_prod].left].insert(t_set.begin(), t_set.end()); //�ϲ����󲿷��ŵ�first����
				while (is_deduced_epsilon[prod[index_prod].right[index_right]]) {
					//����ò���ʽ�Ҳ��׷��ſ����ƿ� 
				next_symb: index_right++; //�����һ��������һ������
					if (index_right < prod[index_prod].right.size())
					{	//����Ҳ��÷��ź��滹�з���
						if (isUpper(prod[index_prod].right[index_right])) {
							//�������嶯����־,����
							goto next_symb;
						}
						if (non_terminators.find(prod[index_prod].right[index_right]) != non_terminators.end()) {
							//��������������Ҳ�Ƿ��ս��
							tmpvec.push_back(prod[index_prod].right[index_right]);
							tmpStorage[prod[index_prod].left] = tmpvec; //�ݴ�֮
							if (!visited[prod[index_prod].right[index_right]]) //����÷���δ�����ʹ�!!!!!!
								getFirst(prod[index_prod].right[index_right]); //����֮
							//else { //����÷����ѱ����ʹ�
							set<string> _set;
							_set = First[prod[index_prod].right[index_right]];
							if (is_deduced_epsilon[prod[index_prod].right[index_right]]) {
								//����÷��ſ����ƿ�  First type is: map<string, set<string>>
								_set.erase("epsilon");
							}
							First[prod[index_prod].left].insert(_set.begin(), _set.end()); //�ϲ����󲿷��ŵ�first����
						}
						else First[prod[index_prod].left].insert(prod[index_prod].right[index_right]);
						//��������������ս���������󲿷���first������
					}
					else break;
				}
			}
			//����ʽ�Ҳ��׷���Ϊ�ս����@, ��֮�����󲿷��ŵ�first����
			else {
				First[prod[index_prod].left].insert(prod[index_prod].right[index_right]);
			}
		}
	}
}
/*---------------------------------------------------------------------------------------------------------------
						��First(alpha),alpha -> (Vn + Vt)*��������Select����First��ں�����
----------------------------------------------------------------------------------------------------------------*/
set<string> Syntax::int_first(vector<string> alpha)//first�ӿ�--��ں���
{
	int index_right = 0;
	if (regex_match(alpha[index_right], regex("[A-Z_0-9]+")))index_right++; //�׷�Ϊ���嶯����־������
	if (alpha[index_right] == "epsilon" || terminators.find(alpha[index_right]) != terminators.end()
		|| (non_terminators.find(alpha[index_right]) != non_terminators.end() && !is_deduced_epsilon[alpha[index_right]]))
	{
		//�������ʽ�Ҳ��׷����Ǧ� �� �ս�� �� �������ƿյķ��ս��
		set<string> tmpset;
		if (alpha[index_right] == "epsilon")
		{ //���ĳ������ʽ�Ҳ�����һ����
			tmpset.insert("epsilon");
			return tmpset;
		}
		return First[alpha[index_right]];
	}
	else {//����ʽ�Ҳ��׷����ǿ����ƿյķ��ս��
		set<string> tmpset;
		while (index_right < alpha.size() &&
			((non_terminators.find(alpha[index_right]) != non_terminators.end() &&
				is_deduced_epsilon[alpha[index_right]]) || isUpper(alpha[index_right]))) {
			//�������ʽ�Ҳ��ǿ����ƿյķ��ս��
			if (regex_match(alpha[index_right], regex("[A-Z_0-9]+"))) {
				//�������嶯�����ţ�������ֱ��ƥ�������һ���̶������Ч�ʣ����ٳ�����ת��
				index_right++;
				continue;
			}
			tmpset.insert(First[alpha[index_right]].begin(), First[alpha[index_right]].end());
			tmpset.erase("epsilon");
			index_right++;
		}
		if (index_right >= alpha.size()) {
			//�Ҳ�ȫ�����ƿ�
			tmpset.insert("epsilon");
		}
		else { //�������޷��ƿյķ���
			tmpset.insert(First[alpha[index_right]].begin(), First[alpha[index_right]].end());
		}
		return tmpset;
	}
}
/*---------------------------------------------------------------------------------------------------------------
												��Follow����
----------------------------------------------------------------------------------------------------------------*/
void Syntax::DFS_Follow()
{
	for (set<string>::iterator it = non_terminators.begin(); it != non_terminators.end(); it++)
		visited[*it] = false; //�ٴγ�ʼ�����ʱ�־����
	for (set<string>::iterator it = non_terminators.begin(); it != non_terminators.end(); it++)
		if (!visited[*it])
			getFollow(*it);
}
void Syntax::getFollow(string curSymbol) {//First type is: map<string, set<string>>
	visited[curSymbol] = true;
	if (curSymbol == prod[0].left)//�����ǰ����Ϊ��ʼ���ţ��Ҹ÷��ŵ�follow����δ����
		Follow[curSymbol].insert("��");
	for (int i = 0; i < size_of_prod; i++) {
		//��������ʽ��
		int index_right = 0;
		vector<string>::iterator it = find(prod[i].right.begin(), prod[i].right.end(), curSymbol);
		while (it != prod[i].right.end())
		{ //����ò���ʽ�Ҳ������ҵ��������
			index_right = it - prod[i].right.begin(); //������ת��Ϊ����ֵ
			index_right++;
			if (index_right < prod[i].right.size() && regex_match(prod[i].right[index_right], regex("[A-Z_0-9]+"))) index_right++; //�÷��ź�����һ�������־������
			while (index_right < prod[i].right.size() && (is_deduced_epsilon[prod[i].right[index_right]]
				|| isUpper(prod[i].right[index_right]))) {
				//�������ʽ�Ҳ��÷��ս�����滹�з��� �Һ���ķ��ſ����ƿջ��ߺ�����һ�������־
				if (regex_match(prod[i].right[index_right], regex("[A-Z_0-9]+"))) {
					index_right++;
					continue;
				} //�������嶯����־��������
				set<string> tmp;
				tmp = First[prod[i].right[index_right]];
				tmp.erase("epsilon");
				Follow[prod[i].right[it - prod[i].right.begin()]].insert(tmp.begin(), tmp.end());
				index_right++;
			}
			if (index_right < prod[i].right.size() && !is_deduced_epsilon[prod[i].right[index_right]]) //�����������һ�������Ʋ�����, ������First��Ҳ�ӽ���
			{
				Follow[prod[i].right[it - prod[i].right.begin()]].insert(First[prod[i].right[index_right]].begin(), First[prod[i].right[index_right]].end());
				it = find(prod[i].right.begin() + index_right, prod[i].right.end(), curSymbol);
			}
			if (index_right >= prod[i].right.size()) //�������ķ���ȫ�������ƿ�
			{
				if (!visited[prod[i].left]) getFollow(prod[i].left); //û�з��ʹ�������֮
				Follow[prod[i].right[it - prod[i].right.begin()]].insert(Follow[prod[i].left].begin(), Follow[prod[i].left].end()); //���ʹ����Ѹò���ʽ�󲿷��ŵ�Follow���ӽ���
				it = prod[i].right.end();
			}

		}
	}
}
/*---------------------------------------------------------------------------------------------------------------
												��Select����
----------------------------------------------------------------------------------------------------------------*/
void Syntax::getSelect() {
	//��select����
	for (int i = 0; i < size_of_prod; i++) {
		//��������ʽ����
		//bool is_all_epsilon = false;
		int index_right = 0;
		while (index_right < prod[i].right.size()
			&& ((non_terminators.find(prod[i].right[index_right]) != non_terminators.end()
				&& is_deduced_epsilon[prod[i].right[index_right]])
				|| prod[i].right[0] == "epsilon" || isUpper(prod[i].right[index_right]))) {
			//����ò���ʽ�Ҳ��׷���Ϊ���ս�� �� �����ƿ�
			index_right++;
		}
		if (index_right >= prod[i].right.size()) //�������ʽ�Ҳ�����ȫ�������ƿ�, alpha =>* epsilon
		{
			set<string> tmpset, first, follow;
			first = int_first(prod[i].right);
			first.erase("epsilon");
			follow = Follow[prod[i].left];
			set_union(first.begin(), first.end(), follow.begin(), follow.end(), inserter(tmpset, tmpset.begin()));
			Select[i] = tmpset;
		}
		else { //alpha ��>* epsilon
			Select[i] = int_first(prod[i].right);
		}
	}
}
/*---------------------------------------------------------------------------------------------------------------
												��LL1��������
----------------------------------------------------------------------------------------------------------------*/
int Syntax::list_of_LL1(string A, string a) //L(A, a) = i ('i' is index of production)
{
	for (int i = 0; i < size_of_prod; i++)
	{
		if (prod[i].left == A && Select[i].find(a) != Select[i].end())
		{
			return i;
		}
	}
	return -1;
}
map<pair<string, string>, int>  Syntax::CreateLL1List()
{
	preProcess();
	dataRevision();
	DFS_First();
	DFS_Follow();
	getSelect();

	set<string> column_of_LL1_list = terminators;
	column_of_LL1_list.insert("��");
	for (set<string>::iterator it_nt = non_terminators.begin(); it_nt != non_terminators.end(); it_nt++)
		for (set<string>::iterator it_t = column_of_LL1_list.begin(); it_t != column_of_LL1_list.end(); it_t++)
		{
			tmp.insert(make_pair(make_pair(*it_nt, *it_t), list_of_LL1(*it_nt, *it_t)));
		}
	return tmp;
}

void Syntax::Enter()
{
	ifstream in(C_RULES, ios::in); //��ȡC�ķ��ļ�
	if (!in.is_open()) { //�ķ��ļ��򿪳��������Ǹ��ļ����û�����ɾ������
		cout << "�ķ������ļ���ȡʧ�ܣ��Ҳ������ļ���" << endl;
		return;
	}
	map<pair<string, string>, int> LL1;
	LL1 = CreateLL1List();
	string line_rules;
	set<string> column_of_LL1_list = terminators;
	column_of_LL1_list.insert("��");
/*	for (set<string>::iterator it_nt = non_terminators.begin(); it_nt != non_terminators.end(); it_nt++)
		for (set<string>::iterator it_t = column_of_LL1_list.begin(); it_t != column_of_LL1_list.end(); it_t++)
		{
			int t = LL1[make_pair(*it_nt, *it_t)];
			if (t > -1)
			{
				cout << *it_nt << "  " << *it_t << "  " << LL1[make_pair(*it_nt, *it_t)] << endl;
				// vector<string>::iterator  ji= prod[t].right;
				/*cout << prod[t].left << "->" ;

				for (int m = 0; m < prod[t].right.size();m++)
				{
					cout << prod[t].right[m];
					cout << " ";
				}
				cout << endl;
				cout << endl;
			}
		}*/
	neigui();
/*	cout << "���ս��" << endl;
	for (set<string>::iterator it_nt = non_terminators.begin(); it_nt != non_terminators.end(); it_nt++)
	{
		cout << *it_nt;
		cout << endl;
	}
	cout << "�ս��" << endl;
	for (set<string>::iterator it_t = column_of_LL1_list.begin(); it_t != column_of_LL1_list.end(); it_t++)
	{
		cout << *it_t;
		cout << endl;

	}
	int y = 0; int x = 0;
	for (set<string>::iterator it_nt = non_terminators.begin(); it_nt != non_terminators.end(); it_nt++, y++)
	{
		for (set<string>::iterator it_t = column_of_LL1_list.begin(); it_t != column_of_LL1_list.end(); it_t++, x++)
		{
			a[y][x] = LL1[make_pair(*it_nt, *it_t)] + 1;
		}
		x = 0;
	}
	for (int q = 0; q < 17; q++)
	{
		for (int p = 0; p < 15; p++)
		{
			cout << a[q][p] << "  ";
		}
		cout << endl;
	}*/

}
void Syntax::neigui()
{
	non_ter = this->non_terminators;
	for (int i = 0; i < 500; i++)
	{
		prod1[i].left = this->prod[i].left;
		prod1[i].right = this->prod[i].right;
		prod1[i].right1 = this->prod[i].right1;
	}
	ter = this->terminators;
}

void Syntax::file_enter()
{
	ifstream in(C_RULES, ios::in); //��ȡC�ķ��ļ�
	if (!in.is_open()) { //�ķ��ļ��򿪳��������Ǹ��ļ����û�����ɾ������
		cout << "�ķ������ļ���ȡʧ�ܣ��Ҳ������ļ���" << endl;
		return;
	}
	fstream datafile;
	datafile.open("test_1.txt", ios::out);
	if (!datafile)
	{
		cout << "�ķ������ļ���ȡʧ�ܣ��Ҳ������ļ���" << endl;
		exit(0);
	}
	map<pair<string, string>, int> LL1;
	LL1 = CreateLL1List();
	string line_rules;
	set<string> column_of_LL1_list = terminators;
	column_of_LL1_list.insert("��");
	for (set<string>::iterator it_nt = non_terminators.begin(); it_nt != non_terminators.end(); it_nt++)
		for (set<string>::iterator it_t = column_of_LL1_list.begin(); it_t != column_of_LL1_list.end(); it_t++)
		{
			int t = LL1[make_pair(*it_nt, *it_t)];
			if (t > -1)
			{
				cout << *it_nt << "  " << *it_t << "  " << LL1[make_pair(*it_nt, *it_t)] + 1 << endl;
				datafile << *it_nt << "  " << *it_t << "  " << LL1[make_pair(*it_nt, *it_t)] + 1 << endl;
			}
		}
	cout << "���ս��" << endl;
	datafile << "���ս��" << endl << endl;
	for (set<string>::iterator it_nt = non_terminators.begin(); it_nt != non_terminators.end(); it_nt++)
	{
		datafile << *it_nt << endl;
		cout << *it_nt;
		cout << endl;
	}
	cout << "�ս��" << endl;
	datafile << "�ս��" << endl << endl;
	for (set<string>::iterator it_t = column_of_LL1_list.begin(); it_t != column_of_LL1_list.end(); it_t++)
	{
		datafile << *it_t << endl;
		cout << *it_t;
		cout << endl;

	}


	int y = 0; int x = 0; int g;
	for (set<string>::iterator it_nt = non_terminators.begin(); it_nt != non_terminators.end(); it_nt++, y++)
	{
		for (set<string>::iterator it_t = column_of_LL1_list.begin(); it_t != column_of_LL1_list.end(); it_t++, x++)
		{
			a[y][x] = LL1[make_pair(*it_nt, *it_t)] + 1;
			g = x;
		}
		x = 0;
	}
	for (int q = 0; q < y; q++)
	{
		for (int p = 0; p <= g; p++)
		{
			cout << a[q][p] << "  ";
			datafile << a[q][p] << "  ";
		}
		cout << endl;
		datafile << endl;
	}



}




void find()                                             //���ҷ����� 
{

	Fund m0;

	/*	for(int t1=0; t1<non_ter.size(); t1++)
		{
			if(x.token==non_ter[t1])
			{
				k=t1;
				break;
			}
		}
		for(int t1=0; t1<ter.size(); t1++)
		{
			if(w.token==ter[t1])
			{
				l=t1;
				break;
			}
		}*/
	for (int t1 = ((prod1[value]).right).size() - 1; t1 >= 0; t1--)
	{
		set<string>::iterator it;
		set<string>::iterator it1;
		it = ter.find(prod1[value].right[t1]);
		it1 = non_ter.find(prod1[value].right[t1]);
		if (it != ter.end() || prod1[value].right[t1] == "id" || prod1[value].right[t1] == "int_const"
			|| prod1[value].right[t1] == "float_const" || prod1[value].right[t1] == "char_const" || prod1[value].right[t1] == "string")           //�жϲ���ʽ�Ҳ� 
		{
			m0.token = prod1[value].right[t1];
			m0.ty = 't';
			m0.num = 100;
			s1.push(m0);
		}
		else if (it1 != non_ter.end())
		{
			m0.token = prod1[value].right[t1];
			m0.ty = 'n';
			m0.num = 100;
			s1.push(m0);
		}
		else
		{
			m0.token = prod1[value].right[t1];
			m0.ty = 'g';
			m0.num = 100;
			s1.push(m0);
		}
	}

}

void start2() {
	Syntax a;
	a.file_enter();
	cout << "�ķ��������!" << endl;
}
void start()
{
	equal();
	string tm;
	Syntax tty;                                                 //�ķ��Զ�����LL1�� 
	tty.Enter();
	Fund m0 = { "#",'q',100 };
	s1.push(m0);

	m0 = { "translation_unit",'n',100 };
	s1.push(m0);
//===============�ʷ������� open �ȷ�ʽ�޸���
	lex1 a;
	a.openfile(in_path);
	while (flag2 == 1)
	{

		flag3 = 1;
		//	w=obj[t++];               //��ȡ��һ��
		
		pair<string, token_struct> w1 = a.char_select_word(in_path);
		//cout << "[" << w1.first << "]"<<endl;
		w.token = w1.first;
		if(w1.second.type== tokenType(1))
		{
			w.ty = '1';
		}
		else if(w1.second.type == tokenType(2))
		{
			w.ty = '2';
		}
		else if (w1.second.type == tokenType(3))
		{
			w.ty = '3';
		}
		else if (w1.second.type == tokenType(4))
		{
			w.ty = '4';
		}
		else if (w1.second.type == tokenType(5))
		{
			w.ty = '5';
		}
		else if (w1.second.type == tokenType(6))
		{
			w.ty = '6';
		}

		//cout << w1.second.type << endl;
		w.num = 100;
		while (flag3 == 1)
		{

			x = s1.top();      //��ջ������x����ջ������
			s1.pop();
			//cout << "ջ" << x.token << "   W" << w.token << w.ty<< endl;
			//if (!SEEM.empty()) cout << SEEM.top().first << endl;
			if (x.ty == 'g')            //���������Ϊ���嶯��
			{
				if(x.token!="epsilon")
				action_fun(x.token, tokenList.size() - 1, w.token);
				//if(!SEEM.empty())cout << "ע��"<<SEEM.top().first << endl;
			}
			else if (x.ty == 't') //������ս��
			{
				if (x.token == "id")
				{
					if ((w.ty == 2))
					{
						/*			j=w.token;*/
						flag2 = 1;
					}
					else flag2 = 2;
				}
				else if (x.token == "int_const" || x.token == "float_const")
				{
					if ((w.ty == 4))
					{
						/*		j=w.token;*/
						flag2 = 1;
					}
					else flag2 = 2;
				}
				else if (x.token == "char_const")
				{
					if ((w.ty == 5))
					{
						/*		j=w.token;*/
						flag2 = 1;
					}
					else flag2 = 2;
				}
				else if (x.token == "string")
				{
					if ((w.ty == 6))
					{
						/*		j=w.token;*/
						flag2 = 1;
					}
					else flag2 = 2;
				}
				else if (x.token == w.token)
				{
					flag2 = 1;
					j = w.token;
				}
				else
				{
					flag1 = 2;
					cout << "Line " << tokenList[SEEM.top().second].second.row << ": �﷨����.";
					exit(1);
					//return;
				}
				flag3 = 2;
				flag2 = 1;
			}
			else
			{
				if (x.ty == 'n')        //����Ƿ��ս��
				{

					if ((w.ty == '2'))
					{
						/*			j=w.token;*/
						tm = "id";
					}
					else if ((w.ty == '4'))
					{
						/*		j=w.token;*/
						tm = "int_const";
					}
					else if ((w.ty == '5'))
					{
						tm = "char_con";
					}
					else if ((w.ty == '6'))
					{
						tm = "string_con";
					}
					else
						tm = w.token;
				
					value = tty.list_of_LL1(x.token, tm);
	
					find();
					flag3 = 1;
					flag2 = 2;
				}
				else
				{
					if (w.token == "#")
					{
						flag1 = 1;
						flag2 = 2;
						flag3 = 2;
					}
					else
					{
						flag1 = 4;
						cout << "Line " << tokenList[SEEM.top().second].second.row << ": �﷨����.";
						exit(1);
					}
				}
			}
		}
	}

}




