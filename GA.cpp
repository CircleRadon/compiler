#include <bits/stdc++.h>
using namespace std;
#define cxmax 1000
#define tmax 500 
#define stacksize 500

fstream gainput;
fstream gaoutput;
string line;

struct node{
    string value;
    string key;
    int row;
    int col;
    node& operator=(node& n1){
        value = n1.value;
        key = n1.key;
        row = n1.row;
        col = n1.col;
    }
};
node all[1000];
node *word=all;
int cnt=0,flag=0;
int ga_error=0;


//���ű��е�����
enum object{
    constant, variable, procedure
};


struct tablestruct{
    string name;        //����
    enum object kind;   //���ͣ�constant, variable, procedure, array
    int val;            //��ֵ��const
    int level;          //Ƕ�ײ��
    int adr;            //��ַ
    int size;           //����Ŀռ䣬procedure
}; 
struct tablestruct table[tmax];  //���ű�
int ptx = 0;  //��ǰ���ű��β+1
int cx  = 0;  //�м��������


//���������
enum fct{
    LIT, OPR, LOD, STO, CAL, INT, JMP, JPC, RED, WRT
};

//���������ṹ
struct instruction{
    enum fct f;   //���������ָ��
    int l;        //���ò���������Ĳ�β�
    int a;        //���ݲ�ͬ��ָ����������
};  
struct instruction code[cxmax];  //�����


void readline();
void sendError(int type);
void enter(enum object k, int lev, int &pdx);
int position(string idt);
int gen(enum fct x, int y, int z);
int toint(string str);
void prog();
void block(int lev, int dxx, int txx);
void vardecl(int lev, int &dx);
void condecl(int lev, int &dx);
void Const(int lev, int &dx);
void proc(int lev, int &dx);
void body(int ctx, int lev);
void statement(int ctx, int lev);
void lexp(int ctx, int lev);
void exp(int ctx, int lev);
void term(int ctx, int lev);
void factor(int ctx, int lev);
int base(int l, int *S, int b);
void interpret();

//�ڷ��ű��м���һ��
//pdx ��ǰӦ����ı�������Ե�ַ ������1
void enter(enum object k, int lev, int &pdx)
{
    string nm = word->value;
    for (int i=0; i<ptx; i++){
        if (table[i].name == nm){
            sendError(26);  //�÷����Ѷ���
            return;
        }
    }
    table[ptx].name = nm;
    table[ptx].kind = k;
    switch(k)
    {
        case constant:  //����
            // if (num>amax)
            // {
            //     sendError();  //����Խ��
            // }
            table[ptx].level = lev;
            break;
        case variable:  //����
            table[ptx].level = lev;
            table[ptx].adr = pdx;
            pdx++;
            break;
        case procedure:  //����
            table[ptx].level = lev;
            break;
    }
    ptx++;
}

//���������ڷ��ű��е�λ�ã��Ҳ�������-1
int position(string idt)
{
    int i = ptx-1;
    while (i>=0&&table[i].name!=idt)
        i--;
    return i;
}

//�����м����
int gen(enum fct x, int y, int z)
{
    if (cx>=cxmax)
    {
        cout << "Program too long" << endl;
        return -1;
    }
    code[cx].f = x;
    code[cx].l = y;
    code[cx].a = z;
    cx++;
    return 0;
}

void readline(){
    if (getline(gainput,line)){
        while (line[0]=='#'||line.empty()){
            getline(gainput,line);
        }
    }
    istringstream iss(line);
    if (line[0]=='['){  //program error
        string s1;
        iss >> s1;
        iss >> s1;
        iss >> all[cnt].row;
       	iss >> all[cnt].col;
        iss >> all[cnt].key;  //UNKNOWWORD:
        iss >> all[cnt].value;
        // all[cnt++].key = "unknown";
    }
    else{
        char ch;
        iss >> ch;  //$
        iss >> all[cnt].key;
        iss >> all[cnt].value;
        iss >> all[cnt].row;
        iss >> all[cnt++].col;
    }
}

int toint(string str)
{
    int num = str.size();
    int ans = 0;
    for (int i=0; i<num; i++){
        ans = ans*10+str[i]-'0';
    }
    return ans;
}

