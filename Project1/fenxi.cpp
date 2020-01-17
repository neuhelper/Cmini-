#include"head.h"

int token_ptr = 0; //token����
SymbolTa tmpsym; //��ʼ��ȫ�ַ��ű�
vector<vector<SymbolTa> > symb;       //�������ű�  ��
vector<SymbolTa> tmpvector;
Fund1 f1;
vector<Fund1> obj1;
stack<pair<string, int>> SEEM;            //����ջ
int off=0 ;
bool is_exist_nsy(string k);

int paramId = 1; //��ǵڼ�������
pair<string, int> funcName;
int qtID = 1;
int nowSynlayer = 0;						 //��ǰ���ڷ��ű�Ĳ��
int preSynlayer = -1;						 //ǰһ����ű�Ĳ��
string tmpstr;
bool isel = false;         //�ж��Ƿ�Ϊelse if 
pair<string, int> tmp1;       //��������ջ��һ��С�ݴ� 
enum Action_id {
	_push,
	_geq,
	_geq_unary,
	_relation,
	_laber,
	_goto,
	_assi,
	_if,
	_else,
	_while,
	_do,
	_for,
	_for_inc,
	_for_sec_pre,
	_for_sec_post,
	_for_third_pre,
	_for_third_post,
	_do_while,
	_do_while_cmp,
	_end_dowh,
	_logic,
	_scope_init,
	_param,
	_call,
	_end,
	_return,
	_cin,
	_cout,
	_init_decl,
	_init_arr,
	_get_func_name,
	_struct_scopeinit,
	_pop
};

map<string, int> Action_eq;             //���嶯����־ӳ��
void equal()
{
	tmpsym.prelayer = -1;
	tmpvector.push_back(tmpsym);
	symb.push_back(tmpvector);

	Action_eq["PUSH"] = _push;
	Action_eq["GEQ"] = _geq;
	Action_eq["GEQ_UNARY"] = _geq_unary;
	Action_eq["RELATION"] = _relation;
	Action_eq["LABER"] = _laber;
	Action_eq["GOTO"] = _goto;
	Action_eq["ASSI"] = _assi;
	Action_eq["IF"] = _if;
	Action_eq["ELSE"] = _else;
	Action_eq["WHILE"] = _while;
	Action_eq["DO"] = _do;
	Action_eq["FOR"] = _for;
	Action_eq["FORINC"] = _for_inc;
	Action_eq["FORSECPRE"] = _for_sec_pre;
	Action_eq["FORSECPST"] = _for_sec_post;
	Action_eq["FORTHDPRE"] = _for_third_pre;
	Action_eq["FORTHDPST"] = _for_third_post;
	Action_eq["DOWHILE"] = _do_while;
	Action_eq["DOWHCMP"] = _do_while_cmp;
	Action_eq["ENDDOWH"] = _end_dowh;
	Action_eq["LOGIC"] = _logic;
	Action_eq["SCOPEINIT"] = _scope_init;
	Action_eq["PARAM"] = _param;
	Action_eq["CALL"] = _call;
	Action_eq["END"] = _end;
	Action_eq["RETURN"] = _return;
	Action_eq["SCANF"] = _cin;
	Action_eq["PRINT"] = _cout;
	Action_eq["INITDECL"] = _init_decl;
	Action_eq["INITARR"] = _init_arr;
	Action_eq["GETFUNCNM"] = _get_func_name;
	Action_eq["STRUCTINIT"] = _struct_scopeinit;
	Action_eq["POP"] = _pop;

}

pair<int, int> getTypeCode(string type) //�������뼰������ռ�ڴ浥Ԫ�ĳ���
{
	if (type == "int") return make_pair(0, 2);
	else if (type == "float") return make_pair(1, 2);
	else if (type == "double") return make_pair(2, 4);
	else if (type == "long") return make_pair(3, 4);
	else if (type == "short") return make_pair(4, 1);
	else if (type == "char") return make_pair(5, 2);
	else if (type == "void") return make_pair(-1, 0); //��void�����������
	/*else {
	//��������
	Tapel[TapelIndex++].Tval =
	}*/
}

int getType_from_name(string type) //�������뼰������ռ�ڴ浥Ԫ�ĳ���
{
	int temp_nowlev = nowSynlayer;
	//for (int i = temp_nowlev; i > -1;) {
	while (symb[temp_nowlev][0].prelayer != -1) {
		//���ڵ�ǰ��,��
		for (int j = 1; j < symb[temp_nowlev].size(); j++) {
			if (type == symb[temp_nowlev][j].name) {
				int a = symb[temp_nowlev][j].ty;
				return a;
			}
		}
		//��һ��û�ҵ���ȥ���������,���Ը������temp_nowlevΪ��������ֵ
		temp_nowlev = symb[temp_nowlev][0].prelayer;
		//�����������ǰ��������-1,��ô���ͻ��ظ�����,��������ѭ��(û�ҵ�),���򱨴�
	}
	cout << "Line " << tokenList[SEEM.top().second].second.row << ": ";
	cout << "error L132: ��" << SEEM.top().first << "��: δ�����ı�ʶ��" << endl;
	exit(1);
	//}
		//������������
	return -2;
}
int change(int a)
{
	if (a == 0) return 2;
	else if (a == 1) return 2;
	else if (a == 2) return 4;
	else if (a == 3) return 4;
	else if (a == 4) return 1;
	else if (a == 5) return 2;
	else if (a == -1) return 0;
}


