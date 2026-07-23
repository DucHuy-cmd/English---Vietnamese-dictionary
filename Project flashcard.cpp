#include <iostream>
#include <cstring>
#include <conio.h> 
#include <cstdlib>
#include <ctime>
#include <windows.h>
using namespace std;

//hang doi tr loi cau hoi (viet theo mang cho de fisher-yates) va toi da dua vao 10 tu
//cau nao duoc xao len dau se duoc tra loi, sau khi trl xog se them vào stack va xoa di va them cau khac vao cuoi  
typedef struct queueVocabulary {
	char Voc[50];
	char A[50]; 
	char B[50];
	char C[50];
	char D[50]; 
	char correctAnswer[50]; 
}qV; 

//lich su chua cac cau tra loi. Tuc la lay tu queue them vao stack
//Se viet theo linkedlist 
typedef struct stackVocabulary {
	char Voc[50];
	char correctAnwser[50]; 
	char yourAnwser[50]; 
	stackVocabulary *next; 
}sV; 

#define MAX 10 

class Vocabulary {
	private:
		qV examEnglish[MAX]; 
		sV *top; // phan tu dau tien cua stack
		int size;// kich thuoc cua stack 
		int front;
		int rear; 
		int num; 
		
		Trie *dictRef;   // con tro toi tu dien, gan 1 lan luc khoi tao
		int loadIndex;   // vi tri tu tiep theo se nap vao queue
	public: 
		Vocabulary();
		~Vocabulary();
		
		void moduleExam ();
		
		void push(char Voca[], char CA[], char YA[]); //them vao dinh 
		// pop //xoa dinh // t?m ko xai 
		void vAHistory (); 
		sV *createNode(char Voca[], char CA[], char YA[]); 
		
		void moduleSaved (char Voca[], char CA[], char YA[]);//Chuyen nhung cau da tra loi vao stack 
		void setDictionary(Trie &dict);
		void enQueue (char Voca[],char A[],char B[],char C[],char D[],char CA[]);// phuc vu cho module truyen cau hoi 
		void deQueue();// xoa cuoi 
		void moduleLoadFromTrie();
		void shuffle();// xao tron; // nhap thoi dung chot cho nay nua 
		
};

Vocabulary::Vocabulary(){
	top=NULL;
	size=0; 
	front=-1;
	rear=-1;
	num=0; 
	dictRef=nullptr;
	loadIndex=0;
	srand(time(NULL));
}

Vocabulary::~Vocabulary(){
	while (top!=NULL){
		sV *p=top;
		top = top->next;
		delete p; 
	} 
	front = -1;
    rear = -1;
    num = 0;
} 

sV *Vocabulary::createNode(char Voca[], char CA[], char YA[]) {
	sV *newNode = new sV();
	if (newNode == NULL){
		cout<<"Memory allocation failed!"<<endl;
		return NULL; 
	} 
	strcpy(newNode->Voc, Voca);
	strcpy(newNode->correctAnwser, CA);
	strcpy(newNode->yourAnwser, YA);
	newNode->next=nullptr; 
	return newNode; 
} 

void Vocabulary::vAHistory(){
	sV*p = top;
	while (p!=NULL){
		cout<<"The question: "<< p->Voc<<endl; 
		cout<<"Correct anwser: "<< p->correctAnwser<<endl;
		cout<<"Your anwser: "<< p->yourAnwser<<endl; 
		cout<<"\n"; 
		p=p->next; 
	} 
	cout<< "Press Enter to return!"<<endl;
	cin.ignore(1000, '\n');
	int a=_getch();
	if (a==13){
		return; 
	} 
} 

void Vocabulary::push(char Voca[], char CA[], char YA[]){//stack 
	sV *newNode = createNode(Voca,CA,YA);
	if (top==nullptr){
		top = newNode; 
	} 
	else {
		newNode->next=top;
		top = newNode; 
	} 
	size++; 
}