void print_code(){
    string f[15]={"lit","opr","lod","sto","cal","int","jmp","jpc","red","wrt"};
    for (int i=0; i<cx; i++)
    {
        cout << "(" << i << ") " << f[code[i].f] << "  " << code[i].l << " " << code[i].a << endl;
    }
}

void sendError(int type){
    ga_error++;
    switch(type){
        case -1:
            cout << "[Grammer ERROR]  [" <<  0 << "," << 0 << "] " << "Missing keyword \"program\"" << endl;
            break;
        case 0:
            cout << "[Grammer ERROR]  [" <<  word->row << "," << word->col << "] " << "Spell error \"program\"" << endl;
            break;
        case 1:
            cout << "[Grammer ERROR]  [" <<  word->row << "," << word->col << "] " << "Invalid program ID" << endl;
            break;
        case 2:
            cout << "[Grammer ERROR]  [" <<  (word-1)->row << "," << (word-1)->col << "] " << "Missing ID" << endl;
            break;
        case 3:
            cout << "[Grammer ERROR]  [" <<  word->row << "," << word->col << "] " <<"Unknown word:  \"" << word->value << "\"" << endl;
            break;
        case 4:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Missing \";\"" << endl;
            break;
        case 5:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Missing keyword \"const\"" << endl;
            break;
        case 6:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Missing keyword \"procedure\"" << endl;
            break;
        case 7:
            cout << "[Grammar ERROR] "<< " [" << word->row << "," << word->col << "] " <<"Spell error \"const\"" << endl;
            break;
        case 8:
            cout << "[Grammar ERROR] "<< " [" << word->row << "," << word->col << "] " <<"Spell error \"procedure\"" << endl;
            break;
        case 9:
            cout << "[Grammar ERROR] "<< " [" << word->row << "," << word->col << "] " <<"Undeclared identifier" << endl;
            break;
        case 10:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Missing ID" << endl;
            break;
        case 11:
            cout << "[Grammar ERROR] "<< " [" << word->row << "," << word->col << "] " <<"Invalid ID" << endl;
            break;
        case 12:
            cout << "[Grammar ERROR] "<< " [" << word->row << "," << word->col << "] " <<"Missing \",\"" << endl;
            break;
        case 13:
            cout << "[Grammar ERROR] "<< " [" << word->row << "," << word->col << "] " <<"Error in \":=\"" << endl;
            break;
        case 14:
            cout << "[Grammer ERROR] "<< " [" <<  (word-1)->row << "," << (word-1)->col << "] " << "Missing \":=\" " << endl;
            break;
        case 15:
            cout << "[Grammer ERROR] "<< " [" <<  (word-1)->row << "," << (word-1)->col << "] " << "Missing \"(\" " << endl;
            break;
        case 16:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Missing \")\" " << endl;
            break;
        case 17:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Missing word \"begin\"" << endl;
            break;
        case 18:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Extra semicolon \";\" before \"end\"" << endl;
            break;
        case 19:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Missing matching \"end\"" << endl;
            break;
        case 20:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Missing word \"then\" " << endl;
            break;
        case 21:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Missing word \"do\" " << endl;
            break;
        case 22:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Missing the compare operator" << endl;
            break;
        case 23:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Wrong factor members" << endl;
            break;        
        case 24:
            cout << "[Grammar ERROR] "<< " [" << word->row << "," << word->col << "] " <<"unknown statement" << endl;
            break;    
        case 25:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Missing expression" << endl;
            break;  
        case 26:
            cout << "[Grammar ERROR] "<< " [" << (word-1)->row << "," << (word-1)->col << "] " <<"Redeclaration of " << word->value << endl;
            break;  
        case 27:
            cout << "[Semantic ERROR] "<< " [" << word->row << "," << word->col << "] " <<"Undefined " << word->value << endl;
            break;    
        case 28:
            cout << "[Semantic ERROR] "<< " [" << word->row << "," << word->col << "] " <<"Error type " << word->value << endl;
            break; 
        case 29:
            cout << "[Semantic ERROR] "<< " [" << word->row << "," << word->col << "] " <<"parameter mismatch" << endl;
            break; 
        case 30:
            cout  <<"division error" << endl;
            break; 
        default:
            cout << "[Grammer ERROR] [" <<  word->row << "," << word->col << "] " << "Unknown wrong" << endl;
            break;
    }
}

