#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include <algorithm>


using namespace std;

#define WIDTH	1
#define HEIGHT	2
#define INIT	3
#define UPDATE	4
#define END		999
#define KEEP	6
#define NON		-1
#define MAX_LENGTH	50

class Map{
public:
	char data;
	vector<int> mark;
	Map(char _d){
		data = _d;
	}
	Map() {}
	void setMark(int num){
		mark.push_back(num);
	}
};

int row, col;
int wCount;		// ���� ����
int hCount;		// ���� ����
Map **map;		// FILED
ifstream file;	// FILE
vector<string> dic[MAX_LENGTH];
vector<char> history;


class InsWord{
public:
	int x, y;	// ���� ��ǥ
	int len;	// ���� getLen() ���ϰ�
	int type;	// ���� ���� Ÿ��
	int dicPos = 0;			//  setWord �� ���� ��ġ ����Ʈ
	int markPos = 0;		//  mark ��ġ
	int number;					// �ڽ��� �ѹ�, ���� ���� mark �뵵
	Map **&ref_map = map;	// �� ���۷���
	vector<string> wordList;	// ��Ī ���� ���� ����Ʈ
	vector<string> filterList;	// ��Ī ���� ���� ����Ʈ
	vector<int> crossList;			// �ڽŰ� ������ �ѹ� ��� (�ϳ��� ���)
	string preWord;				// setWord()�ϱ� ���� ����� ��Ʈ��
	bool change = true;		// �ٲ� ��ȣ ó��

	InsWord(int _x, int _y, int _type, int num){
		x = _x;
		y = _y;
		type = _type;
		number = num;
		len = getLen();

	}
	int getMarkPos(){ return markPos; }
	void incMarkPos(){ markPos++; }
	void decMarkPos(){ markPos--; }
	void initMarkPos(){ markPos = 0; }
	int getMarkNum(){
		incMarkPos(); return crossList[markPos];
	}
	void getCross();	// ������ ����
	int checkWord();	// ���� ���� _ĭ ����	
	// �ܾ� ���� ����Ʈ ����
	int setList(string findWord, int pos, vector<string> temp, int env);

	// �ܾ� ���� ����Ʈ �ʱ�ȭ
	int initList();

	int setWord(); // map�� �ܾ� ����
	void rollBack(); // ������ �ܾ� ����
	string getWord(); // ���� ��ü �ܾ�
	int getLen(); // ���� ��ü ����
	void changeSig(); // �ٲ� ��ȣ�� ������ list�� ���� ������ ����

	friend std::ostream& operator<<(std::ostream& out, const InsWord *p){
		return out;
		// WORD �׽�Ʈ ���۷�����
	}
};
void InsWord::getCross(){
	int i = x, j = y;
	// �ڽ��� �������� �� ����ü�� Ž���ϸ鼭 �ʱ⿡ �����
	// mark�� 2 �̻��� ��� ���� ���(crossList)�� ������
	if (type == WIDTH){
		while (ref_map[i][j].data != '*'){
			if (map[i][j].mark.size() > 1)
			for (int k = 0; k < map[i][j].mark.size(); k++){
				crossList.push_back(map[i][j].mark[k]);
			}
			j++;
			if (j == row)
				break;
		}
	}
	if (type == HEIGHT){
		while (ref_map[i][j].data != '*'){
			if (map[i][j].mark.size() > 1)
			for (int k = 0; k < map[i][j].mark.size(); k++){
				crossList.push_back(map[i][j].mark[k]);
			}
			i++;
			if (i == col)
				break;
		}
	}
}
int InsWord::checkWord(){
	// _ĭ ���� ������ ��� ������ ���� ����
	string word = getWord();
	for (int i = 0; i < len; i++){
		if (word[i] == '_') return KEEP;
	}
	return END;
}
int InsWord::setList(string findWord, int pos, vector<string> temp, int env){
	// ���� _ĭ�� ������ END ����
	if (checkWord() == END)
		return END;
	// ���� �ۼ�
	if (env == INIT){
		wordList.clear();
		wordList = temp;
		filterList = wordList;
	}
	// ����
	if (env == UPDATE){
		filterList.clear();
		filterList = temp;
	}

	vector<string> newList; // �ӽ� ����Ʈ

	for (int i = pos; i < len; i++){
		if (findWord[i] != '_'){
			for (int j = 0; j < temp.size(); j++){
				if (findWord[i] == temp[j][i]){
					newList.push_back(temp[j]);
				}
			}
			// �ӽ� ����Ʈ�� �� ���
			return setList(findWord, i + 1, newList, env);
		}
	}
	// �ڽ��� ���� �ȿ� ������(change, getWord())�� �������� ������
	if (env == UPDATE)
		return filterList.size();
	else
		return wordList.size();
}
int InsWord::initList(){
	// ���� �� ���� ���� �뵵
	wordList.clear();
	return setList(getWord(), 0, dic[len], INIT);
}
int InsWord::setWord(){
	// ���� �� Ž������ ��� END ����
	if (dicPos >= filterList.size())
		return END;
	// ���� �ڽ� ���� �ȿ� ���� ���� �־��� ��� ���� ����
	if (change == true){
		if (setList(getWord(), 0, wordList, UPDATE) == 0){
			return NON;
		}
		changeSig();
	}
	// ���ŵ� ����Ʈ�� �ƿ� ��� ���ٸ� ���� ���
	if (filterList.size() == 0){
		return NON;
	}
	// ���� string ���
	preWord = getWord();

	// map�� ����
	if (type == WIDTH)
	for (int i = y; i < len + y; i++){
		ref_map[x][i] = filterList[dicPos][i - y];
	}


	if (type == HEIGHT)
	for (int i = x; i < len + x; i++){
		ref_map[i][y] = filterList[dicPos][i - x];
	}

	// ���� ������ ����
	dicPos++;

	return KEEP;
}
void InsWord::rollBack(){
	// ��� �ܾ ����
	if (type == WIDTH)
	for (int i = y; i < len + y; i++){
		ref_map[x][i] = preWord[i - y];
	}


	if (type == HEIGHT)
	for (int i = x; i < len + x; i++){
		ref_map[i][y] = preWord[i - x];
	}
}

