#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <windows.h>
#include <string>
#include <conio.h>
using namespace std;

HANDLE console_color = GetStdHandle(STD_OUTPUT_HANDLE); //pang-set ng text color sa console.
const int SCREEN_WIDTH = 175; //width ng screen, ginagamit pang-center ng text.
int arrayChar[256]; //array para i-store ang character frequencies (base sa ASCII value).

void gotoxy(int x, int y) { //ito ang nagse-set ng position ng cursor sa console coordinates (x, y).
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord); //pwede natin ilipat ang cursor sa kahit anong posisyon sa console kapag ginamit ito.
}

void setColor(int color) { //nagse-set ng kulay ng text.
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void centerText(int y, const string& text) { //Center the text in y row.
    int x = (SCREEN_WIDTH - text.length()) / 2;  //Calculate the x position based sa length ng text at screen width.
    if (x < 0) x = 0;  //if the text is too wide for the screen.
    gotoxy(x, y);
    cout << text;
}

struct Node { //Struct para sa nodes ng Huffman tree
    int value;
    char ch;
    Node* next;
    Node* left;
    Node* right;

    Node(int x, char c) : ch(c), value(x), next(NULL), left(NULL), right(NULL) {}
};

class PriorityQueue { //nag-sstore ng elements based sa priority.
public:
    Node* head;
    Node* huff;
    string huffmancode[256];

    PriorityQueue() : head(NULL), huff(NULL) {} //iniinitialize ang head at huff.

    string code(char& c) {  //para kumuha ng Huffman code ng character
        for (int i = 0; i < 256; i++) { //Loops from 0 to 255 para hanapin ang code na naka-store sa huffmancode.
            if (i == (int)c) {
                return huffmancode[(int)c];
            }
        }
        return ""; //kung walang match, nagri-return ng empty string
    }

    void huffmanCode(Node* r, string codePerChar, int& yPosition) { //dito ginagawa ang pre-order traversal ng Huffman tree.
        if (r == NULL) {
            return;
        }

        huffmanCode(r->left, codePerChar + "0", yPosition); //Traverse left child, dagdag ng "0"
        huffmanCode(r->right, codePerChar + "1", yPosition); // Traverse right child, dagdag ng "1"

        //IF leaf node, print character, frequency, at Huffman code
        if (r->left == NULL && r->right == NULL) {
            stringstream ss;
            ss << r->ch << "       " << r->value << "       " << codePerChar;
            string row = ss.str();

            setColor(7); //set color to default
            centerText(yPosition++, row);

            huffmancode[(int)r->ch] = codePerChar;
        }
    }

    Node* generateHuffman() { //nag-ge-generate ng Huffman tree mula sa priority queue.
        while (head->next != NULL) {
            dequeue();
        }
        huff = head;
        head = NULL;

        return huff;
    }

    void insertQueue() { //para mag-insert ng mga nodes sa queue based sa frequency.
        for (int index = 0; index < 256; index++) {
            if (arrayChar[index] > 0) { //kung may frequency ang character
                Node* temp = new Node(arrayChar[index], (char)index);
                enqueue(temp);
            }
        }

        int retrievedDataStartY = 18;
        int retrievedDataEndY = retrievedDataStartY;

        string retrievedData = "";
        stringstream ss(retrievedData); //para mag-format ng retrieved data sa screen.
        string line;

        while (getline(ss, line)) {
            centerText(retrievedDataEndY++, line);
        }

        retrievedDataEndY++;

        centerText(retrievedDataEndY + 1, "==========================================");
        setColor(10);
        centerText(retrievedDataEndY + 2, "Huffman Table");
        setColor(7);
        centerText(retrievedDataEndY + 3, "==========================================");
        setColor(10);
        centerText(retrievedDataEndY + 5, "CHAR    TALLY   HUFFMAN");

        Node* tempRoot = generateHuffman();
        int yPosition = retrievedDataEndY + 7;
        huffmanCode(tempRoot, "", yPosition);

        cout << endl;
        system("pause");
    }

    void enqueue(Node* newNode) { //dinadagdag yung bagong node sa priority queue, in-order ng frequency.
        if (!head || newNode->value < head->value) {
            newNode->next = head;
            head = newNode;
        } else {
            Node* current = head;

            while (current->next && newNode->value >= current->next->value) {
                current = current->next;
            }

            newNode->next = current->next;
            current->next = newNode;
        }
    }

    void dequeue() { //combine 2 nodes (w/ long frequency) to make a new node.
        if (head == NULL) {
            system("cls");
            cout << "Priority Queue is empty." << endl;
            return;
        }

        while (head->next != NULL) {
            Node* temp = head;
            Node* temp1 = head->next;

            Node* roottemp = new Node(temp->value + temp1->value, '\0');
            roottemp->left = temp;
            roottemp->right = temp1;
            enqueue(roottemp);
            head = head->next->next;
        }
    }

    bool isEmpty() { //pancheck kung ang priority queue ay empty o wala nang laman.
        return head == NULL;
    }

    void saveLeaf(Node* r, ofstream& fpc) { //pinipili at isinusulat sa file yung mga leaf nodes
        if (r == NULL) {
            return;
        }

        saveLeaf(r->left, fpc);
        saveLeaf(r->right, fpc);

        if (r->left == NULL && r->right == NULL) {
            if (r->ch == ' ') {
                fpc << "sp" << endl<< r->value << endl;
            } else if (r->ch == '\n') {
                fpc << "newline" << endl<< r->value << endl;
            } else if (r->ch == '\t') {
                fpc << "tab" << endl<< r->value << endl;
            } else if(r->ch == 13) {
                fpc << "Ascii13" << endl<< r->value << endl;
            } else {
                fpc << r->ch << endl<< r->value << endl;
            }
        }
    }

    void hufftrav(ofstream& fpc) { //Traverses through the Huffman tree and saves all leaf nodes to a file.
        saveLeaf(huff, fpc);
    }

    void renderMessage(string filename) { //will read a file, converts it into characters in Huffman codes.
        ifstream fp(filename);

        if (!fp.is_open()) {
            system("cls");
            cout << "Error";
            return;
        } else {
            char c;
            string bin;

            while (fp.get(c)) {
                bin += code(c);
            }

            if (bin.length() % 6 == 0) { //kung (bin.length()) ay divisible by 6, diretso itong isi-save at dinedecode.
                save(bin.length());
                decodeBin(bin);
            } else { //kapag hindi divisible by 6
                save(bin.length());
                int remainderVal = bin.length() % 6;
                remainderVal = 6 - remainderVal;

                for (int i = remainderVal; i > 0; i--) {
                    bin = bin + "0";
                }
                decodeBin(bin);
            }
        }
    }

    void save(int size) { //Saves the size of the compressed data
        ofstream fp("compressed_data.txt");

        if (!fp.is_open()) {
            system("cls");
            cout << "Error" << endl;
            return;
        } else {
            fp << size;
        }

        ofstream fpc("char_freq.txt");

        if (!fpc.is_open()) {
            system("cls");
            cout << "Error" << endl;
            return;
        } else {
            hufftrav(fpc);
        }
    }

    void decodeBin(string& binary) { //Decodes the binary string using the Huffman tree.
        stringstream ss;

        for (size_t i = 0; i < binary.length(); i += 6) {
            string sixBits = binary.substr(i, 6);

            if (sixBits.length() < 6) {
                int padding = 6 - sixBits.length();
                sixBits = string(padding, '0') + sixBits;
            }

            int num = 0;

            for (size_t j = 0; j < sixBits.length(); j++) {
                if (sixBits[j] == '1') {
                    num += pow(2, 5 - j);
                }
            }

            num += 32;
            ss << (char)num;
        }

        string resultString = ss.str();
        ofstream fp("PROJECT2.dsaproj");

        if (!fp.is_open()) {
            system("cls");
            cout << "error" << endl;
            return;
        } else {
            fp << resultString;
        }
    }

    //Decompression Methods
    string binTobase10(int decimal) { //cinoconvert ang decimal number to a binary string of 6digits.
        string binary;

        if (decimal==0) {
            return binary = "000000";
        }

        while (decimal > 0 || binary.length() < 6) {
            binary = to_string(decimal % 2) + binary;
            decimal /= 2;
        }

        return binary;
    }

    int retrievesize() { //reads a size value
        ifstream fp("compressed_data.txt");

        if (!fp.is_open()) {
            system("cls");
            cout<<"File not found"<<endl;
            return -1;
        } else {
            int a;
            fp>>a;
            return a;
        }
    }

    void renderMessage() { //used to decode and display a message from a file
        string filename = "PROJECT2.dsaproj";
        ifstream fp(filename);

        if (!fp.is_open()) {
            system("cls");
            cout << "Error" << endl;
            return;
        } else {
            char c;
            string bin;

            while (fp.get(c)) {
                bin += c;
            }

            string binsequence;

            for (char ch: bin) {
                int deciVal = (int)ch;
                deciVal -= 32;
                binsequence += binTobase10(deciVal);
            }

            string BIN;
            int size=retrievesize();

            if (size > 0) {
                int index=0;

                while (index < size) {
                    BIN += binsequence[index];
                    index++;
                }

                system("cls");
                string header = "==========================================";

                centerText(5, header);
                setColor(10);
                centerText(6, "Output");
                setColor(7);
                centerText(7, header);
                cout << endl;

                string decodedMessage = decodeBin(BIN, huff);
                centerText(8, decodedMessage);

                cout << "\n";
                system("pause");
            } else {
                system("cls");
                SetConsoleTextAttribute(console_color, 4);
                gotoxy(58, 11); cout << "========================================================" << endl;
                gotoxy(82, 12); cout << "Error";
                gotoxy(58, 13); cout << "========================================================" << endl;
                gotoxy(59, 15); cout << "No input found.";
                gotoxy(58, 17); cout << "========================================================" << endl;
                gotoxy(59, 19); cout << "Press any key to return to the main menu...";
                cin.ignore();
                cin.get();
                return;
            }
        }
    }

    string decodeBin (string& binary, Node *temp) { //decodes a binary string using a Huffman tree
        string decodedMessage = "";
        int index=0;

        while (index<binary.length()) {
            if (binary[index]=='1') {
                temp=temp->right;
            } else if (binary[index]=='0') {
                temp=temp->left;
            }

            if (temp->left ==NULL && temp->right==NULL) {
                decodedMessage += temp->ch;
                temp = huff;
            }

            index++;
        }

        return decodedMessage;
    }

    //Function para i-rebuild ang Huffman tree during decompression
    void rebuildHuffmanTree() {
        system("cls");

        ifstream fp("char_freq.txt");

        if (!fp.is_open()) {
            system("cls");
            cout<<"File not found"<<endl;
            return;
        }

        string character;
        int value;

        while (fp >> character >> value) {
            char ch;
            if (character == "sp") ch = ' ';
            else if (character == "newline") ch = '\n';
            else if (character == "tab") ch = '\t';
            else if (character == "Ascii13") ch = 13;
            else ch = character[0];

            arrayChar[(int)ch] = value;
        }

        insertQueue();  //Rebuild the Huffman tree
    }
};

void showLoadingBar2(int duration) { //loading bar display
    int bar1 = 176, bar2 = 178;

    SetConsoleTextAttribute(console_color, 7);
    gotoxy(70, 20); cout << "Compressing File...";

    SetConsoleTextAttribute(console_color, 8);
    cout << "\n\n\t\t\t\t\t\t\t";

    for (int i = 0; i < 50; i++) { //ginagamit ang loop na ito para gumalaw ang loading bar display.
        cout << (char)bar2;
        Sleep(duration);
    }
    cout << endl;
}

void checkFile() { //checks if a file exists and processes its contents.
    PriorityQueue Queue;

    system("cls");
    SetConsoleTextAttribute(console_color, 7);
    gotoxy(58, 11); cout << "========================================================" << endl;
    gotoxy(77, 12); cout << "PLEASE ENTER FILE";
    gotoxy(58, 13); cout << "========================================================" << endl;
    gotoxy(58, 15); cout << "File: ";
    string file;
    cin >> file;

    ifstream fp(file);
    if (!fp.is_open()) {
    system("cls");
    SetConsoleTextAttribute(console_color, 4);
    gotoxy(58, 11); cout << "========================================================" << endl;
    gotoxy(82, 12); cout << "Error";
    gotoxy(58, 13); cout << "========================================================" << endl;
    gotoxy(59, 15); cout << "File does not exist! Please try again.";
    gotoxy(58, 17); cout << "========================================================" << endl;
    gotoxy(59, 19); cout << "Press any key to return to the main menu...";
    cin.ignore();
    cin.get();
    return;
} else {
    system("cls");
    showLoadingBar2(15);
    char c;

    for (int i = 0; i < 256; i++) {
        arrayChar[i] = 0;
    }

    system("cls");
    setColor(7);
    centerText(11, "==========================================");
    setColor(10);
    centerText(12, "Retrieved");
    setColor(7);
    centerText(13, "==========================================");

    string sample;

    while (fp.get(c)) {
        if (c>0 && c!='\0') {
            sample += c;
            arrayChar[(int)c]++;
        }
    }

    int retrievedDataStartY = 15;  //Start position ng retrieved data
    int retrievedDataEndY = retrievedDataStartY;

    //Print the retrieved data, line by line.
    stringstream ss(sample);
    string line;
    while (getline(ss, line)) {
        centerText(retrievedDataEndY++, line);  //Center each line of the retrieved data.
    }

    retrievedDataEndY += 2;
    setColor(7);
    centerText(retrievedDataEndY++, " ");

    Queue.insertQueue();
    Queue.renderMessage(file);

    cout << endl;
}
}

void showLoadingBar1(int duration) { //loading bar display
    int bar1 = 176, bar2 = 178;

    SetConsoleTextAttribute(console_color, 7);
    gotoxy(72, 23); cout << "Loading Program...";

    SetConsoleTextAttribute(console_color, 8);
    cout << "\n\n\t\t\t\t\t\t\t";

    for (int i = 0; i < 50; i++) { //ginagamit ang loop na ito para gumalaw ang loading bar display.
        cout << (char)bar2;
        Sleep(duration);
    }
    cout << endl;
}

void showLoadingBar3(int duration) { //loading bar display
    int bar1 = 176, bar2 = 178;

    SetConsoleTextAttribute(console_color, 7);
    gotoxy(70, 20); cout << "Decompressing File...";

    SetConsoleTextAttribute(console_color, 8);
    cout << "\n\n\t\t\t\t\t\t\t";

    for (int i = 0; i < 50; i++) { //ginagamit ang loop na ito para gumalaw ang loading bar display.
        cout << (char)bar2;
        Sleep(duration);
    }
    cout << endl;
}

void showLoadingBar4(int duration) { //loading bar display
    int bar1 = 176, bar2 = 178;

    SetConsoleTextAttribute(console_color, 7);
    gotoxy(65, 20); cout << "Rebuilding tree for decompression...";

    SetConsoleTextAttribute(console_color, 8);
    cout << "\n\n\t\t\t\t\t\t\t";

    for (int i = 0; i < 50; i++) { //ginagamit ang loop na ito para gumalaw ang loading bar display.
        cout << (char)bar2;
        Sleep(duration);
    }
    cout << endl;
}

void Title() { //welcome screen - logo/title
    system("cls");
    SetConsoleTextAttribute(console_color, 9);
    gotoxy(15, 8); cout << "                                              .         .                                                                                                                                                              .         .                                                                                    " << endl;
    gotoxy(15, 9); cout << "      ,o888888o.        ,o888888o.           ,8.       ,8.          8 888888888o   8 888888888o.   8 8888888888     d888888o.      d888888o.             8888888 8888888888 ,o888888o.                8 8888          ,8.       ,8.          8 888888888o   8 888888888o.   8 8888888888     d888888o.      d888888o. " << endl;
    gotoxy(15, 10); cout << "    8888     `88.   . 8888     `88.        ,888.     ,888.         8 8888    `88. 8 8888    `88.  8 8888         .`8888:' `88.  .`8888:' `88.                 8 8888    . 8888     `88.              8 8888         ,888.     ,888.         8 8888    `88. 8 8888    `88.  8 8888         .`8888:' `88.  .`8888:' `88. " << endl;
    gotoxy(15, 11); cout << " ,8 8888       `8. ,8 8888       `8b      .`8888.   .`8888.        8 8888     `88 8 8888     `88  8 8888         8.`8888.   Y8  8.`8888.   Y8                 8 8888   ,8 8888       `8b             8 8888        .`8888.   .`8888.        8 8888     `88 8 8888     `88  8 8888         8.`8888.   Y8  8.`8888.   Y8 " << endl;
    gotoxy(15, 12); cout << " 88 8888           88 8888        `8b    ,8.`8888. ,8.`8888.       8 8888     ,88 8 8888     ,88  8 8888         `8.`8888.      `8.`8888.                     8 8888   88 8888        `8b            8 8888       ,8.`8888. ,8.`8888.       8 8888     ,88 8 8888     ,88  8 8888         `8.`8888.      `8.`8888. " << endl;
    gotoxy(15, 13); cout << " 88 8888           88 8888         88   ,8'8.`8888,8^8.`8888.      8 8888.   ,88' 8 8888.   ,88'  8 888888888888  `8.`8888.      `8.`8888.                    8 8888   88 8888         88            8 8888      ,8'8.`8888,8^8.`8888.      8 8888.   ,88' 8 8888.   ,88'  8 888888888888  `8.`8888.      `8.`8888. " << endl;
    gotoxy(15, 14); cout << " 88 8888           88 8888         88  ,8' `8.`8888' `8.`8888.     8 888888888P'  8 888888888P'   8 8888           `8.`8888.      `8.`8888.                   8 8888   88 8888         88            8 8888     ,8' `8.`8888' `8.`8888.     8 888888888P'  8 888888888P'   8 8888           `8.`8888.      `8.`8888. " << endl;
    gotoxy(15, 15); cout << " 88 8888           88 8888        ,8P ,8'   `8.`88'   `8.`8888.    8 8888         8 8888`8b       8 8888            `8.`8888.      `8.`8888.                  8 8888   88 8888        ,8P            8 8888    ,8'   `8.`88'   `8.`8888.    8 8888         8 8888`8b       8 8888            `8.`8888.      `8.`8888. " << endl;
    gotoxy(15, 16); cout << " `8 8888       .8' `8 8888       ,8P ,8'     `8.`'     `8.`8888.   8 8888         8 8888 `8b.     8 8888        8b   `8.`8888. 8b   `8.`8888.                 8 8888   `8 8888       ,8P             8 8888   ,8'     `8.`'     `8.`8888.   8 8888         8 8888 `8b.     8 8888        8b   `8.`8888. 8b   `8.`8888. " << endl;
    gotoxy(15, 17); cout << "    8888     ,88'   ` 8888     ,88' ,8'       `8        `8.`8888.  8 8888         8 8888   `8b.   8 8888        `8b.  ;8.`8888 `8b.  ;8.`8888                 8 8888    ` 8888     ,88'              8 8888  ,8'       `8        `8.`8888.  8 8888         8 8888   `8b.   8 8888        `8b.  ;8.`8888 `8b.  ;8.`8888 " << endl;
    gotoxy(15, 18); cout << "     `8888888P'        `8888888P'  ,8'         `         `8.`8888. 8 8888         8 8888     `88. 8 888888888888 `Y8888P ,88P'  `Y8888P ,88P'                 8 8888       `8888888P'                8 8888 ,8'         `         `8.`8888. 8 8888         8 8888     `88. 8 888888888888 `Y8888P ,88P'  `Y8888P ,88P' " << endl;
    SetConsoleTextAttribute(console_color, 8);
}

void mainMenu() { //main menu/choices
    int choice;

    do {
        system("cls");
        setColor(7);
        gotoxy(65, 11); cout << "==========================================" << endl;
        gotoxy(76, 12); cout << "COMPRESS TO IMPRESS";
        gotoxy(65, 13); cout << "==========================================" << endl;

        setColor(10);
        gotoxy(65, 15); cout << "Welcome!";
        gotoxy(65, 16); cout << "Please select from the following options: ";
        gotoxy(77, 18); cout << "[1] Compress a File";
        gotoxy(77, 19); cout << "[2] Decompress a File";
        gotoxy(77, 20); cout << "[3] Exit";

        setColor(7);
        gotoxy(65, 22); cout << "-----------------------------------------" << endl;
        gotoxy(65, 23); cout << "Your choice: ";
        cin >> choice;

        if (choice == 1) {
            checkFile();  //Perform compression
        } else if (choice == 2) {
            PriorityQueue Queue;

            system("cls");
            showLoadingBar4(15);
            Queue.rebuildHuffmanTree();  //Rebuild the tree for decompression

            system("cls");
            showLoadingBar3(15);
            Queue.renderMessage();  //Perform decompression
        } else if (choice == 3) {
            system("cls");
            SetConsoleTextAttribute(console_color, 7);
            gotoxy(58, 11); cout << "========================================================" << endl;
            gotoxy(79, 12); cout << "Thank You !";
            gotoxy(58, 13); cout << "========================================================" << endl;
            gotoxy(59, 15); cout << "Thank you for using the system.";
            gotoxy(59, 16); cout << "Have a great day!";
            gotoxy(58, 18); cout << "========================================================" << endl;
            break;  //Exit the loop and terminate the program.
        } else {
            setColor(4);
            gotoxy(65, 26); cout << "Invalid choice! Please try again.";
            Sleep(1500);
        }
    } while (true);
}

int main() {
    Title();
    showLoadingBar1(15);
    mainMenu();
    return 0;
}