/*
<prog> -> program <id>; <block>
*/
void prog(){
    if ((word->value == "program" && word->key == "keyword")){
        word++;
    }
    else{
        if ((word+1)->value==";"){
            sendError(-1); //missing program
        }
        else{
            sendError(0); //spell error
        }
    }
    if (word->key == "ID"){
        word++;
    }
    else{
        if ((word+1)->value==";"){
            sendError(1); //invalid ID
            word++;
        }
        else{  //if (word->value==";"){
            sendError(2); //missing ID
        }
    }
    if ((word->value == ";"&&word->key=="EOP")){
        word++;
    }
    else{
        if ((word+1)->value==";"){
            sendError(3); //unknown word
            word+=2;
        }
        else{
            sendError(4); //missing ;
        }
        
    }
    block(0,3,0);
}

/*
<block> -> [<condecl>][<vardecl>][<proc>]<body>
    lev: ����
    dxx: ������βΣ���dxx��ʼ
    txx: ���̿�ʼ��table�е�λ��
*/
void block(int lev, int dxx, int txx){
    int dx=dxx;
    table[txx].adr = cx;
    int ctemp = cx;
    gen(JMP,0,0);

    // if ((word+1)->key=="keyword"){
    //     sendError(3);
    //     word++;
    // }
    if (word->value=="const"&&word->key=="keyword"){
        word++;
        condecl(lev,dx);
    }
    while (word->key!="keyword"){
        sendError(3);
        word++;
    }
    if (word->value=="var"&&word->key=="keyword"){
        word++;
        vardecl(lev,dx);
    }
    while (word->key!="keyword"){
        sendError(3);
        word++;
    }
    int cur_tx = ptx;

    if (word->value=="procedure"&&word->key=="keyword"){
        cur_tx = ptx+1;
        word++;
        proc(lev,dx);
    }
    while (word->key!="keyword"){
        sendError(3);
        word++;
    }

    //proc����
    ptx = cur_tx;

   // code[table[txx].adr].a = cx;  //���ɵ�ǰ���̵Ĵ���
    code[ctemp].a = cx;
    //cout << cx << endl;
    gen(INT,0,dx);   //�����ڴ�

    body(cur_tx,lev);
    table[txx].size = dx;
    gen(OPR,0,0);
}

/*
<vardecl> -> var <id>{,<id>};
*/
void vardecl(int lev, int &dx){
    if (word->key=="ID"){
        enter(variable,lev,dx);
        word++;
    }
    else{
        if (word->value==","||word->value==";"){
            sendError(10); //missing id
        }
        else{
            sendError(11);  //invalid id
            word++;
        }
    }
    int last=0;  //,|id
    while(word->value==","||word->key=="ID"){
        if (word->value==","&&last==0){  //,
            word++;
            last = 1;
        }
        else if (word->key=="ID"&&last==1){
            enter(variable,lev,dx);
            word++;
            last = 0;
        }
        else if (last==0){
            sendError(12); //missing ,
            last = 1;
        }
        else if (last==1){
            sendError(10);  //missing id
            last = 0;
        }
    }
    if (word->value==";"&&last==0){
        word++;
    }
    else if (word->value==";"&&last==1){
        sendError(10);  //missing id
        word++;
    }
    else{
        sendError(4); //missing ;
    }
}

/*
<condecl> -> const <const>{,<const>};
*/
void condecl(int lev, int &dx){
    if (word->key=="ID"){
        enter(constant,lev,dx);
        word++;
        Const(lev,dx);
    }
    else{
        if ((word+1)->value==":="){
            sendError(11);  //invalid id
            word++;
            Const(lev,dx);
        }
        else{
            sendError(10); //missing id
            Const(lev,dx);
        }
    }
    while(word->value==","&&word->key=="SOP"){
        word++;
        if (word->key=="ID"){
            enter(constant,lev,dx);
            word++;
            Const(lev,dx);
        }
        else{
             if ((word+1)->value==":="){
                sendError(11);  //invalid id
                word++;
                Const(lev,dx);
            }
            else{
                sendError(10); //missing id
                Const(lev,dx);
            }
        }
    }


    if (word->value==";"&&word->key=="EOP"){
        word++;
    }
    else{
        if ((word+1)->value==";"){
            sendError(3);  //unknown word
            word+=2;
        }
        else{
            sendError(4);  //missing ;
        }
    }
}