void Vocabulary::enQueue(char Voca[],char A[],char B[],char C[],char D[],char CA[]){//queue 
	if (num==MAX){
		cout<<"Add failed, List is full!"<<endl; 
		return; 
	} 
	else if (front==-1){
		strcpy (examEnglish[0].Voc,Voca);
		strcpy (examEnglish[0].A,A);
		strcpy (examEnglish[0].B,B);
		strcpy (examEnglish[0].C,C);
		strcpy (examEnglish[0].D,D);
		strcpy (examEnglish[0].correctAnswer,CA); 
		front=rear=0;
		num++; 
	}
	else if (rear+1==MAX){
		strcpy (examEnglish[0].Voc,Voca);
		strcpy (examEnglish[0].A,A);
		strcpy (examEnglish[0].B,B);
		strcpy (examEnglish[0].C,C);
		strcpy (examEnglish[0].D,D); 
		strcpy (examEnglish[0].correctAnswer,CA); 
		rear = 0; 
		num++; 
	}
	else {
		strcpy (examEnglish[rear+1].Voc,Voca);
		strcpy (examEnglish[rear+1].A,A);
		strcpy (examEnglish[rear+1].B,B);
		strcpy (examEnglish[rear+1].C,C);
		strcpy (examEnglish[rear+1].D,D); 
		strcpy (examEnglish[rear+1].correctAnswer,CA); 
		rear++;
		num++;
	} 
}

void Vocabulary::deQueue(){
	if (num == 0){
		cout << "No more anwser!" <<endl;
		return; 
	} 
	else if (front == rear){
		front = rear = -1;
		num =0; 
	} 
	else if (front + 1 == MAX) {
		front = 0;
		num--;
	}
	else {
		front++;
		num--; 
	} 
	// module chuyen question vao queue them (chua code)
}

void Vocabulary::moduleSaved(char Voca[], char CA[], char YA[]){
	if (num == 0){
		cout<<"There aren't any more questions."<<endl;
		return; 
	} 
	else{
		push(Voca,CA,YA);
		deQueue();
		moduleLoadFromTrie();
	} 
}

void Vocabulary::moduleExam(){
	while (1){
		if (num == 0){
			cout<<"There aren't any more questions."<<endl;
			return;
		}
		else {
			shuffle(); 
			system("cls");
			cout << examEnglish[front].Voc<<endl;
			cout << "A:"<< examEnglish[front].A << "	" << 
					"B:"<< examEnglish[front].B << "	" << 
					"C:"<< examEnglish[front].C << "	" << 
					"D:"<< examEnglish[front].D << endl;
			cout << "Press ESC to exit!" <<endl; 
			cin.ignore(1000, '\n');
			char Anwser = _getch();
			if (Anwser == 27) 
			    return;
			while ((Anwser < 'a' || Anwser > 'd') && (Anwser < 'A' || Anwser > 'D') ) {
				cout << "There is no suitable answer. Please enter again!" << endl; 
				Anwser = _getch();
				if (Anwser == 27) 
			    	return;
			}
			switch (Anwser) {
	    		case 'A':
	   			case 'a':
	       			if (strcmp(examEnglish[front].A, examEnglish[front].correctAnswer) == 0)
	            		cout << "Correct!" ;
	        		else
	            		cout << "Wrong! Correct anwser: " << examEnglish[front].correctAnswer;
					moduleSaved(examEnglish[front].Voc, examEnglish[front].correctAnswer, examEnglish[front].A);
					Sleep(1000);
						break;
			    case 'B':
			    case 'b':
			        if (strcmp(examEnglish[front].B, examEnglish[front].correctAnswer) == 0)
			            cout << "Correct!";
			        else
			            cout << "Wrong! Correct anwser: " << examEnglish[front].correctAnswer;
			       	moduleSaved(examEnglish[front].Voc, examEnglish[front].correctAnswer, examEnglish[front].B);
			   		Sleep(1000);
			        break;
			
			    case 'C':
			    case 'c':
			        if (strcmp(examEnglish[front].C, examEnglish[front].correctAnswer) == 0)
			            cout << "Correct!";
			        else
			        	cout << "Wrong! Correct anwser: " << examEnglish[front].correctAnswer; 
			       	moduleSaved(examEnglish[front].Voc, examEnglish[front].correctAnswer, examEnglish[front].C);
			       	Sleep(1000);
			        break;
			
			    case 'D':
			    case 'd':
			        if (strcmp(examEnglish[front].D, examEnglish[front].correctAnswer) == 0)
			            cout << "Correct!";
			        else
			        	cout << "Wrong! Correct anwser: " << examEnglish[front].correctAnswer;
			        moduleSaved(examEnglish[front].Voc, examEnglish[front].correctAnswer, examEnglish[front].D);
			        Sleep(1000);
			        break;
			} 
		} 
	} 		
} 