bool isExistInSymb(string id, int index) {                            //���ҷ��ű����Ƿ���  �������������������� 
	if (!symb.empty() && index <= symb.size()) {
		if (index >= symb.size()) index = symb.size() - 1;
		/*		vector<SymbolTa>::iterator it = symb[index].begin(), stor_it = it;        */
		int it = 0;
		for (it = 0; it != symb[index].size(); it++) {
			if (symb[index][it].name == id) {
				return true;
			}
		}
		if (it == symb[index].size() || symb[index][0].prelayer != -1) {
			//��ǰ������ķ��ű���δ�ҵ��ñ�ʶ�����Ҹ��������ȫ��������
			return isExistInSymb(id, symb[index][0].prelayer);
		}
		else if (it == symb[index].size() || symb[index][0].prelayer == -1) {
			return false;
		}
	}
	else return false;
}

int getOffset(string id, int index) {
	if (!symb.empty() && index <= symb.size()) {
		if (index >= symb.size()) index = symb.size() - 1;
		int it = 0;
		for (it = 0; it != symb[index].size(); it++) {
			if (symb[index][it].name == id) {
				return symb[index][it].offset;
			}
		}
		if (it == symb[index].size() || symb[index][0].prelayer != -1) {
			//��ǰ������ķ��ű���δ�ҵ��ñ�ʶ�����Ҹ��������ȫ��������
			return getOffset(id, symb[index][0].prelayer);
		}
		else if (it == symb[index].size() || symb[index][0].prelayer == -1) {
			return -1;
		}
	}
	else return -1;
}