/*
<const> -> <id>:=<integer>
<id> -> l{l|d}
<integer> -> d{d}
*/
void Const(int lev, int &dx){
    if (word->value==":="&&word->key=="assignment"){
        word++;
    }
    else{
        if (word->key=="INT"){
            sendError(14); //missing :=
        }
        if (word->value=="="||word->value==":"){
            sendError(13); //error in :=
            word++;
        }
    }
    if (word->key=="INT"){
        table[ptx-1].val = toint(word->value);
        word++;
    }
    else{
        sendError(15);  //missing int
    }
}


/*
<proc> -> procedure <id> ([<id>{,<id>}]) ;<block>{;<proc>}
*/
void proc(int lev, int &dx){
    int tx0;   //������ų�ʼλ��
    int dxx = 3;
    tx0 = ptx;

    if (word->key=="ID"){
        enter(procedure,lev,dx);
        word++;
    }
    else{
        if ((word+1)->value=="("){
            sendError(11);  //invalid id
            word++;
        }
        else{
            sendError(10);  //missing id
        }
    }

    if (word->value=="("&&word->key=="SOP"){
        word++;
    }
    else{
        if (word->key=="ID"||word->value==";"){
            sendError(15); //missing (
        }
        else{
            sendError(3);  //unknown word
        }
    }
    if (word->key=="ID"){
        enter(variable,lev+1,dxx);
        word++;
    }
    else{
        if (word->value==")"){
        }
        else if (word->value==";"){
            sendError(10);  //missing id
        }
        else if ((word+1)->value==","||(word+1)->value==")"||(word+1)->value==";"){
            sendError(11);  //invalid id
            word++;
        }
        else{
            sendError(10);  //missing id
        }
    }
    while(word->value==","&&word->key=="SOP"){
        word++;
        if (word->key=="ID"){
            enter(variable,lev+1,dxx);
            word++;
        }
        else{
            sendError(10);  //missing id
        }
    }
    while (word->key=="ID"){
        sendError(12); //missing ,
        word++;
    }
    table[tx0].val = dxx-3;   //�βεĸ���
    if (word->value==")"&&word->key=="SOP"){
        word++;
    }
    else{
        if ((word+1)->value==")"){
            sendError(3);  //unknown word
            word++;
        }
        else{
            sendError(16);  //missing )
        }
    }
    if (word->value==";"&&word->key=="EOP"){
        word++;
    }
    else{
        sendError(4);
    }
    block(lev+1,dxx,tx0);

    while(word->value==";"&&word->key=="EOP"){
        word++;
        if (word->value=="procedure"&&word->key=="keyword"){
            word++;
            proc(lev,dx);
        }
        else {
            if ((word+1)->value=="("){
                sendError(6);  //missing procedure
                proc(lev,dx);
            }
            else if ((word+2)->value=="("){
                sendError(8);  //procedure spell error
                word++;
                proc(lev,dx);
            }
            else{
                sendError(3); //unknown word
                word++;
            }
        }
    }
       
}

