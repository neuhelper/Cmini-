#include"head.h"
#include<fstream> 
class lex1 {
public:
	char ch;
	string path;//�ļ���ַ 
	string text;//ʵ���ı���Ϣ(������ʵ�ı�)


	//token_word������һ��token�ڵ�ĺ���,����һ��pair��, 
/*		pair<string,token_struct>  token_word(){

	token_struct token_tmp;
	token_tmp.type = tokenType(1);
	token_tmp.id = 1;
	token_tmp.row = 2;
	pair<string,token_struct> token_singleword("word",token_tmp);

	//return token_singleword;
	}*/
	//���ı�ÿһ�е���Ϣ����
	string getit(string a) {
		string target1 = "//";
		string target2 = "/*";
		string target3 = "*/";
		//int n = target.size();
		int pos1 = a.find(target1);
		int pos2 = a.find(target2);
		int pos3 = a.find(target3);
		//	cout << get_tag<<"  "<<pos3 <<endl;
		//get_tag���Ϊ�Ƿ�ʶ���� /* ���,ֻ�� */ ��Գɹ���Ϊ0����Ϊ1
		if (get_tag == 0) {
			if (pos1 != a.npos) {//�ҵ�˫б��
				//cout<<pos<<endl; 
			   //	cout<<"�ҵ�"<<endl;
				a = a.substr(0, pos1);
				get_tag = 0;
			}
			if (pos2 != a.npos) {
				if (pos3 != a.npos) {
					get_tag = 0;
					//  cout<<"��ǳɹ�f"<<get_tag<<endl;
				}
				else {
					get_tag = 1;
					//cout<<"��ǳɹ�"<<get_tag<<endl;
				}
				a = a.substr(0, pos2);
			}
		}
		else {
			//	cout<<"��ǳɹ�"<<get_tag<<endl;
			if (pos3 == a.npos) {
				a = "";
				get_tag = 1;
			}
			else {
				if (pos3 < a.length() - 1) {// /*  */��Գɹ�,����Ч�ı���
					a = a.substr(pos3 + 2, a.length() - 1);
				}
				else {
					a = "";
				}

				get_tag = 0;
			}
		}


		return a;
	}
	string find_include(string pre_string) {
		//ֻ��һ��
		int pos = pre_string.find('\n');
		if (pos != pre_string.npos) {
			//cout << pre_string << endl;
			string temp1 = pre_string;
			temp1 = temp1.substr(0,pos);
			string temp2 = pre_string.substr(pos, pre_string.length() - 1);
			pre_string = get_include(temp1, temp2);
		}
		//cout << "����#,��������ͷ�ļ�����" << endl;
		return pre_string;
	}
	string get_include(string _defind, string _trueString) {
		string target = "\"";
		int pos1 = _defind.find(target);
		if (pos1 != _defind.npos) {//�ҵ�����
			_defind= _defind.substr(pos1 + 1, _defind.length() - 1);
			int pos2 = _defind.find(target);
			if (pos2 != _defind.npos) {//�ҵ�����
				_defind = _defind.substr(0, pos2);
				fstream include_head;
				include_head.open("head.txt");
				if (!include_head) {
					cout << "open head.txt failed! ͷ�ļ��򿪴���!" << endl;
					exit(1);
				}
				string s="";
				_defind = "";
				while (!include_head.eof()) {
					getline(include_head, s);
					_defind += s + '\n';
					//	cout<<word<<endl;

				}
				include_head.close();
				_trueString = _defind + _trueString;
				//cout << _trueString << endl;
				//cout << "ͷ�ļ���ӳɹ�" << endl;
				return _trueString;
			}
			else {
				cout << "Include a head file is fail!!!" << endl;
				exit(1);
			}
		}
		else {
			cout << "Include a head file is fail!!!"<<endl;
			exit(1);
		}
		
		//return _trueString;
	}
	string openfile(string path) {
		string word;
		fstream in;
		char* _path = const_cast<char*>(path.c_str());
		in.open(_path);
		if (!in) {
			cout << "open file failed! Դ�ļ��򿪴���!" << endl;
			exit(1);
		}
		string s;
		while (!in.eof()) {
			string c;
			getline(in, c);
			s = this->getit(c);
			word += s + ' ' + '\n';
				//cout<<word<<endl;

		}
		//word += '#';
	//	cout<<word<<endl;
		in.close();
		//cout << "��һ��Ӧ�ò���ͷ�ļ�" << endl;
		if (word[0] == '#') {
			//cout << "Ӧ�ò���ͷ�ļ�" << endl;
			word=find_include(word);
		}
	//	cout << "[" << word << "]" << endl;
		text = word;//�����޸���
		return word;
	}
	pair<string, token_struct> char_select_word(string path) {
		pair<string, token_struct> token_singleword;
		string word;
		//text�ĳ����public
		
		//text = this->openfile(path);
		//	cout<<text<<endl;
		int pre_charnum;
		while (_charnum < text.length())
		{
			int state;
			string word = "";
			t = text[_charnum];
			//	cout<<"��ǰʶ���"<<line_num<<"�е��ַ�: "<<text[_charnum]<<(int)text[_charnum]<<endl;
			if ((int)t == 32)
				state = 0;// ��ǰ�ַ�Ϊ�ո�ʱ,״̬���Ϊ0
			else if (((int)t > 64 && (int)t < 91) || ((int)t > 96 && (int)t < 123))// 65~90 A-Z,97~122 a~z
				state = 1;// �����ǰ�ַ�Ϊ�ַ�ʱ,״̬���Ϊ1
			else if ((int)t >= 48 && (int)t <= 57)
				state = 2;// �����ǰ�ַ�Ϊ����ʱ,״̬���Ϊ2
			else if ((int)t == 34)
				state = 3;// �����ǰ�ַ�Ϊ˫����ʱ,״̬���Ϊ3
			else if ((int)t == 39)
				state = 4;// �����ǰ�ַ�Ϊ������ʱ,״̬���Ϊ4
			else if (((int)t >= 33 && (int)t <= 47 && (int)t != 34 && (int)t != 39)
				|| ((int)t >= 58 && (int)t <= 64) || ((int)t >= 91 && (int)t <= 96)
				|| ((int)t >= 123 && (int)t <= 126))
				state = 5;// �����ַ�,״̬���Ϊ5
			else if (t == '\t' || t == '\v' || t == '\f') {
				state = 0;
			}
			else if ((int)t == 10) {
				line_num++;
				state = 0;
			}
			else {
				// ��⵽�Ƿ��ַ�ʱ����쳣��Ϣ
				cout << "����쳣" << endl;
				cout << t << endl;
				break;
			}


			//state_f = state;
			//state = state_change(state,ch);
			switch (state) {
			case 0:
				_charnum++;
				break;
			case 1:
				pre_charnum = _charnum;
				if (_charnum < text.length()) {
					_charnum++;
					t = text[_charnum];
					while (((t > 64 && t < 91) || (t > 96 && t < 123) || (t > 47 && t < 58))
						&& (_charnum < (text.length() - 1))) {// 65~90 A-Z,97~122 a~z,48~57 0-9
					// �����ǰ�ַ�Ϊ��ĸ��������ʱ,��ǰ�ַ��±�+1,����һֱ�������һ������
						_charnum++;// �����±�
						t = text[_charnum];// ���µ�ǰ�ַ�ֵ
					}
				}
				for (int j = pre_charnum; j < _charnum; j++) {
					word = word + text[j];
				}
				//	cout<<"case1:���ɵ���<"<<word<<">"<<endl;
				if (IsKeywords(word) >= 0) {
					//numkt++; 
					token_struct token_tmp;
					token_tmp.type = tokenType(1);
					token_tmp.id = IsKeywords(word);
					token_tmp.row = line_num;
					token_singleword.first = word;
					token_singleword.second = token_tmp;
					tokenList.push_back(token_singleword);

					//cout<<"�ǹؼ���"<<token_singleword.second.row<<endl;
				}
				else {
					if (IsKeywords(word) >= 0) {// ���i��ʶ����Ϊ��,�ұ������������
						token_struct token_tmp;
						token_tmp.type = tokenType(2);
						token_tmp.id = IsKeywords(word);
						token_tmp.row = line_num;
						token_singleword.first = word;
						token_singleword.second = token_tmp;
						tokenList.push_back(token_singleword);
						//return token_singleword;
					//	cout<<"���ִ��ʶ��"<<token_singleword.first<<endl;
					}
					else {

						token_struct token_tmp;
						token_tmp.type = tokenType(2);
						token_tmp.id = numit;
						token_tmp.row = line_num;
						token_singleword.first = word;
						token_singleword.second = token_tmp;
						tokenList.push_back(token_singleword);
						numit++;
						//return token_singleword;
				//		cout<<"�����ִ��ʶ��"<<token_singleword.first<<endl;
					}

				}
				//	_charnum++;
				return token_singleword;
				break;
			case 2:
				pre_charnum = _charnum;
				if (_charnum <= text.length()) {// �����ǰ�ַ����ı���Ϣ��
					while ((int)t >= 48 && (int)t <= 57 && _charnum < text.length() - 1) { // 48~57 0-9
						if ((int)text[_charnum + 1] == 46) {// ���������С����
							_charnum += 2;
							t = text[_charnum];
						}
						else {// ������ֺ���������
							while ((int)t >= 48 && (int)t <= 57 && _charnum <= text.length()) {
								_charnum++;
								if (_charnum < text.length()) {
									t = text[_charnum];
								}
								else {
									t = 'c';//ǿ���˳�ѭ��
								}

								if ((int)t == 46) {
									_charnum++;
									t = text[_charnum];
								}
							}
						}
					}
				}

				// ��ȡ�øĳ��ȵ��ַ�,�õ�һ����
				for (int j = pre_charnum; j < _charnum; j++) {
					word = word + text[j];
				}
				// cout<<"case:2����word"<<endl;
				// System.out.println(word);
				 // ����c������,�������ֱ�Ӳ���λ��
				if (IsConstnum(word) >= 0) {// �����Ϊ��,�ұ������������
					token_struct token_tmp;
					token_tmp.type = tokenType(4);
					token_tmp.id = IsConstnum(word);
					token_tmp.row = line_num;
					token_singleword.first = word;
					token_singleword.second = token_tmp;
					tokenList.push_back(token_singleword);
					//	return token_singleword;
					//	cout<<"�ǳ������е�ֵ"<<token_singleword.first<<endl;
				}
				else {

					token_struct token_tmp;
					token_tmp.type = tokenType(4);
					token_tmp.id = numct;
					token_tmp.row = line_num;
					token_singleword.first = word;
					token_singleword.second = token_tmp;
					tokenList.push_back(token_singleword);
					numct++;

					//	cout<<"�ǳ������Ǳ���û��"<<token_singleword.first<<endl;
				}
				return token_singleword;
				break;
			case 3:
				pre_charnum = _charnum;
				_charnum++;
				// ��˫���ź�һλ��ʼʶ��,��˫�����ַ�һ�ɻ�Ϊ�ַ�������
				while ((int)text[_charnum] != 34 && _charnum < text.length() - 1)
					_charnum++;
				//cout<<text[_charnum+1]<<endl;
				// ���ƥ�䲻���ɶԵ�˫����,�򱨴�
				if (text[_charnum + 1] == ';')
				{
					if (text[_charnum] == '\"') {
						//cout<<"ƥ��ɹ�"<<text[_charnum]<<endl;
					}
					else {
						cout << text[_charnum + 1] << "˫���Ų��ɶԴ���" << text[_charnum] << endl;
						exit(1);
					}
				}
				// �ַ�����Ҫ��˫����һ���������
				for (int j = pre_charnum; j < _charnum + 1; j++) {
					word = word + text[j];
				}
				_charnum++;

				// ����S��,���S���к����������
				if (IsString(word) >= 0) {// ���S��Ϊ��,�ұ������������
					token_struct token_tmp;
					token_tmp.type = tokenType(6);
					token_tmp.id = IsString(word);
					token_tmp.row = line_num;
					token_singleword.first = word;
					token_singleword.second = token_tmp;
					tokenList.push_back(token_singleword);
					//	cout<<"�������е��ַ���token"<<token_singleword.first<<endl;
				}
				else {

					token_struct token_tmp;
					token_tmp.type = tokenType(6);
					token_tmp.id = numst;
					token_tmp.row = line_num;
					token_singleword.first = word;
					token_singleword.second = token_tmp;
					tokenList.push_back(token_singleword);
					numst++;
					//		cout<<"�����������ַ���token"<<token_singleword.first<<endl;
				}
				return token_singleword;
				break;
			case 4:// �ַ�
				pre_charnum = _charnum;
				_charnum++;
				// �ӵ����ź��濪ʼʶ��,����ǵ��ַ�
				if ((int)text[_charnum] != 39 && _charnum < text.length() - 1)
					_charnum++;
				// ������ƥ�䲻�ɹ�,�򱨴�
				if (text[_charnum] != '\'') {
					cout << text[_charnum] << "�����Ų��ɶԴ���" << '\'' << endl;
					exit(1);
				}
				// ���浥���ź͵��ַ���Ϊһ������
				for (int j = pre_charnum; j < _charnum + 1; j++) {
					word = word + text[j];
				}
				// ������ƥ�䲻�ɹ�,�򱨴�
			   // if ((int)text[word.length()-1 ]!= 39)
				 //   cout<<"�����Ų��ɶԴ���"<<endl;
				// ���C���ַ����к����������
				if (IsChar(word) >= 0) {// ���i��ʶ����Ϊ��,�ұ������������
					token_struct token_tmp;
					token_tmp.type = tokenType(5);
					token_tmp.id = IsChar(word);
					token_tmp.row = line_num;
					token_singleword.first = word;
					token_singleword.second = token_tmp;
					tokenList.push_back(token_singleword);
				}
				else {

					token_struct token_tmp;
					token_tmp.type = tokenType(5);
					token_tmp.id = numzt;
					token_tmp.row = line_num;
					token_singleword.first = word;
					token_singleword.second = token_tmp;
					tokenList.push_back(token_singleword);
					numzt++;
				}
				_charnum++;
				return token_singleword;
				break;
			case 5:// ����
				   // ����Ƿ���,��ֱ�ӽ�����ȡ��

				word = text[_charnum];

				///	cout<<_charnum<<">"<<word[0]<<"<"<<"���ҷ���ֵ"<<IsDelimiter(word)<<endl; 
				if (_charnum + 1 < text.length()) {
					t = text[_charnum + 1];
					if (((int)t >= 33 && (int)t <= 47 && (int)t != 34 && (int)t != 39 && (int)t != 44) || ((int)t >= 58 && (int)t <= 64) || ((int)t >= 91 && (int)t <= 96)
						|| ((int)t == 124)) {
						char f_point1 = text[_charnum];
						char f_point2 = text[_charnum + 1];
						// cout<< f_point1 <<"  "<<f_point2 <<endl;
						if ((f_point1 == '&' && f_point2 == '&') || (f_point1 == '+' && f_point2 == '+')
							|| (f_point1 == '-' && f_point2 == '-') || (f_point1 == '|' && f_point2 == '|')
							|| (f_point1 == '>' && f_point2 == '=') || (f_point1 == '<' && f_point2 == '=')
							|| (f_point1 == '=' && f_point2 == '=') || (f_point1 == '!' && f_point2 == '=')
							|| (f_point1 == '<' && f_point2 == '<') || (f_point1 == '>' && f_point2 == '>')
							|| (f_point1 == '+' && f_point2 == '=') || (f_point1 == '-' && f_point2 == '=')
							|| (f_point1 == '+' && f_point2 == '+') || (f_point1 == '*' && f_point2 == '=')
							|| (f_point1 == '/' && f_point2 == '=') || (f_point1 == '%' && f_point2 == '=')
							|| (f_point1 == '&' && f_point2 == '=') || (f_point1 == '|' && f_point2 == '=')
							|| (f_point1 == '^' && f_point2 == '=')) {
							word += f_point2;
							//  cout<<word<<endl;
							_charnum++;
						}
					}
				}

				// token_struct token_tmp;
			   //  cout<<"2>"<<word<<"<"<<"���ҷ���ֵ"<<IsDelimiter(word)<<endl; 
				if (IsDelimiter(word) >= 0) {// �����Ϊ��,�ұ������������
					token_struct token_tmp;
					token_tmp.type = tokenType(3);
					token_tmp.id = IsDelimiter(word);
					token_tmp.row = line_num;
					token_singleword.first = word;
					token_singleword.second = token_tmp;
					tokenList.push_back(token_singleword);

					//	cout<<"�������еķ���token"<<token_singleword.first<<endl;
				}
				else {
					//cout<<word<<endl;
					cout << "�Ƿ��ַ�!!!" << endl;
					exit(1);
				}
				_charnum++;
				return token_singleword;
				break;
			default:
				cout << "\n�ַ�ʶ���쳣:�޷�ʶ���״̬" << endl;
				exit(1);
				break;
			}
		}

		if (_charnum == text.length()) {
			token_struct token_tmp;
			token_tmp.type = tokenType(3);
			token_tmp.id = 40;
			token_tmp.row = line_num;
			pair<string, token_struct> token_singleword("#", token_tmp);
			tokenList.push_back(token_singleword);
			return token_singleword;
		}

	}


