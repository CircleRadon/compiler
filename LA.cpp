#include <bits/stdc++.h>
using namespace std;

const string KeyWord[15] = {"program", "const", "var", "procedure", "begin", "end", "if",
                            "then", "else", "while", "do", "call", "read", "write", "odd"};

fstream input;
fstream output;

int row, col;
int col_error;
int la_error=0;

bool isDigit(char ch) //�Ƿ�Ϊ����
{
    if (ch >= '0' && ch <= '9')
        return true;
    else
        return false;
}

bool isLetter(char ch) //�Ƿ�Ϊ��ĸ
{
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
        return true;
    else
        return false;
}

bool isKeyWord(string s) //�Ƿ�Ϊ�ؼ���
{
    for (int i = 0; i < 15; i++)
    {
        if (s.compare(KeyWord[i]) == 0)
            return true;
    }
    return false;
}

bool isBC(char ch) //�Ƿ��ǿհ��ַ�
{
    if (ch == ' ')
    {
        col++;
        return 1;
    }
    else if (ch == '\t')
    { //tab
        col += 4;
        return 1;
    }
    else if (ch == '\n')
    {
        row++;
        col = 1;
        return 1;
    }
    else
        return 0;
}

string ConCat(string str, char ch) //��ch�����ݼ���str
{
    str += ch;
    return str;
}

void Retract()
{
    if (!input.eof())
    {
        input.seekg(-1, ios::cur);
    }
}

bool ischar(char ch)
{
    if (ch == '\n' || ch == '\t' || ch == ' ' || ch == '<' || ch == '>' || ch == '=' ||
        ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == ':'||ch == ';'||ch == ',')
        return true;
    else
        return false;
}

void LA()
{
    string inputFileName;
    cout << "Please input the file name:";
    cin >> inputFileName;
    //inputFileName = "1.txt";
    cout << endl;
    cout << "********************* Lexical Analysis *********************" << endl;
    //���ļ�
    input.open(inputFileName, ios::in);
    output.open("la_out.txt", ios::out | ios::trunc);

    if (!input.is_open())
    {
        cout << "Can't open the input file!" << endl;
        exit(0);
    }
    if (!output.is_open())
    {
        cout << "Can't open the output file!" << endl;
        exit(0);
    }

    //header
    time_t curTime;
    struct tm *timeinfo;
    time(&curTime);
    timeinfo = localtime(&curTime);
    output << "#Generate Time: " << asctime(timeinfo);
    output << "#input File Name: " << inputFileName << endl;
    output << "#Language Set: PL/0" << endl;
    output << endl;

    string strToken;
    char ch;
    row = 1;
    col = 1;
    while (!input.eof())
    {
        ch = input.get();
        if (isBC(ch))
            strToken = "";
        else if (isLetter(ch))
        {
            while (isLetter(ch) || isDigit(ch))
            {
                strToken = ConCat(strToken, ch);
                col++;
                ch = input.get();
            }
            if (isKeyWord(strToken))
            {
                output << "$keyword  " << strToken << " " << row << " " << col << endl;
            }
            else
            {
                output << "$ID  " << strToken << " " << row << " " << col << endl;
            }
            strToken = "";
            Retract();
        }
        else if (isDigit(ch))
        {
        	col_error = col;
            while (isDigit(ch))
            {
                strToken = ConCat(strToken, ch);
                col++;
                ch = input.get();
            }
            if (isLetter(ch))
            {
                output << "[Lexical ERROR] " <<  row << " " << col_error  << "InvalidINT ";
                cout << "[Lexical ERROR] " <<  " [" << row << "," << col_error  << "] " << "Invalid ID: ";
                la_error++;
                while (isLetter(ch) || isDigit(ch))
                {
                    strToken = ConCat(strToken, ch);
                    col++;
                    ch = input.get();
                }
                output << "\"" << strToken << "\"" << endl;
                cout << "\"" << strToken << "\"" << endl;
            }
            else
            {
                output << "$INT  " << strToken << " " << row << " " << col << endl;
            }
            Retract();
            strToken = "";
        }
        else if (ch == '=')
        {
            col++;
            output << "$LOP  " << ch << " " << row << " " << col << endl;
        }
        else if (ch == '<')
        {
            col++;
            ch = input.get();
            if (ch == '>')
            {
                col++;
                output << "$LOP  <>" << " " << row << " " << col << endl;
            }
            else if (ch == '=')
            {
                col++;
                output << "$LOP  <=" << " " << row << " " << col << endl;
            }
            else
            {
                output << "$LOP  <" << " " << row << " " << col << endl;
                Retract();
            }
        }
        else if (ch == '>')
        {
            col++;
            ch = input.get();
            if (ch == '=')
            {
                col++;
                output << "$LOP  >=" << " " << row << " " << col << endl;
            }
            else
            {
                output << "$LOP  >" << " " << row << " " << col << endl;
                Retract();
            }
        }
        else if (ch == ':')
        {
            col++;
            ch = input.get();
            if (ch == '=')
            {
                col++;
                output << "$assignment  :=" << " " << row << " " << col << endl;
            }
            else
            {
                // output << "[Program ERROR] " << "[" << row << "," << col << "]" << "Missing '=' behind the ':'" << endl;
                output << "[Lexical ERROR]  " << row << " " << col << "InvalidWord: \":\"" << endl;
                cout << "[Lexical ERROR]  " << "[" << row << " " << col << "]" << "InvalidWord: \":\""  << endl;
                la_error++;
                Retract();
            }
        }
        else if (ch == '+' || ch == '-')
        {
            col++;
            output << "$AOP  " << ch << " " << row << " " << col << endl;
        }
        else if (ch == '*')
        {
            col++;
            ch = input.get();
            if (ch=='*'){
                output << "$POW  **" << " " << row << " " << col << endl;
            }
            else{
                Retract();
                output << "$MOP  " << ch << " " << row << " " << col << endl;
            }
        }
        else if (ch == '/')
        {
            col++;
            output << "$MOP  " << ch << " " << row << " " << col << endl;
        }
        else if (ch == ';')
        {
            col++;
            output << "$EOP  " << ch << " " << row << " " << col << endl;
        }
        else if (ch == '(' || ch == ')' || ch == ',')
        {
            col++;
            output << "$SOP  " << ch << " " << row << " " << col << endl;
        }
        else
        {
        	col_error = col;
            while (!ischar(ch)&&!isLetter(ch)&&!input.eof())
            {
            	col++;
            	if (ch<0){ //�������ֽ� 
            		strToken = ConCat(strToken, ch);
            		ch = input.get();
            		if (ch<0||ch>63)
            			strToken = ConCat(strToken, ch);
            		else{
            			Retract();
					}
            	}
            	else{
            		strToken = ConCat(strToken, ch);
            	}
                ch = input.get();
            }
            output << "[Lexical ERROR] " << row << " " << col_error  << " UNKNOWNWORD�� " << strToken << endl;
            cout << "[Lexical ERROR]  " << "[" <<row << "," << col_error  << "]" << " UNKNOWNWORD�� " << strToken << endl;
            strToken="";
            la_error++;
            Retract();
        }
    }

    input.close();
    output.close();
    if (la_error==0){
        cout << "0error!" << endl;
    }
    cout << endl;
//    cout << "Lexical Analysis is finished!" << endl;
//    cout << "Output file is generated successfully!" << endl;

}