/*
<body> -> begin <statement>{;<statement>}end
ctx: ��ʼ��tx
*/
void body(int ctx, int lev){
    if (word->value=="begin"&&word->key=="keyword"){
        word++;
        node *end=all+cnt,*temp=word;
        int flag = 0;
        while (temp<end){
            if (temp->value=="end"){
                flag = 1;
                break;
            }
            temp++;
        }
        if (flag==0){  

            sendError(19); //Missing matching end
        }
    }
    else{
        sendError(17); //missing begin
    }
    statement(ctx,lev);

    while (word->value!="end"&&word<all+cnt){
        if (word->value==";"){
            word++;
        }
        else{
            sendError(4); //missing ;
        }
        if (word->value=="end"){
            sendError(18);
            break;
        }
        statement(ctx,lev);
    }
    // while (word->value==";"&&word->key=="EOP" ){
    //     word++;
    //     if (word->value=="end"){
    //         sendError(18);
    //         break;
    //     }
    //     statement();
    // }
    // int wrong_row=0;
    // while (flag==1&&word->value!="end"){
    //     if (wrong_row!=word->row){
    //         wrong_row=word->row;
    //         sendError(24);  //unknown statement
    //     }
    //     word++;
    // }
    if (word->value=="end"&&word->key=="keyword"){
        word++;
    }
    // else{
    //     sendError(19); //missing end
    // }
}