string InsWord::getWord(){
	string word;

	// �ڽ��� ���� ���� ���� �ܾ �����Ͽ� ����
	if (type == WIDTH)
	for (int i = y; i < y + len; i++){
		word.append(1, ref_map[x][i].data);
	}

	if (type == HEIGHT)
	for (int i = x; i < x + len; i++){
		word.append(1, ref_map[i][y].data);
	}
	return word;
}
int InsWord::getLen(){
	// ���� ���� �������� Ÿ�Կ� ���� �������� �������� ���� ����
	int i = x, j = y, _len = 0;
	if (type == WIDTH){
		while (ref_map[i][j].data != '*'){
			ref_map[i][j].setMark(number);
			_len++;
			j++;
			if (j == row)
				break;
		}
	}
	if (type == HEIGHT){
		while (ref_map[i][j].data != '*'){
			ref_map[i][j].setMark(number);
			_len++;
			i++;
			if (i == col)
				break;
		}
	}
	return _len;
}

void InsWord::changeSig(){
	dicPos = 0;
	change = false;
}

void printMap(){
	for (int i = 0; i < row; i++){
		for (int j = 0; j < col; j++){
			cout << map[i][j].data;
		}
		cout << endl;
	}
}

int compute(vector<InsWord*> words){
	int pos = 0;
	
	while (1){
		if (words[pos]->setWord() == NON){
			cout << "ERORR" << endl;
			break;
		}
	//	//if (words[pos]->crossList.size() != 0){
	//		pos = words[pos]->getMarkNum();
	//	}
		system("cls");
		printMap();
		system("pause");
		pos++;
	}
	
	return END;
}

void crossCheck(vector<InsWord*> words){
	for (int i = 0; i < row; i++){
		for (int j = 0; j < col; j++){
			if (map[i][j].mark.size() > 1){
				cout << "���� ���� : x = " << i << " y = " << j << " " << "���� �� : " << map[i][j].mark.size() << endl;
				cout << "�����ϴ� ��ϰ� �� ������ : ";
				for (int k = 0; k < map[i][j].mark.size(); k++){
					cout << "(" << map[i][j].mark[k] << ")" << words[map[i][j].mark[k]]->getWord() << " , ";
				}
				cout << "���� ��� : [" << map[i][j].data << "]";
				cout << endl;
			}

		}
	}

}


int main() {
	clock_t start = clock();
	file.open("input.txt");

	int wordCount;
	file >> wordCount;

	
	for (int i = 0; i < wordCount; i++) {
		char *newWord = new char[MAX_LENGTH];
		file >> newWord;
		string *newString = new string(newWord);
		dic[newString->size()].push_back(newWord);
		// �ܾ� ���̹�° dic ĭ�� �ִ� �з�
	}

	int cases;
	file >> cases;

	while (cases--){
		file >> row >> col;

		map = new Map*[row];
		for (int i = 0; i < row; i++){
			map[i] = new Map[col];
		}
		for (int i = 0; i < row; i++){
			for (int j = 0; j < col; j++)
				file >> map[i][j].data;
		}

		// ���� ���
		printMap();
		
		file >> wCount >> hCount;

		vector<InsWord*> words;
		for (int i = 0; i < wCount; i++){
			int x, y;
			file >> x >> y;
			InsWord *newWord = new InsWord(x - 1, y - 1, WIDTH, i);
			words.push_back(newWord);
		}
		// ���� ����
		for (int i = 0; i < hCount; i++){
			int x, y;
			file >> x >> y;
			InsWord *newWord = new InsWord(x - 1, y - 1, HEIGHT, i + wCount);
			words.push_back(newWord);
		}
		// ���� ����

		// ���� ���� ���� �׽�Ʈ ���
		//crossCheck(words);

		for (int i = 0; i < words.size(); i++){
			words[i]->initList(); // �ܾ� �ʱ�ȭ
			words[i]->getCross(); // ũ�ν� ����
		}

		compute(words);
		// ��� �κ�

	}

	clock_t end = clock(); // �ð� �׽�Ʈ

	cout << end - start << endl;

}