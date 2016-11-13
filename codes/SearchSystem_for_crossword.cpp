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
int wCount;		// 가로 갯수
int hCount;		// 세로 갯수
Map **map;		// FILED
ifstream file;	// FILE
vector<string> dic[MAX_LENGTH];
vector<char> history;


class InsWord{
public:
	int x, y;	// 시작 좌표
	int len;	// 길이 getLen() 리턴값
	int type;	// 가로 세로 타입
	int dicPos = 0;			//  setWord 시 사전 위치 포인트
	int markPos = 0;		//  mark 위치
	int number;					// 자신의 넘버, 교차 지점 mark 용도
	Map **&ref_map = map;	// 맵 레퍼런스
	vector<string> wordList;	// 매칭 사전 원본 리스트
	vector<string> filterList;	// 매칭 사전 갱신 리스트
	vector<int> crossList;			// 자신과 교차된 넘버 목록 (하나인 경우)
	string preWord;				// setWord()하기 이전 백업된 스트링
	bool change = true;		// 바뀐 신호 처리

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
	void getCross();	// 교차점 조사
	int checkWord();	// 현재 워드 _칸 조사	
	// 단어 사전 리스트 갱신
	int setList(string findWord, int pos, vector<string> temp, int env);

	// 단어 사전 리스트 초기화
	int initList();

	int setWord(); // map에 단어 기입
	void rollBack(); // 기입한 단어 복구
	string getWord(); // 현재 객체 단어
	int getLen(); // 현재 객체 길이
	void changeSig(); // 바뀐 신호를 받으면 list와 사전 포인터 갱신

	friend std::ostream& operator<<(std::ostream& out, const InsWord *p){
		return out;
		// WORD 테스트 오퍼레이터
	}
};
void InsWord::getCross(){
	int i = x, j = y;
	// 자신의 영역안의 맵 구조체를 탐색하면서 초기에 기록한
	// mark가 2 이상인 경우 교차 목록(crossList)에 기입함
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
	// _칸 조사 있으면 계속 없으면 끝을 리턴
	string word = getWord();
	for (int i = 0; i < len; i++){
		if (word[i] == '_') return KEEP;
	}
	return END;
}
int InsWord::setList(string findWord, int pos, vector<string> temp, int env){
	// 만약 _칸이 없으면 END 리턴
	if (checkWord() == END)
		return END;
	// 최초 작성
	if (env == INIT){
		wordList.clear();
		wordList = temp;
		filterList = wordList;
	}
	// 갱신
	if (env == UPDATE){
		filterList.clear();
		filterList = temp;
	}

	vector<string> newList; // 임시 리스트

	for (int i = pos; i < len; i++){
		if (findWord[i] != '_'){
			for (int j = 0; j < temp.size(); j++){
				if (findWord[i] == temp[j][i]){
					newList.push_back(temp[j]);
				}
			}
			// 임시 리스트가 들어간 재귀
			return setList(findWord, i + 1, newList, env);
		}
	}
	// 자신의 영역 안에 변경점(change, getWord())을 기준으로 갱신함
	if (env == UPDATE)
		return filterList.size();
	else
		return wordList.size();
}
int InsWord::initList(){
	// 최초 맵 구성 갱신 용도
	wordList.clear();
	return setList(getWord(), 0, dic[len], INIT);
}
int InsWord::setWord(){
	// 사전 다 탐색했을 경우 END 리턴
	if (dicPos >= filterList.size())
		return END;
	// 만약 자신 구역 안에 변경 점이 있었을 경우 새로 갱신
	if (change == true){
		if (setList(getWord(), 0, wordList, UPDATE) == 0){
			return NON;
		}
		changeSig();
	}
	// 갱신된 리스트가 아예 없어서 막다른 길일 경우
	if (filterList.size() == 0){
		return NON;
	}
	// 이전 string 백업
	preWord = getWord();

	// map에 기입
	if (type == WIDTH)
	for (int i = y; i < len + y; i++){
		ref_map[x][i] = filterList[dicPos][i - y];
	}


	if (type == HEIGHT)
	for (int i = x; i < len + x; i++){
		ref_map[i][y] = filterList[dicPos][i - x];
	}

	// 사전 포인터 증가
	dicPos++;

	return KEEP;
}
void InsWord::rollBack(){
	// 백업 단어를 기입
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

	// 자신의 현재 영역 안의 단어를 생성하여 리턴
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
	// 영역 시작 지점부터 타입에 따른 방향으로 벽까지의 길이 리턴
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
				cout << "교차 지점 : x = " << i << " y = " << j << " " << "교차 수 : " << map[i][j].mark.size() << endl;
				cout << "교차하는 목록과 그 포인터 : ";
				for (int k = 0; k < map[i][j].mark.size(); k++){
					cout << "(" << map[i][j].mark[k] << ")" << words[map[i][j].mark[k]]->getWord() << " , ";
				}
				cout << "교차 장소 : [" << map[i][j].data << "]";
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
		// 단어 길이번째 dic 칸에 넣는 분류
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

		// 원본 출력
		printMap();
		
		file >> wCount >> hCount;

		vector<InsWord*> words;
		for (int i = 0; i < wCount; i++){
			int x, y;
			file >> x >> y;
			InsWord *newWord = new InsWord(x - 1, y - 1, WIDTH, i);
			words.push_back(newWord);
		}
		// 가로 셋팅
		for (int i = 0; i < hCount; i++){
			int x, y;
			file >> x >> y;
			InsWord *newWord = new InsWord(x - 1, y - 1, HEIGHT, i + wCount);
			words.push_back(newWord);
		}
		// 세로 셋팅

		// 교차 지점 정보 테스트 출력
		//crossCheck(words);

		for (int i = 0; i < words.size(); i++){
			words[i]->initList(); // 단어 초기화
			words[i]->getCross(); // 크로스 셋팅
		}

		compute(words);
		// 계산 부분

	}

	clock_t end = clock(); // 시간 테스트

	cout << end - start << endl;

}