void Vocabulary::shuffle() {
    for (int i = num - 1; i > 0; i--) {

        int j = rand() % (i + 1);

        int idx1 = (front + i) % MAX;
        int idx2 = (front + j) % MAX;

        qV temp = examEnglish[idx1];
        examEnglish[idx1] = examEnglish[idx2];
        examEnglish[idx2] = temp;
    }
    char *ans[4] = {
        examEnglish[front].A,
        examEnglish[front].B,
        examEnglish[front].C,
        examEnglish[front].D
    };

    for (int i = 3; i > 0; i--) {

        int j = rand() % (i + 1);

        char temp[50];
        strcpy(temp, ans[i]);
        strcpy(ans[i], ans[j]);
        strcpy(ans[j], temp);
    }
    strcpy (examEnglish[front].A,ans[0]);
    strcpy (examEnglish[front].B,ans[1]);
    strcpy (examEnglish[front].C,ans[2]);
    strcpy (examEnglish[front].D,ans[3]);
}

int menu(Vocabulary &t) {
    int choice;
    while (1) {
        system("cls");
        cout << "=========================================\n";
        cout << "       ENGLISH VOCABULARY PROGRAM\n";
        cout << "=========================================\n";
        cout << "1. Exam\n";
        cout << "2. View answer history\n";
        cout << "0. Exit\n";
        cout << "=========================================\n";
        cout << "Enter your choice: ";
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input!" << endl;
            system("pause");
            continue;
        }
        switch (choice) {
            case 1:
            	t.moduleExam();
            	break; 
            case 2:
            	t.vAHistory(); 
            	break; 
            case 0:
                return choice;
            default:
                cout << "Invalid choice!" << endl;
                system("pause");
        }
    }
}

void Vocabulary::moduleLoadFromTrie() {
    if (dictRef == nullptr) return; // chua gan tu dien thi thoi

    const vector<pair<string, string>> &data = dictRef->getAllWords();
    int total = (int)data.size();
    if (total == 0 || loadIndex >= total) return; // het tu de nap

    string word = data[loadIndex].first;
    string correctMeaning = data[loadIndex].second;
    loadIndex++;

    vector<string> wrongChoices;
    int guard = 0;
    while (wrongChoices.size() < 3 && guard < 100) {
        int r = rand() % total;
        bool trung = (data[r].second == correctMeaning);
        for (auto &w : wrongChoices)
            if (w == data[r].second) trung = true;
        if (!trung) wrongChoices.push_back(data[r].second);
        guard++;
    }
    while (wrongChoices.size() < 3) wrongChoices.push_back("N/A");

    string choices[4] = {correctMeaning, wrongChoices[0], wrongChoices[1], wrongChoices[2]};
    for (int k = 3; k > 0; k--) {
        int j = rand() % (k + 1);
        swap(choices[k], choices[j]);
    }

    char Voc[50], A[50], B[50], C[50], D[50], CA[50];
    strncpy(Voc, word.c_str(), 49); Voc[49] = '\0';
    strncpy(A, choices[0].c_str(), 49); A[49] = '\0';
    strncpy(B, choices[1].c_str(), 49); B[49] = '\0';
    strncpy(C, choices[2].c_str(), 49); C[49] = '\0';
    strncpy(D, choices[3].c_str(), 49); D[49] = '\0';
    strncpy(CA, correctMeaning.c_str(), 49); CA[49] = '\0';

    enQueue(Voc, A, B, C, D, CA);
}

void Vocabulary::setDictionary(Trie &dict) {
    dictRef = &dict;
}

// Update (23/07/2026) 
// Note:  const vector<pair<string, string>>& getAllWords() const { return allWords; } này thêm vao class Trie 
// hàm setDictionary dung de gan dia chi cua doi tuong trie vao con tro dictRef 
// hàm moduleLoadFromTrie nap cau hoi vao queue moi lan goi nap 1 lan
// Trie *dictRef;   // con tro toi tu dien, int loadIndex; vi tri tu tiep theo se nap (Hai thuoc tinh moi) 
// tao nhung khoang nghi giua nhung lan tra loi
// lich su tra loi de nhin hon 