/*
<statement> -> <id> := <exp>
               |if <lexp> then <statement>[else <statement>]
               |while <lexp> do <statement>
               |call <id> ([<exp>{,<exp>}])
               |<body>
               |read (<id>{,<exp>})
               |write (<exp>{,<exp>})
*/
void statement(int ctx, int lev){
    //<id> := <exp>
    if (word->key=="ID"){
        int i = position(word->value);
        if (i==-1)
            sendError(27);  //δ����
        else if (table[i].kind!=variable)
            sendError(28);  //���ʹ���
        word++;
        if (word->value==":="&&word->key=="assignment"){
            word++;
            exp(ctx,lev);
            gen(STO,lev-table[i].level,table[i].adr);
        }
        else{           
            if (word->value=="="){
                sendError(13);
                word++;
                exp(ctx,lev);
            }
            // else if (word->key=="ID"){
            //     sendError(14);
            //     exp();
            // }
            else{
                sendError(14); //missing:=
                while (!(word->value==";"||word->value=="end")){
                    word++;
                }   
            }
        }
    }

    //if <lexp> then <statement>[else <statement>]
    else if (word->value=="if"&&word->key=="keyword"){
        word++;
        lexp(ctx,lev);
        if (word->value=="then"&&word->key=="keyword"){
            word++;
        }
        else{
            sendError(20);  //missing then
            // while (!(word->value==";"||word->value=="end")){
            //     word++;
            // }   
        }
        int cx1 = cx;  //jpc��ָ��
        gen(JPC,0,0);
        statement(ctx,lev);
        int cx2 = cx;  //jmp��ָ��
        gen(JMP,0,0);
        code[cx1].a = cx;
        if (word->value=="else"&&word->key=="keyword"){
            word++;
            statement(ctx,lev);
        }
        code[cx2].a = cx;
    }

    //while <lexp> do <statement>
    else if (word->value=="while"&&word->key=="keyword"){
        word++;
        int cx1 = cx;
        lexp(ctx,lev);
        int cx2 = cx;
        gen(JPC,0,0);
        if (word->value=="do"&&word->key=="keyword"){
            word++;
        }
        else{
            sendError(21);  //missing do
            // while (!(word->value==";"||word->value=="end")){
            //     word++;
            // }   
        }
        statement(ctx,lev);
        gen(JMP,0,cx1);     //����������while
        code[cx2].a = cx;   //���������� ����ѭ��
    }
    //call <id> ([<exp>{,<exp>}])
    else if (word->value=="call"&&word->key=="keyword"){
        int i=-1;
        word++;
        if (word->key=="ID"){
            i = position(word->value);
            if (i==-1)
                sendError(27);   //δ����
            else if (table[i].kind!=procedure)
                sendError(28);   //���Ͳ�ƥ��
            word++;  
        }
        else{
            if ((word+1)->value=="("){
                sendError(11); //invalid id
                word++;
            }
            else{
                sendError(10);  //missing id
            }
        }      
        if (word->value=="("&&word->key=="SOP"){
            word++;    
        }
        else{
            if ((word+1)->value=="("){
                sendError(3); //invalid word
                word++;
            }
            else{
                sendError(15); //missing (
            }
        } 
        int cx_temp;
        if (i!=-1)
            cx_temp = table[i].val;  //��������
        int last=1;  //,|id
        while(word->value==","||word->key=="AOP"||word->key=="ID"||word->key=="INT"||word->value=="("){
            if (word->value==","&&last==0){  //,
                word++;
                last = 1;
            }
            else if (word->value!=","&&last==1){ //exp
                exp(ctx,lev);
                gen(OPR,0,7);   //���β�����ջ
                cx_temp--;
                last = 0;
            }
            else if (last==0){
                sendError(12); //missing ,
                last = 1;
            }
            else if (last==1){
                sendError(25);  //missing exp
                last = 0;
            }
        }
        if (cx_temp!=0)
            sendError(29);   //������ƥ��
        if (i!=-1)
            gen(CAL,lev-table[i].level,table[i].adr);
        if (word->value==")"&&(last==0||last==1&&(word-1)->value=="(")){
            word++;
        }
        else if (word->value==")"&&last==1){
            sendError(25);  //missing exp
            word++;
        }
        else{
            sendError(4); //missing )
        }
    }

    //read (<id>{,<id>})
    else if (word->value=="read"&&word->key=="keyword"){
        word++;
        if (word->value=="("&&word->key=="SOP"){
            word++;
        }
        else{
            sendError(15); //missing (
        }
        if (word->key=="ID"){
            int i = position(word->value);
            if (i==-1)
                sendError(27);
            else if(table[i].kind!=variable)
                sendError(28);
            gen(RED,lev-table[i].level,table[i].adr);
            word++;
        }
        else{
            sendError(10);
        }
        int last = 0;
        while (word->value==","||word->key=="ID"){
            if (word->value==","&&last==0){
                last = 1;
                word++;
            }
            else if (word->key=="ID"&&last==1){
                last = 0;
                int i = position(word->value);
                if (i==-1)
                    sendError(27);
                else if(table[i].kind!=variable)
                    sendError(28);
                gen(RED,lev-table[i].level,table[i].adr);
                word++;
            }
            else if(last==1){
                sendError(10);  //missing id
            }
            else if(last==0){
                sendError(12);  //missing ,
            }
        }
        if (word->value==")"&&last==0){
            word++;
        }
        else if (word->value==")"){
            sendError(10);  //missing id
            word++;
        }
        else{
            sendError(16);  //missing )
            while (!(word->value==";"||word->value=="end")){
                word++;
            }   
        }
    }
    //write (<exp>{,<exp>})
    else if (word->value=="write"&&word->key=="keyword"){
        word++;
        if (word->value=="("&&word->key=="SOP"){
            word++;
        }
        else{
            sendError(15); //missing (
        }
        exp(ctx,lev);
        gen(WRT,0,0);  //���ֵ
        gen(OPR,0,15); //����
        int last=0;  //,|exp
        while(word->value==","||word->key=="AOP"||word->key=="ID"||word->key=="INT"||word->value=="("){
            if (word->value==","&&last==0){  //,
                word++;
                last = 1;
            }
            else if (word->value!=","&&last==1){ //exp
                exp(ctx,lev);
                gen(WRT,0,0);  //���ֵ
                gen(OPR,0,15); //����
                last = 0;
            }
            else if (last==0){
                sendError(12); //missing ,
                last = 1;
            }
            else if (last==1){
                sendError(25);  //missing exp
                last = 0;
            }
        }
        if (word->value==")"&&last==0){
            word++;
        }
        else if (word->value==")"&&last==1){
            sendError(25);  //missing exp
            word++;
        }
        else{
            sendError(16); //missing )
            while (!(word->value==";"||word->value=="end")){
                word++;
            }   
        }
    }
      
    else if (word->value=="begin"){
        body(ctx,lev);
    }
    else{
        sendError(24); //unknown statement
        while (word->value!=";"){
            word++;
        }
    }
}


/*
<lexp> -> <exp> <lop> <exp>|odd <exp>
*/
void lexp(int ctx, int lev){
    string temp;
    if (word->value=="odd"){
        word++;
        exp(ctx,lev);
        gen(OPR,0,6);
    }
    else{
        exp(ctx,lev);
        if (word->key=="LOP"){
            temp = word->value;
            word++;
            exp(ctx,lev);
        }
        else{
            sendError(22); //Missing the compare operator
        }
        if (temp=="=")
            gen(OPR,0,8);
        else if (temp=="<>")
            gen(OPR,0,9);
        else if (temp=="<")
            gen(OPR,0,10);
        else if (temp==">=")
            gen(OPR,0,11);
        else if (temp==">")
            gen(OPR,0,12);
        else if (temp=="<=")
            gen(OPR,0,13);
    }
}