void action_fun(string Act, int Tokenstation, string nowstr)
{
	vector<SymbolTa> tmptable; //��ʱ�����ű�
	SymbolTa tmpNode; //��ʱ�����ű���
	stack<pair<string, int>> tmpStack;
	switch (Action_eq[Act])
	{
	case Action_id::_push:
		SEEM.push(make_pair(nowstr, Tokenstation));
		break;
	case Action_id::_cin:
		if (tokenList[SEEM.top().second].second.type == _identifier) {
			f1 = { "cin","_","_",SEEM.top().first };
			obj1.push_back(f1);
		}
		break;
	case Action_id::_cout:
		if (!is_exist_nsy(SEEM.top().first))
		{
			if (!SEEM.empty() && (tokenList[SEEM.top().second].second.type == _identifier || tokenList[SEEM.top().second].second.type == _constnum)) {
				tmp1 = SEEM.top();
				if (tokenList[SEEM.top().second].second.type = _identifier) {
					//������Ǳ�����ֵ
					if (!isExistInSymb(SEEM.top().first, nowSynlayer)) {
						cout << "Line " << tokenList[SEEM.top().second].second.row << ": ";
						cout << "error C2065: ��" << SEEM.top().first << "��: δ�����ı�ʶ��" << endl;
						exit(1);
					}
				}

				SEEM.pop(); //������ݳ�ջ
				if (!SEEM.empty() && tokenList[SEEM.top().second].second.type == _constnum)
				{
					f1 = { "cout",SEEM.top().first,"_",tmp1.first };
					obj1.push_back(f1);
					SEEM.pop();
				}
				//else SEM.push(tmp);
				else
				{
					f1 = { "cout","10","_",tmp1.first };
					obj1.push_back(f1);
				}
			}
		}
		break;
	

	case Action_id::_geq:
		tmpstr = SEEM.top().first ; //cout << tmpstr << "is out!" << endl;//for test
		SEEM.pop();
		f1.p = SEEM.top().first;
		/*		quat << SEEM.top().first << " ";   //cout << SEM.top().first << "is out!" << endl;//for test*/
		SEEM.pop();
		f1.a1 = SEEM.top().first;
		f1.a2 = tmpstr;
		f1.ob = "t" + to_string(qtID);
		obj1.push_back(f1);
//		for (int i = 0;i < obj1.size();i++)
//			cout << obj1[i].p << " " << obj1[i].a1 << " " << obj1[i].a2 << " " << obj1[i].ob << endl;

//����ʱ������д�����ű���
		tmpNode.prelayer = preSynlayer;
		tmpNode.name = f1.ob;
		//tmpNode.ty =  getTypeCode(tokenList[SEEM.top().second].second.type).first;
		//һ���Ǵ������ڵ���������,����ٲ���,�������������,������Ҳ����ͱ���.������һ��������ȡ
		if (isnumm(f1.a1)) tmpNode.ty = 0;
		else tmpNode.ty = getType_from_name(f1.a1);//���� -1 Ӧ�ñ���
		tmpNode.if_temp = 't';
		tmpNode.offset = off;
		off += change(tmpNode.ty);
		//tmptable.push_back(tmpNode);
		symb[nowSynlayer].push_back(tmpNode);


		SEEM.pop();
		SEEM.push(make_pair("t" + to_string(qtID), -1));
		qtID++;
		break;

	case Action_id::_relation:
		if (SEEM.top().second == -1 || (tokenList[SEEM.top().second]).second.type == _identifier || (tokenList[SEEM.top().second]).second.type == _constnum) {
			if ((tokenList[SEEM.top().second]).second.type == _identifier) {
				//����Ƿ�δ����
				if (SEEM.top().second != -1 && !isExistInSymb(SEEM.top().first, nowSynlayer)) {
					cout << "Line " << (tokenList[SEEM.top().second]).second.row<< ": ";
					cout << "error C2065: ��" << SEEM.top().first << "��: δ�����ı�ʶ��" << endl;
					exit(1);
				}
			}
			tmp1 = SEEM.top();
			SEEM.pop();
			if (SEEM.top().first == "<" || SEEM.top().first == ">" || SEEM.top().first == "<=" || SEEM.top().first == ">="
				|| SEEM.top().first == "==" || SEEM.top().first == "!=") {
				f1 = {SEEM.top().first," "," "," " };
				SEEM.pop();
				if ((tokenList[SEEM.top().second]).second.type == _identifier) {
					//����Ƿ�δ����
					if (SEEM.top().second != -1 && !isExistInSymb(SEEM.top().first, nowSynlayer)) {
						//cout << "!isExistInSynbl(" << SEM.top().first << ", " << curSynblLev << ")" << endl;
						cout << "Line " << (tokenList[SEEM.top().second]).second.row << ": ";
						cout << "error C2065: ��" << SEEM.top().first << "��: δ�����ı�ʶ��" << endl;
						exit(1);
					}
				}
				f1.a1 = SEEM.top().first;
				f1.a2 = tmp1.first;
				f1.ob = "t" + to_string(qtID);
				obj1.push_back(f1);

				//����ʱ������д�����ű���
				tmpNode.prelayer = preSynlayer;
				tmpNode.name = f1.ob;
				//tmpNode.ty =  getTypeCode(tokenList[SEEM.top().second].second.type).first;
				//һ���Ǵ������ڵ���������,����ٲ���,�������������,������Ҳ����ͱ���.������һ��������ȡ
				if (isnumm(f1.a1)) tmpNode.ty = 0;
				else tmpNode.ty = getType_from_name(f1.a1);//���� -1 Ӧ�ñ���
				tmpNode.if_temp = 't';
				tmpNode.offset = off;
				off += change(tmpNode.ty);
				//tmptable.push_back(tmpNode);
				symb[nowSynlayer].push_back(tmpNode);
				SEEM.pop();
				SEEM.push(make_pair("t" + to_string(qtID), -1));
				qtID++;
			}
			else SEEM.push(tmp1);
		}
		break;
	case Action_id::_geq_unary:
		if (is_exist_nsy(SEEM.top().first)|| SEEM.top().first=="return_val") break;
		if (!SEEM.empty() && (SEEM.top().first == "++" || SEEM.top().first == "--")) {
			f1.p = SEEM.top().first[0];
			SEEM.pop();
			f1.a1 = SEEM.top().first;
			f1.a2 = "1";
			f1.ob = "t" + to_string(qtID);
			obj1.push_back(f1);

			//����ʱ������д�����ű���
			tmpNode.prelayer = preSynlayer;
			tmpNode.name = f1.ob;
			//tmpNode.ty =  getTypeCode(tokenList[SEEM.top().second].second.type).first;
			//һ���Ǵ������ڵ���������,����ٲ���,�������������,������Ҳ����ͱ���.������һ��������ȡ
			if (isnumm(f1.a1)) tmpNode.ty = 0;
			else tmpNode.ty = getType_from_name(f1.a1);//���� -1 Ӧ�ñ���
			tmpNode.if_temp = 't';
			tmpNode.offset = off;
			off += change(tmpNode.ty);
			//tmptable.push_back(tmpNode);
			symb[nowSynlayer].push_back(tmpNode);


			tmp1 = SEEM.top();
			SEEM.pop();
			f1.p = "=";
			f1.a1 = "t" + to_string(qtID);
			f1.a2 = "_";
			f1.ob = tmp1.first;
			obj1.push_back(f1);
			SEEM.push(tmp1);
			qtID++;
		}
		else if (!SEEM.empty() && (tokenList[SEEM.top().second].second.type == _identifier || SEEM.top().second == -1)) {
			tmp1 = SEEM.top();
			SEEM.pop();
			if (!SEEM.empty() && (SEEM.top().first == "++" || SEEM.top().first == "--")) {
				f1.p = SEEM.top().first[0];
				f1.a1 = tmp1.first;
				f1.a2 = "1";
				f1.ob = "t" + to_string(qtID);
				obj1.push_back(f1);


				//����ʱ������д�����ű���
				tmpNode.prelayer = preSynlayer;
				tmpNode.name = f1.ob;
				//tmpNode.ty =  getTypeCode(tokenList[SEEM.top().second].second.type).first;
				//һ���Ǵ������ڵ���������,����ٲ���,�������������,������Ҳ����ͱ���.������һ��������ȡ
				if (isnumm(f1.a1)) tmpNode.ty = 0;
				else tmpNode.ty = getType_from_name(f1.a1);//���� -1 Ӧ�ñ���
				tmpNode.if_temp = 't';
				tmpNode.offset = off;
				off += change(tmpNode.ty);
				//tmptable.push_back(tmpNode);
				symb[nowSynlayer].push_back(tmpNode);

				f1.p = "=";
				f1.a1 = "t" + to_string(qtID);
				f1.a2 = "_";
				f1.ob = tmp1.first;
				obj1.push_back(f1);
				SEEM.pop();
				SEEM.push(tmp1);
				qtID++;
			}
			else SEEM.push(tmp1);
		}
		else if (!SEEM.empty() && (SEEM.top().second == -1 || tokenList[SEEM.top().second].second.type == _identifier || tokenList[SEEM.top().second].second.type == _constnum)) {
			tmp1 = SEEM.top();
			SEEM.pop();
			if (!SEEM.empty() && SEEM.top().first == "~") {
				f1.p = "not";
				f1.a1 = tmp1.first;
				f1.a2 = "_";
				f1.ob = "t" + to_string(qtID);;
				obj1.push_back(f1);
				//����ʱ������д�����ű���
				tmpNode.prelayer = preSynlayer;
				tmpNode.name = f1.ob;
				//tmpNode.ty =  getTypeCode(tokenList[SEEM.top().second].second.type).first;
				//һ���Ǵ������ڵ���������,����ٲ���,�������������,������Ҳ����ͱ���.������һ��������ȡ
				if (isnumm(f1.a1)) tmpNode.ty = 0;
				else tmpNode.ty = getType_from_name(f1.a1);//���� -1 Ӧ�ñ���
				tmpNode.if_temp = 't';
				tmpNode.offset = off;
				off += change(tmpNode.ty);
				//tmptable.push_back(tmpNode);
				symb[nowSynlayer].push_back(tmpNode);

				SEEM.pop();
				SEEM.push(make_pair("t" + to_string(qtID), -1));
				qtID++;
			}
			else SEEM.push(tmp1);
		}
		break;
	case Action_id::_logic:
		if (!SEEM.empty() && (SEEM.top().second == -1 || (tokenList[SEEM.top().second]).second.type == _identifier || (tokenList[SEEM.top().second]).second.type == _constnum))
		{
			tmp1 = SEEM.top();
			SEEM.pop();
			if (!SEEM.empty() && (SEEM.top().first == "&&" || SEEM.top().first == "&")) {
				SEEM.pop(); //&& is out!
				f1 = { "and",SEEM.top().first,tmp1.first,"t" + to_string(qtID) };
				obj1.push_back(f1);
				SEEM.pop();
				SEEM.push(make_pair("t" + to_string(qtID), -1));
				qtID++;
			}
			else if (!SEEM.empty() && (SEEM.top().first == "||" || SEEM.top().first == "|")) {
				SEEM.pop(); //|| is out!
				f1 = { "or",SEEM.top().first,tmp1.first,"t" + to_string(qtID) };
				obj1.push_back(f1);
				SEEM.pop();
				SEEM.push(make_pair("t" + to_string(qtID), -1));
				qtID++;
			}
			else if (!SEEM.empty() && (SEEM.top().first == "^")) {
				SEEM.pop(); //^ is out!
				f1 = { "or",SEEM.top().first,tmp1.first,"t" + to_string(qtID) };
				obj1.push_back(f1);
				SEEM.pop();
				SEEM.push(make_pair("t" + to_string(qtID), -1));
				qtID++;
			}
			else if (!SEEM.empty() && (SEEM.top().first == "<<")) {
				SEEM.pop();
				f1 = { "shl",SEEM.top().first,tmp1.first,"t" + to_string(qtID) };
				obj1.push_back(f1);
				SEEM.pop();
				SEEM.push(make_pair("t" + to_string(qtID), -1));
				qtID++;
			}
			else if (!SEEM.empty() && (SEEM.top().first == ">>")) {
				SEEM.pop();
				f1 = { "shr",SEEM.top().first,tmp1.first,"t" + to_string(qtID) };
				obj1.push_back(f1);
				SEEM.pop();
				SEEM.push(make_pair("t" + to_string(qtID), -1));
				qtID++;
			}
			else SEEM.push(tmp1);
		}
		break;
	case Action_id::_if:
		if (!SEEM.empty() && SEEM.top().second == -1) {         //˵����tmp 
			if (isel == false) { //ֻ��if
				preSynlayer = nowSynlayer;
				//cout << "preSynblLev = " << preSynblLev << endl;//for test
				nowSynlayer = symb.size(); //���˺����������Ҫcur = pre;
				//cout << "curSynblLev = " << curSynblLev << endl;//for test
				//tmpNode.name = nullptr;
				tmpNode.prelayer = preSynlayer;
				tmpNode.domainName = "if";
				tmptable.push_back(tmpNode);
				symb.push_back(tmptable);
				//cout << "�����ܱ����½ڵ�" << synbl.size() - 1 << ": prelev = " << tmpNode.prelev << endl;//for test
				//cout << "------------IF------------" << endl;//for test
				f1.p = "if";
				f1.a1 = SEEM.top().first;
				f1.a2 = "_";
				f1.ob = to_string(nowSynlayer);
				obj1.push_back(f1);
				SEEM.pop();
			}
			else {
				//else if...
				f1.p = "if";
				f1.a1 = SEEM.top().first;
				f1.a2 = "_";
				f1.ob = to_string(nowSynlayer);
				obj1.push_back(f1);
				SEEM.pop();
			}
		}
		break;
	case Action_id::_else:
		preSynlayer = nowSynlayer;
		nowSynlayer = symb.size(); //���˺����������Ҫcur = pre;
		tmpNode.prelayer = preSynlayer;
		tmpNode.domainName = "else";
		tmptable.push_back(tmpNode);
		symb.push_back(tmptable);
		isel = true;
		f1.p = "el";
		f1.a1 = "_";
		f1.a2 = "_";
		f1.ob = to_string(nowSynlayer);
		obj1.push_back(f1);
		break;
	case Action_id::_while:
		preSynlayer = nowSynlayer;
		nowSynlayer = symb.size(); //���˺����������Ҫcur = pre;
		tmpNode.prelayer = preSynlayer;
		tmpNode.domainName = "while";
		tmptable.push_back(tmpNode);
		symb.push_back(tmptable);
		f1.p = "wh";
		f1.a1 = "_";
		f1.a2 = "_";
		f1.ob = to_string(nowSynlayer);
		obj1.push_back(f1);
		break;
	case Action_id::_do:
		if (SEEM.top().second == -1) {
			f1.p = "do";
			f1.a2 = "_";
			f1.ob = "_";
			f1.a1 = SEEM.top().first;
			obj1.push_back(f1);
			SEEM.pop();
		}
		break;
	case Action_id::_laber:
		f1 = {"label","_","_",SEEM.top().first};
		obj1.push_back(f1);
		SEEM.pop();
		break;
	case Action_id::_goto:
		f1 = { "goto","_","_",nowstr };
		obj1.push_back(f1);
		break;
	case Action_id::_scope_init:
		if ((tokenList[SEEM.top().second]).second.type == _identifier) {
			preSynlayer = nowSynlayer;   //��ǰ��α�Ϊ��һ��
			nowSynlayer = symb.size();  //��ǰ��Ϊ�����ܱ������¿��ٵ�һ��
			tmpNode.prelayer = preSynlayer;//����������ָ�����¼��һ���������
			tmpNode.domainName = SEEM.top().first;//��¼�������������
			tmptable.push_back(tmpNode);
			symb.push_back(tmptable);   //�ڷ����ܱ���Ϊ�������򿪱ٿռ�

			tmp1 = SEEM.top();
			SEEM.pop();
			if ((tokenList[SEEM.top().second]).second.type == _keyword) {
				f1.p = "fun_do";
				f1.a1 = tmp1.first;
				f1.a2 = "_";
				f1.ob = to_string(nowSynlayer);
				obj1.push_back(f1);
				//quat << "func "<<curSynblLev<<" # " << tmp1.first << endl; //generate function quat
				SEEM.pop(); //data type is out of stack!
				while (!SEEM.empty()) { //����һ���µ�������--��������SEMջ��֮ǰ������ȫ�����
					SEEM.pop();
				}
			}
			else SEEM.push(tmp1);
		}
		break;
	case Action_id::_end:
		//cout << "---------END---------" << endl;//for test
		/*if (synbl[curSynblLev][0].domainName == "max")
			cout << "stop";*/
		f1.p = "end";
		f1.a1 = "_";
		f1.a2 = "_";
		f1.ob = symb[nowSynlayer][0].domainName;
		obj1.push_back(f1);
		//cout<< "end # # " << synbl[curSynblLev][0].domainName << endl;//for test
		nowSynlayer = preSynlayer;					//�ָ���ǰ������Ϊ��һ��������
		preSynlayer = symb[preSynlayer][0].prelayer; //�ָ���һ������Ϊ���ϲ�������
		//cout << "preSynblLev = " << preSynblLev << endl;//for test
		//cout << "---------END---------" << endl;//for test
//		for (int i = 0;i < obj1.size();i++)
//						cout << obj1[i].p << " " << obj1[i].a1 << " " << obj1[i].a2 << " " << obj1[i].ob << endl;
		if (!SEEM.empty())
		{
			SEEM.pop() ;
		}
		break;
	case Action_id::_return:
		if (!SEEM.empty() && (SEEM.top().second == -1 || (tokenList[SEEM.top().second]).second.type == _identifier || (tokenList[SEEM.top().second]).second.type == _constnum)) {
			f1.p = "re";
			f1.a2 = "_";
			f1.ob = "_";
			f1.a1 = SEEM.top().first;
			obj1.push_back(f1);
		}
		else if (SEEM.empty()) {
			f1.p = "re";
			f1.a2 = "_";
			f1.ob = "_";
			f1.a1 = "void";
			obj1.push_back(f1);
		}
		break;
			case Action_id::_for:
				preSynlayer = nowSynlayer;
				nowSynlayer = symb.size();
				tmpNode.prelayer = preSynlayer;
				tmpNode.domainName = "for";
				tmptable.push_back(tmpNode);
				symb.push_back(tmptable);
				f1 = {"for","_","_",to_string(nowSynlayer) };
				obj1.push_back(f1);
				break;
			case Action_id::_for_inc:
				if (symb[nowSynlayer][0].domainName == "for") {
					f1 = { "jmp","_","_","inc" };
					obj1.push_back(f1);
				}
				break;
			case Action_id::_for_sec_pre:
				if (symb[nowSynlayer][0].domainName == "for") {
					f1 = { "label","_","_","cmp" };
					obj1.push_back(f1);
				}
				break;
			case Action_id::_for_sec_post:
				if (symb[nowSynlayer][0].domainName == "for") {
					if (SEEM.top().second == -1) {
						f1 = { "do",SEEM.top().first,"_","_" };
						obj1.push_back(f1);
						f1 = { "jmp","_","_" ,"end" };
						obj1.push_back(f1);
						f1 = { "jmp","_","_" ,"start" };
						obj1.push_back(f1);
					}
				}
				break;
			case Action_id::_for_third_pre:
				if (symb[nowSynlayer][0].domainName == "for") {
					f1 = { "label","_","_","inc" };
					obj1.push_back(f1);
				}
				break;
			case Action_id::_for_third_post:
				if (symb[nowSynlayer][0].domainName == "for") {
					f1 = { "jmp","_","_" ,"cmp" };
					obj1.push_back(f1);
					f1 = { "label","_","_","start" };
					obj1.push_back(f1);
				}
				break;
	case Action_id::_do_while:
		preSynlayer = nowSynlayer;
		nowSynlayer = symb.size();
		tmpNode.prelayer = preSynlayer;
		tmpNode.domainName = "dowhile";
		tmptable.push_back(tmpNode);
		symb.push_back(tmptable);
		f1 = { "dowhile","_","_",to_string(nowSynlayer) };
		obj1.push_back(f1);
		f1 = { "lable","_","_","start" };
		obj1.push_back(f1);
		break;
	case Action_id::_do_while_cmp:
		if (symb[nowSynlayer][0].domainName == "dowhile") {
			f1 = { "dowhcmp","_","_","_" };
			obj1.push_back(f1);
		}
		break;

	case Action_id::_end_dowh:
		if (SEEM.top().second == -1) {
			f1 = { "do",SEEM.top().first,"_","_" };
			obj1.push_back(f1);
			f1 = { "jmp","_","_","start" };
			obj1.push_back(f1);
			//quat << "end # # dowhile" << endl;
		}
		if (!SEEM.empty())
		{
			SEEM.pop() ;
		}
		break;
	case Action_id::_assi:
		if (!SEEM.empty()) {
			tmp1 = SEEM.top();
			SEEM.pop(); //number is out!
		/*	if (tmp1.first == "1")
				cout << "stop";*/
			if (tmp1.first == "return_val") {
				SEEM.pop(); //pop =
				if (!SEEM.empty() && (tokenList[SEEM.top().second]).second.type == _identifier) {
					f1 = { "mov",SEEM.top().first,"_","DX" };
					obj1.push_back(f1);
				}
			}
			else if (!SEEM.empty() && SEEM.top().first == "=") {
				//quat << SEM.top().first << " " << tmp1.first << " # ";
				SEEM.pop(); //= is out
				//cout << "curSynblLev = " << curSynblLev << endl;//for test
				//cout << "preSynblLev = " << preSynblLev << endl;//for test
				/*if (SEM.top().first == "b")
					cout << "stop";*/
				if (!SEEM.empty() && (tokenList[SEEM.top().second]).second.type == _constnum) {
					//���鸳ֵ
					pair<string, int> arroffset = SEEM.top();
					SEEM.pop(); //a[index]��index��ջ
					//if(stoi(arroffset.first)>=)//@@@@@@δ����û���������Խ������
					f1 = { "movs",to_string(getOffset(SEEM.top().first, nowSynlayer) + stoi(arroffset.first) * 2),"_",tmp1.first };
					obj1.push_back(f1);
					//Ŀǰ֧��int�����飬int����Ϊ2
				}

				else if (!SEEM.empty() && ((tokenList[SEEM.top().second]).second.type == _identifier || SEEM.top().second == -1)) {
					f1 = { "=",tmp1.first,"_","" };
					if (SEEM.top().second != -1 && !isExistInSymb(SEEM.top().first, nowSynlayer)) {
						//���������ֵ
						cout << "Line " << (tokenList[SEEM.top().second]).second.row << ": ";
						cout << "error C2065: ��" << SEEM.top().first << "��: δ�����ı�ʶ��" << endl;
						exit(1);
					}
					f1.ob = SEEM.top().first;
					obj1.push_back(f1);
				}
				//SEM.pop(); //delete it!
			}
			/*else if (!SEEM.empty() && regex_match(SEM.top().first, regex("\\+=|-=|\\*=|/=|%=|&=|\\|=|^=|>>=|<<="))) {
						if (regex_match(SEM.top().first, regex("\\+=|-=|\\*=|/=|%=")))
							quat << SEM.top().first[0] << " ";
						else if (SEM.top().first == "&=") quat << "and ";
						else if (SEM.top().first == "|=") quat << "or ";
						else if (SEM.top().first == "^=") quat << "not ";
						else if (SEM.top().first == ">>=") quat << "shr ";
						else if (SEM.top().first == "<<=") quat << "shl ";
						SEM.pop();
						if (SEEM.top().second != -1 && !isExistInSymb(SEEM.top().first, nowSynlayer)) {
							cout << "Line " << tokenSeq[SEM.top().second].row << ": ";
							cout << "error C2065: ��" << SEM.top().first << "��: δ�����ı�ʶ��" << endl;
							exit(1);
						}
						quat << SEM.top().first << " " << tmp1.first << " qtmp" + to_string(qtmpID) << endl;
						quat << "= qtmp" + to_string(qtmpID) << " # " << SEM.top().first << endl;
						//SEM.pop();//delete it!
						qtmpID++;
					}*/
			else if (!SEEM.empty() && !is_exist_nsy(SEEM.top().first)&& !is_exist_nsy(tmp1.first))
			{
				//cout << SEEM.top().first << endl;
			if (!SEEM.empty() && tokenList[SEEM.top().second].second.type == _identifier && tokenList[tmp1.second].second.type == _constnum) {
				//����Ԫ�ظ�ֵ��һ������,arr_index is out in tmp1, int b = a .1
				int memPos = getOffset(SEEM.top().first, nowSynlayer) + stoi(tmp1.first) * 2;
				pair<string, int> tmpa = SEEM.top();
				SEEM.pop(); //a is out
				if (!SEEM.empty() && SEEM.top().first == "=") {
					SEEM.pop();//= is out
					f1 = { "movs",SEEM.top().first ,"_",to_string(memPos) };
					obj1.push_back(f1);
				}
				else {
					SEEM.push(tmpa);
					SEEM.push(tmp1);
				}
			}
			else SEEM.push(tmp1);
		}
			else SEEM.push(tmp1);
		}
		break;

	case Action_id::_init_decl:
	{
		if (!SEEM.empty() && (tokenList[SEEM.top().second]).second.type != _keyword) 
		{
			int i;
			for (i = 1; !symb.empty() && nowSynlayer < symb.size() && i < symb[nowSynlayer].size(); i++) {
				//���ñ����Ƿ��Ѿ������
				if (symb[nowSynlayer][i].name == SEEM.top().first) {
					//�ظ�����
					cout << "Line " << (tokenList[SEEM.top().second]).second.row << ": ";
					cout << "error C2086: ��int " << SEEM.top().first << "��: �ض���." << endl;
					exit(1); //�ض���ֱ��ֹͣ����
				}
			}
			tmp1 = SEEM.top();
		//	cout << "����" << tmp1.first << endl;
			tmpNode.prelayer = preSynlayer;
			tmpNode.name = tmp1.first;
			tmpNode.if_temp = "nt";
			stack<pair<string, int>> tmpSEM;
			while (!SEEM.empty() && (tokenList[SEEM.top().second]).second.type != _keyword) {
				tmpSEM.push(SEEM.top());
				SEEM.pop();
			}
			tmpNode.ty = getTypeCode(SEEM.top().first).first; //��д���ű��TYP������
			if (tmpNode.ty == -1) {
				//void���ͱ����ڶ��������������
				cout << "Line " << (tokenList[tmp1.second]).second.row << ": ";
				cout << "error C2182: ��" << tmpNode.name << "��: �Ƿ�ʹ�á�void������." << endl;
				exit(1);	
			} 
			tmpNode.offset = off; //��¼֮ǰ��ƫ����
			off += getTypeCode(SEEM.top().first).second; //���µ�ǰƫ����
			//cout << "$$$$$$$$$$$$$$$$$$$$$$$$$ off = " << off << endl; //for test
			//SEM.pop(); //int��ջ���������ͳ�ջ????????????????????????
			if (!symb.empty() && nowSynlayer < symb.size()) {
				symb[nowSynlayer].push_back(tmpNode); //�����ű��Ѿ����ڵ����
			//	cout << endl << tmpNode.name << "����synbl[" << curSynblLev << "]" << endl; //for test
			}
			else {
				//��ǰ������״���д���������ű����ڵ����
				tmptable.push_back(tmpNode); //��ǰ�����뵥���ű���
				symb.push_back(tmptable);
				//	cout << "�����ܱ��½ڵ�" << synbl.size() - 1 << ": " << tmpNode.name << "����";
				//	cout << ", prevel = " << tmpNode.prelev << endl;//for test
			}

			while (!tmpSEM.empty()) { //�ݴ�����ԭ����ջ��
				SEEM.push(tmpSEM.top());
				tmpSEM.pop();
			}
		}
	}
//	cout<<"�����ű��Ƿ�����"<<(symb[1]).size();
/*	for(int i=0;i<(symb[1]).size()-1;i++)
	{
	cout << symb[1][0].domainName <<symb[1][i+1].name << " " << symb[1][i+1].prelayer << " " << symb[1][i+1].ty << " "<<symb[1][i+1].offset << endl;
	}*/
	break;
	case Action_id::_init_arr:
		if (!SEEM.empty() && (tokenList[SEEM.top().second]).second.type == _constnum) {
			//	cout << "$$$$$$$$$$$$$$$$$$$$$$$$$ off = " << off << endl; //for test
			tmp1 = SEEM.top();
			SEEM.pop();
			if (!SEEM.empty() && (tokenList[SEEM.top().second]).second.type == _identifier) {
				//vector<SymbolTable> tmpSynbl; //��ʱ�����ű�
				//SymbolTable tmpNode; //��ʱ�����ű���
				tmpNode.name = SEEM.top().first;
				tmpNode.if_temp = "nt" ;
				pair<string, int> idtmp = SEEM.top();
				SEEM.pop();//id is out!
				tmpNode.prelayer = preSynlayer;
				tmpNode.offset = off;
				if (!SEEM.empty() && (tokenList[SEEM.top().second]).second.type == _keyword) {
					tmpNode.ty = getTypeCode(SEEM.top().first).first;
					off += getTypeCode(SEEM.top().first).second * (stoi(tmp1.first));
					symb[nowSynlayer].push_back(tmpNode);
					//SEM.pop();//int is out
					//SEM.push(idtmp);
				}
			}
		}
		break;
	case Action_id::_get_func_name:
		if (!SEEM.empty() && tokenList[SEEM.top().second].second.type == _identifier) {
			paramId = 1;
			funcName = SEEM.top();
		}
		break;
	case Action_id::_param:
		//����ʵ����Ԫʽ
		if (SEEM.top().second == -1 || tokenList[SEEM.top().second].second.type == _identifier || tokenList[SEEM.top().second].second.type == _constnum) {
			//quat << "param # # " << SEM.top().first << endl;
			int i;
			for (i = 0; i < symb.size(); i++) {
				if (symb[i][0].domainName == funcName.first) {
					f1 = { "mov", to_string(symb[i][paramId].offset) ,"_",SEEM.top().first };
					obj1.push_back(f1);
					break;
				}
			}
			if (i < symb.size()) //�ҵ���
				SEEM.pop();
			else {
				//û�ҵ��ú���
				cout << "Line " << tokenList[funcName.second].second.row << ": ";
				cout << "'" << funcName.first << "' was not declared in this scope" << endl;
				exit(1);
			}
			paramId++;
		}
		break;
	case Action_id::_call:
		cout << "���ú���" << endl;
		if (tokenList[SEEM.top().second].second.type == _identifier) {
			f1 = { "call", "_" ,"_",SEEM.top().first };
			obj1.push_back(f1);
			SEEM.pop();
			SEEM.push(make_pair("return_val", -3));
		}
		else {
			cout << "Line " << tokenList[funcName.second].second.row << ": ";
			cout << "'" << funcName.first << "' was not declared as an identifier" << endl;
			exit(1);
		}
		break;
/*	case Action_id::_pop:
		while (!SEEM.empty()) {
			if (tokenList[SEEM.top().second].second.type == _constnum)
			{	//��ֵ��� stack<pair<string, int>> SEM;
				string constnum = SEEM.top().first; //��¼�¸ó���
				SEEM.pop(); //ֵ��ջ
				token_ptr = SEEM.top().second; //id��Token����
				int i;
				for (i = 0; !symb.empty() && nowSynlayer < symb.size() && i < symb[nowSynlayer].size(); i++) {
					//���ñ����Ƿ��Ѿ������
					if (symb[nowSynlayer][i].name == identifier_table[tokenList[SEEM.top().second].second.id].name) {
						//�ظ�����
						cout << "Line " << tokenList[SEEM.top().second].second.row << ": ";
						cout << "error C2086: ��int " << identifier_table[tokenList[SEEM.top().second].second.id].name << "��: �ض���." << endl;
						//SEM.pop();
						exit(1); //�ض���ֱ��ֹͣ����
						//break;
					}
				}
				//if (!synbl.empty() && i < synbl[curSynblLev].size()) continue; //�����ض���
				tmpNode.name = SEEM.top().first;
				tmpNode.prelayer = preSynlayer;
				//cout << "tmpNode.prelev = " << tmpNode.prelev << endl;//for test
				SEM.pop(); //id��ջ
				stack<pair<string, int>> tmpSEM;
				while (!SEEM.empty() && tokenList[SEEM.top().second].second.type != _keyword) {
					tmpSEM.push(SEEM.top());
					SEEM.pop();
				} //���ҹ��˵�ͬһ�д���ǰ��Ķ��������
				string tmptype = "";
				while (!SEM.empty() && tokenList[SEEM.top().second].second.type == _keyword) {
					//����ű��Type���������������ӵ����ͱ�
					tmptype += (SEEM.top().first + " ");
					tmpSEM.push(SEEM.top());
					SEM.pop();
				}
				tmptype = tmptype.substr(0, tmptype.size() - 1); //ȥ�������Ŀո�
				tmpNode.ty = getTypeCode(tmptype).first; //��д���ű��TYP������ 
				//identifier_table[synbl_ptr].typ = getTypeCode(tmptype);   
				if (tmpNode.ty == -1) {
					//void���ͱ����ڶ��������������
					cout << "Line " << tokenList[token_ptr].second.row << ": " << endl;
					cout << "error C2182: ��" << SEEM.top().first << "��: �Ƿ�ʹ�á�void������" << endl;
					exit(1);
				}
				tmpNode.offset = off;
				off += getTypeCode(tmptype).second;
				//cout << "$$$$$$$$$$$$$$$$$$$$$$$$$ off = " << off << endl; //for test
				if (!symb.empty() && nowSynlayer < symb.size()) {
					symb[nowSynlayer].push_back(tmpNode); //�����ű��Ѿ����ڵ����
				}
				//if (curSynblLev >= synbl.size()) {
				else {
					//��ǰ������״���д���������ű����ڵ����
					tmptable.push_back(tmpNode); //��ǰ�����뵥���ű���
					symb.push_back(tmptable);
					//cout << "�����½ڵ�" << synbl.size() - 1 << ": prelev = " << tmpNode.prelev << endl;//for test
				}
				f1 = { "=",constnum,"#",tmpNode.name };
				obj1.push_back(f1);
				while (!tmpSEM.empty()) { //�ݴ�����ԭ����ջ��
					SEEM.push(tmpSEM.top());
					tmpSEM.pop();
				}
			}
			else if (tokenList[SEEM.top().second].second.type == _identifier) {
				//��������� int b;
				int i;
				for (i = 0; !symb.empty() && nowSynlayer < symb.size() && i < symb[nowSynlayer].size(); i++) {
					//���ñ����Ƿ��Ѿ������
					if (symb[nowSynlayer][i].name == SEEM.top().first) {
						//�ظ�����
						cout << "Line " << tokenList[SEEM.top().second].second.row << ": ";
						cout << "error C2086: ��int " << SEEM.top().first << "��: �ض���." << endl;
						//SEM.pop();
						//break;
						exit(1); //�ض���ֱ��ֹͣ����
					}
				}
				//if (!synbl.empty() && i < synbl[curSynblLev].size()) continue; //�����ض���
				tmpNode.name = SEEM.top().first;
				tmpNode.prelayer = preSynlayer;
				//cout << "tmpNode.prelev = " << tmpNode.prelev << endl; //for test
				//synbl_ptr = tokenSeq[SEM.top().second].id;
				token_ptr = SEEM.top().second;
				SEM.pop(); //id��ջ
				stack<pair<string, int>> tmpSEM;
				while (!SEM.empty() && tokenList[SEEM.top().second].second.type != _keyword) {
					tmpSEM.push(SEEM.top());
					SEM.pop();
				}
				string tmptype = "";
				while (!SEM.empty() && tokenList[SEEM.top().second].second.type == _keyword) {
					//����ű��Type���������������ӵ����ͱ�
					tmptype += (SEEM.top().first + " ");
					tmpSEM.push(SEEM.top());
					SEEM.pop();
				}
				tmptype = tmptype.substr(0, tmptype.size() - 1); //ȥ�������Ŀո�
				tmpNode.ty = getTypeCode(tmptype).first; //��д���ű��TYP������
														  //identifier_table[synbl_ptr].typ = getTypeCode(tmptype);   //��д���ű��TYP������
				if (tmpNode.ty == -1) {
					//void���ͱ����ڶ��������������
					cout << "Line " << tokenList[token_ptr].second.row << ": " << endl;
					cout << "error C2182: ��" << tmpNode.name << "��: �Ƿ�ʹ�á�void������." << endl;
					exit(1);
				}

				//for test
				//cout << tmpNode.typ << "������ű�TYP��, " << endl;
				//for test
				tmpNode.offset = off; //��¼֮ǰ��ƫ����
				//cout << "��ǰƫ����Ϊ��" << tmpNode.offset << endl; //for test
				off += getTypeCode(tmptype).second; //���µ�ǰƫ����
				//cout << "$$$$$$$$$$$$$$$$$$$$$$$$$ off = " << off << endl; //for test
				if (!symb.empty() && nowSynlayer < symb.size()) {
					symb[nowSynlayer].push_back(tmpNode); //�����ű��Ѿ����ڵ����
				//	cout << endl << tmpNode.name << "����synbl[" << curSynblLev << "]" << endl; //for test
				}
				//if (curSynblLev >= synbl.size()) {
				else {
					//��ǰ������״���д���������ű����ڵ����
					tmptable.push_back(tmpNode); //��ǰ�����뵥���ű���
					symb.push_back(tmptable);
					//	cout << "�����ܱ��½ڵ�" << synbl.size() - 1 << ": " << tmpNode.name << "����";
					//	cout << ", prevel = " << tmpNode.prelev << endl;//for test
				}

				while (!tmpSEM.empty()) { //�ݴ�����ԭ����ջ��
					SEEM.push(tmpSEM.top());
					tmpSEM.pop();
				}
			}
			else SEM.pop();
		}
		break;*/
	}
}