	// char_select_word �Ǵ��ı���Ϣ�ж�ȡ�ַ�


   //�ж��Ƿ�Ϊ�ؼ��ʺͽ��
	int IsDelimiter(string ch)		//�ж��Ƿ�Ϊ���
	{
		//	cout<<"Ҫ���ҵĵ���Ϊ>"<<ch<<"<"<<endl; 
		for (int i = 0; i < 45; i++) {

			if (ch == pt[i]) {
				//	cout<<"���:"<<ch<<endl; 
				return i;
			}
		}
		return -1;
	}
	int IsKeywords(string ch)		//�ж��Ƿ�Ϊ
	{

		for (int i = 0; i < 33; i++) {
			if (ch == kt[i]) {
				return i;
			}
		}
		return -1;
	}
	//

	int IsIdentifier(string ch) {
		for (int i = 0; i < 100; i++) {
			if (ch == it[i]) {
				return i;
			}
		}
		return -1;
	}
	int IsConstnum(string ch) {
		for (int i = 0; i < 100; i++) {
			if (ch == ct[i]) {
				return i;
			}
		}
		return -1;
	}
	int IsString(string ch) {
		for (int i = 0; i < 100; i++) {
			if (ch == st[i]) {
				return i;
			}
		}
		return -1;
	}
	int IsChar(string ch) {
		for (int i = 0; i < 100; i++) {
			if (ch == zt[i]) {
				return i;
			}
		}
		return -1;
	}
};
/*
class lex {
public:
	char ch;
	string path;//�ļ���ַ 
	string text;//ʵ���ı���Ϣ(������ʵ�ı�)
	string getit(string a);//������Ĵ���Ϣ���д���
	string find_include(string pre_string);//�������ļ������Ҳ���,�ϲ������ļ������ڵ��ļ���
	string get_include(string _defind, string _trueString);//
	string openfile(string path);//�����ڵ��ļ������ݶ�ȡ��������
	pair<string, token_struct> char_select_word(string path);//ʶ���ַ���ת����һ��token
	int IsDelimiter(string ch);//�����ɵ�token���������ж�
	int IsKeywords(string ch);
	int IsIdentifier(string ch);
	int IsConstnum(string ch);
	int IsString(string ch);
	int IsChar(string ch);
};
*/