/*
<exp> -> [+|-]<term>{<aop><term>}
*/
void exp(int ctx, int lev){
	int temp = 0;
    if (word->key=="AOP"){  //+|-
        if (word->value =="-")
            temp = 1;
        word++;
    }
    term(ctx,lev);
    if (temp==1)
        gen(OPR,0,1);   //ջ��ȡ��
    while (word->key=="AOP"){
        if (word->value=="-")
            temp = 1;
        else
            temp = 0;
        word++;
        term(ctx,lev);
        if (temp==1)
            gen(OPR,0,3);
        else
            gen(OPR,0,2); 
    }
}

/*
<term> -> <factor>{<mop><factor>}
*/
void term(int ctx, int lev){
    string temp;
    factor(ctx,lev);
    if(word->key=="MOP"){
        temp = word->value;
        word++;
        factor(ctx,lev);
        if (temp=="*")
            gen(OPR,0,4);
        else //if (temp=="/")
            gen(OPR,0,5);
    }
}

/*
<factor> -> <id>|<integer>|(<exp>)
*/
void factor(int ctx, int lev){
    if (word->key=="ID"){
        int i = position(word->value);
        if (i==-1)
            sendError(27);
        else if (table[i].kind==procedure)
            sendError(28);
        else if (table[i].kind==constant)
            gen(LIT,0,table[i].val);
        else if (table[i].kind==variable)
            gen(LOD,lev-table[i].level,table[i].adr);
        word++;
    }
    else if (word->key=="INT"){
        gen(LIT,0,toint(word->value));
        word++;
    }
    else if (word->key=="("&&word->value=="SOP"){
        word++;
        exp(ctx,lev);
        if (word->key==")"&&word->value=="SOP"){
            word++;
        }
        else{
            sendError(16); //missing )
        }
    }
    else{
        sendError(23); //Wrong factor members
    }
}

void openfile(){
    gainput.open("la_out.txt",ios::in);
    gaoutput.open("ga_output.txt",ios::out | ios::trunc);
    if (!gainput.is_open()){
        cout << "Can't open the la_out.txt!" << endl;
        exit(0); 
    }
    if (!gaoutput.is_open()){
        cout << "Can't open the ga_out.txt!" << endl;
        exit(0); 
    }
}

/////////////// ���� /////////////// 

//l����� b���˴ι��̻�ַ
int base(int l, int *S, int b)
{
    int i=b;
    while(l>0)
    {
        i = S[i];
        l--;
    }
    return i;
}

//���ͳ���
void interpret(){
    int S[stacksize];
    int p,b,t;  // ָ��ָ�룬ָ���ַ��ջ��ָ��
    int i;
    int temp=3;
    p=0,b=0,t=0;
    S[0] = S[1] = S[2] = 0;
    cout << "********************* start pl0 *********************" << endl;
    do{
        i = p;
        p++;
        switch(code[i].f)
        {
            case LIT:   //ȡ����a��������ջջ��
                S[t] = code[i].a;
                t++;
                break;
            case OPR:   //����
                switch(code[i].a)
                {
                    case 0:   //���ص��õ㲢��ջ
                        t = b;
                        p = S[b+2];  //RA ���ص�ַ
                        b = S[b+1];  //DL ��̬�� ǰ��Ļ�ַ
                        break;
                    case 1:   //ջ��ȡ��
                        S[t-1] = -S[t-1]; 
                        break;
                    case 2:   //��ջ����ջ�����
                        t--;
                        S[t-1] = S[t-1]+S[t];
                        break;
                    case 3:   //��ջ����ջ��
                        t--;
                        S[t-1] = S[t-1]-S[t];
                        break;
                    case 4:   //��ջ����ջ��
                        t--;
                        S[t-1] = S[t-1]*S[t];
                        break;
                    case 5:   //��ջ����ջ��
                        t--;
                        if (S[t]==0)
                        {
                            sendError(30);  //����Ϊ0
                            return;
                        }
                        S[t-1] = S[t-1]/S[t];
                        break;
                    case 6:   //ջ��Ԫ�ص���ż���ж�
                        S[t-1] = S[t-1]%2;
                        break;
                    case 7:
                        if (t+temp>=stacksize)
                        {
                            return;
                        }
                        t--;
                        S[t+temp]=S[t];
                        temp++;
                        break;
                    case 8:   //�Ƿ����
                        t--;
                        S[t-1] = (S[t-1]==S[t]);
                        break;
                    case 9:   //�Ƿ񲻵�
                        t--;
                        S[t-1] = (S[t-1]!=S[t]);
                        break;
                    case 10:   //С���ж�
                        t--;
                        S[t-1] = (S[t-1]<S[t]);
                        break;
                    case 11:   //���ڵ����ж�
                        t--;
                        S[t-1] = (S[t-1]>=S[t]);
                        break;
                    case 12:   //�����ж�   
                        t--;
                        S[t-1] = (S[t-1]>S[t]);
                        break;       
                    case 13:   //С�ڵ����ж�
                        t--;
                        S[t-1] = (S[t-1]<=S[t]);
                        break;      
                    case 14:   //ջ��ֵ���
                        t--;
                        cout << S[t];
                        break;
                    case 15:   //��Ļ�������
                        cout << endl;
                        break;
                    case 16:   //�������ж���һ���������뵽ջ��
                        //cout << "��������ֵ��";
                        cin >> S[t];
                        t++;
                        break;
                }
                break;

            case LOD:   //ȡ��������Ե�ַΪa�����ΪL���ŵ�����ջ��ջ��
                S[t] = S[base(code[i].l,S,b)+code[i].a];
                t++;
                break;
            case STO:   //������ջջ�������ݴ������
                t--;
                S[base(code[i].l,S,b)+code[i].a] = S[t];
                break;
            case CAL:   //���ù��̣���ڵ�ַΪa����β�ΪL��
                S[t] = base(code[i].l,S,b);  //SL �����̻�ַ��ջ
                S[t+1] = b;  //DL ���ػ�ַ�������ߵĻ�ַ��
                S[t+2] = p;  //RA
                b = t;
                p = code[i].a;
                temp = 3;
                break;

            case INT:   //����ջջ��ָ������a
                t+=code[i].a;
                break;
            case JMP:   //������ת�Ƶ���ַΪa��ָ��
                p = code[i].a;
                break;
            case JPC:   //����ת��ָ�ת�Ƶ���ַΪa��ָ��
                t--;
                if (S[t]==0)
                    p = code[i].a;
                break;
            case RED:   //�����ݲ������������Ե�ַΪa����β�ΪL��
                //cout << "��������ֵ��";
                int temp;
                cin >> temp;
                S[base(code[i].l,S,b)+code[i].a] = temp;
               // *(S.base+B+2+code[i].a) = temp; ��ô��
                break;
            case WRT:   //��ջ���������
                t--;
                cout << S[t];
                break;
        }
    }while(p!=0);
}

void closefile(){
    gainput.close();
    gaoutput.close();
}

void GA(){
    openfile();

    //header
    time_t curTime;
    struct tm *timeinfo;
    time(&curTime);
    timeinfo = localtime(&curTime);

    gaoutput << "# Grammer Analysis Result #" << endl;
    gaoutput << "Generate Time: " << asctime(timeinfo);
    gaoutput << "Language Set: PL/0" << endl;
    gaoutput << "***************************************************" << endl; 
    gaoutput << endl;

	cout << "********************* Grammer Analysis *********************" << endl;
    while(gainput.peek()!=EOF){
        readline();
    }
    
    prog();
    
    if (ga_error==0){
        cout << "0error!" << endl;
        cout << "successfully compiled..." << endl;
        cout << "********************** Ŀ �� �� �� **********************" << endl;
        print_code();
        interpret();
    }
    closefile();
}



