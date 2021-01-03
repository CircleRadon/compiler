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


//符号表中的类型
enum object{
    constant, variable, procedure
};


struct tablestruct{
    string name;        //名字
    enum object kind;   //类型：constant, variable, procedure, array
    int val;            //数值，const
    int level;          //嵌套层次
    int adr;            //地址
    int size;           //分配的空间，procedure
}; 
struct tablestruct table[tmax];  //符号表
int ptx = 0;  //当前符号表表尾+1
int cx  = 0;  //中间代码数量


//虚拟机代码
enum fct{
    LIT, OPR, LOD, STO, CAL, INT, JMP, JPC, RED, WRT
};

//虚拟机代码结构
struct instruction{
    enum fct f;   //虚拟机代码指令
    int l;        //引用层与声明层的层次差
    int a;        //根据不同的指令有所区别
};  
struct instruction code[cxmax];  //代码表


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
void body(int lev);
void statement(int lev);
void lexp(int lev);
void exp(int lev);
void term(int lev);
void factor(int lev);
int base(int l, int *S, int b);
void interpret();
void printtable();

//在符号表中加入一项
//pdx 当前应分配的变量的相对地址 分配后加1
void enter(enum object k, int lev, int &pdx)
{
    string nm = word->value;

    table[ptx].name = nm;
    table[ptx].kind = k;
    switch(k)
    {
        case constant:  //常量
            table[ptx].level = lev;
            break;
        case variable:  //变量
            table[ptx].level = lev;
            table[ptx].adr = pdx;
            pdx++;
            break;
        case procedure:  //过程
            table[ptx].level = lev;
            break;
    }
    ptx++;
}

//查找名字在符号表中的位置，找不到返回-1
int position(string idt)
{
    int i = ptx-1;
    int lnow;
    if (i>=0)
        lnow = table[i].level;
    while (i>=0){
        if (table[i].level<=lnow&&table[i].name==idt)
            return i;
        if (table[i].level<lnow)
            lnow = table[i].level;
        i--;
    }
    return -1;
}

//在当前层找
int find_in_cur(string idt)
{
    int i = ptx-1;
    int lnow = table[i].level;
    i--;
    while (i>=0&&table[i].level==lnow)
    {
        if (table[i].name==idt)
            return i;
        i--;
    }
    return -1;
}

//生成中间代码
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
        case 31:
            cout  <<"[Semantic ERROR] "<< " [" << word->row << "," << word->col << "] " <<"Reassignment" << endl;
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
    block(1,3,0);
}

/*
<block> -> [<condecl>][<vardecl>][<proc>]<body>
    lev: 层数
    dxx: 如果有形参，从dxx开始
    txx: 过程开始在table中的位置
*/
void block(int lev, int dxx, int txx){
    int dx=dxx;
    table[txx].adr = cx;
    int ctemp = cx;
    gen(JMP,0,0);

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

    if (word->value=="procedure"&&word->key=="keyword"){
        word++;
        proc(lev,dx);
    }
    while (word->key!="keyword"){
        sendError(3);
        word++;
    }

   // code[table[txx].adr].a = cx;  //生成当前过程的代码
    code[ctemp].a = cx;
    //cout << cx << endl;
    gen(INT,0,dx);   //分配内存

    body(lev);
    table[txx].size = dx;
    gen(OPR,0,0);
}

/*
<vardecl> -> var <id>{,<id>};
*/
void vardecl(int lev, int &dx){
    if (word->key=="ID"){
        int i = find_in_cur(word->value);
        if (i==-1)
            enter(variable,lev,dx);
        else{
            sendError(26);
        }
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
            int i = find_in_cur(word->value);
            if (i==-1)
                enter(variable,lev,dx);
            else{
                sendError(26);
            }
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
    int tx0;   //本层符号初始位置
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
    table[tx0].val = dxx-3;   //形参的个数
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
*/
void body(int lev){
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
    statement(lev);

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
        statement(lev);
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
void statement(int lev){
    //<id> := <exp>
    if (word->key=="ID"){
        int i = position(word->value);
        if (i==-1)
            sendError(27);  //未定义
        else if (table[i].kind==constant)
            sendError(31);
        else if (table[i].kind!=variable)
            sendError(28);  //类型错误
        word++;
        if (word->value==":="&&word->key=="assignment"){
            word++;
            exp(lev);
            gen(STO,lev-table[i].level,table[i].adr);
        }
        else{           
            if (word->value=="="){
                sendError(13);
                word++;
                exp(lev);
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
        lexp(lev);
        if (word->value=="then"&&word->key=="keyword"){
            word++;
        }
        else{
            sendError(20);  //missing then
            // while (!(word->value==";"||word->value=="end")){
            //     word++;
            // }   
        }
        int cx1 = cx;  //jpc的指令
        gen(JPC,0,0);
        statement(lev);
        int cx2 = cx;  //jmp的指令
        gen(JMP,0,0);
        code[cx1].a = cx;
        if (word->value=="else"&&word->key=="keyword"){
            word++;
            statement(lev);
        }
        code[cx2].a = cx;
    }

    //while <lexp> do <statement>
    else if (word->value=="while"&&word->key=="keyword"){
        word++;
        int cx1 = cx;
        lexp(lev);
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
        statement(lev);
        gen(JMP,0,cx1);     //无条件跳回while
        code[cx2].a = cx;   //条件不满足 跳出循环
    }
    //call <id> ([<exp>{,<exp>}])
    else if (word->value=="call"&&word->key=="keyword"){
        int i=-1;
        word++;
        if (word->key=="ID"){
            i = position(word->value);
            if (i==-1)
                sendError(27);   //未定义
            else if (table[i].kind!=procedure)
                sendError(28);   //类型不匹配
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
        int cx_temp,cx_temp1;
        if (i!=-1){
            cx_temp = table[i].val;  //参数个数
            cx_temp1 = cx_temp;
        }
        int last=1;  //,|id
        while(word->value==","||word->key=="AOP"||word->key=="ID"||word->key=="INT"||word->value=="("){
            if (word->value==","&&last==0){  //,
                word++;
                last = 1;
            }
            else if (word->value!=","&&last==1){ //exp
                exp(lev);
                //gen(OPR,0,7);   //把形参送入栈
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
            sendError(29);   //参数不匹配
        if (i!=-1){
            gen(OPR,1,cx_temp1);
            gen(CAL,lev-table[i].level,table[i].adr);
        }
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
        exp(lev);
        gen(WRT,0,0);  //输出值
        gen(OPR,0,15); //换行
        int last=0;  //,|exp
        while(word->value==","||word->key=="AOP"||word->key=="ID"||word->key=="INT"||word->value=="("){
            if (word->value==","&&last==0){  //,
                word++;
                last = 1;
            }
            else if (word->value!=","&&last==1){ //exp
                exp(lev);
                gen(WRT,0,0);  //输出值
                gen(OPR,0,15); //换行
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
        body(lev);
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
void lexp(int lev){
    string temp;
    if (word->value=="odd"){
        word++;
        exp(lev);
        gen(OPR,0,6);
    }
    else{
        exp(lev);
        if (word->key=="LOP"){
            temp = word->value;
            word++;
            exp(lev);
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
void exp(int lev){
	int temp = 0;
    if (word->key=="AOP"){  //+|-
        if (word->value =="-")
            temp = 1;
        word++;
    }
    term(lev);
    if (temp==1)
        gen(OPR,0,1);   //栈顶取反
    while (word->key=="AOP"){
        if (word->value=="-")
            temp = 1;
        else
            temp = 0;
        word++;
        term(lev);
        if (temp==1)
            gen(OPR,0,3);
        else
            gen(OPR,0,2); 
    }
}

/*
<term> -> <factor>{<mop><factor>}
*/
void term(int lev){
    string temp;
    factor(lev);
    while (word->key=="MOP"){
        temp = word->value;
        word++;
        factor(lev);
        if (temp=="*")
            gen(OPR,0,4);
        else //if (temp=="/")
            gen(OPR,0,5);
    }
}

/*
<factor> -> <id>|<integer>|(<exp>)
*/
void factor(int lev){
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
        exp(lev);
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

void printtable(){
    fstream fp;
    fp.open("table.txt",ios::out);
    if (!fp){
        fp << "table文件打开失败！" << endl;
        exit(0);
    }
    fp << "\t名称\t类型\t\t值\t空间\t层\t偏移量" << endl;
    for (int i=0; i<ptx; i++)
    {
        fp << i << "\t";
        fp << table[i].name << "\t";
        switch(table[i].kind){
            case constant:
                fp << "constant\t\t";
                fp << table[i].val << "\t\t";
                break;
            case variable:
                fp << "var\t\t";
                fp << " \t\t";
                break;
            case procedure:
                fp << "procedure\t";
                fp << table[i].val << "\t" << table[i].size << "\t";
                break;
        }
        fp << table[i].level << "\t" << table[i].adr << endl;
    }
    fp.close();
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

/////////////// 运行 /////////////// 

//l：层差 b：此次过程基址
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

//解释程序
void interpret(){
    int S[stacksize];
    int p,b,t;  // 指令指针，指令基址，栈顶指针
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
            case LIT:   //取常量a放入数据栈栈顶
                S[t] = code[i].a;
                t++;
                break;
            case OPR:   //运算
                if (code[i].l==1){
                    int num = code[i].a;
                    while (num--){
                        t--;
                        S[t+3] = S[t];
                    }
                }
                else{
                    switch(code[i].a)
                    {
                        case 0:   //返回调用点并退栈
                            t = b;
                            p = S[b+2];  //RA 返回地址
                            b = S[b+1];  //DL 动态链 前面的基址
                            break;
                        case 1:   //栈顶取反
                            S[t-1] = -S[t-1]; 
                            break;
                        case 2:   //次栈顶与栈顶相加
                            t--;
                            S[t-1] = S[t-1]+S[t];
                            break;
                        case 3:   //次栈顶减栈顶
                            t--;
                            S[t-1] = S[t-1]-S[t];
                            break;
                        case 4:   //次栈顶乘栈顶
                            t--;
                            S[t-1] = S[t-1]*S[t];
                            break;
                        case 5:   //次栈顶除栈顶
                            t--;
                            if (S[t]==0)
                            {
                                sendError(30);  //除数为0
                                return;
                            }
                            S[t-1] = S[t-1]/S[t];
                            break;
                        case 6:   //栈顶元素的奇偶性判断
                            S[t-1] = S[t-1]%2;
                            break;
                        case 7:
                            // if (t+temp>=stacksize)
                            // {
                            //     return;
                            // }
                            // t--;
                            // S[t+temp]=S[t];
                            // temp++;
                            break;
                        case 8:   //是否相等
                            t--;
                            S[t-1] = (S[t-1]==S[t]);
                            break;
                        case 9:   //是否不等
                            t--;
                            S[t-1] = (S[t-1]!=S[t]);
                            break;
                        case 10:   //小于判定
                            t--;
                            S[t-1] = (S[t-1]<S[t]);
                            break;
                        case 11:   //大于等于判定
                            t--;
                            S[t-1] = (S[t-1]>=S[t]);
                            break;
                        case 12:   //大于判定   
                            t--;
                            S[t-1] = (S[t-1]>S[t]);
                            break;       
                        case 13:   //小于等于判定
                            t--;
                            S[t-1] = (S[t-1]<=S[t]);
                            break;      
                        case 14:   //栈顶值输出
                            t--;
                            cout << S[t];
                            break;
                        case 15:   //屏幕输出换行
                            cout << endl;
                            break;
                        case 16:   //从命令行读入一个数字输入到栈顶
                            //cout << "请输入数值：";
                            cin >> S[t];
                            t++;
                            break;
                    }
                }
                break;

            case LOD:   //取变量（相对地址为a，层差为L）放到数据栈的栈顶
                S[t] = S[base(code[i].l,S,b)+code[i].a];
                t++;
                break;
            case STO:   //将数据栈栈顶的内容存入变量
                t--;
                S[base(code[i].l,S,b)+code[i].a] = S[t];
                break;
            case CAL:   //调用过程（入口地址为a，层次差为L）
                S[t] = base(code[i].l,S,b);  //SL 父过程基址入栈
                S[t+1] = b;  //DL 本地基址（调用者的基址）
                S[t+2] = p;  //RA
                b = t;
                p = code[i].a;
                temp = 3;
                break;

            case INT:   //数据栈栈顶指针增加a
                t+=code[i].a;
                break;
            case JMP:   //无条件转移到地址为a的指令
                p = code[i].a;
                break;
            case JPC:   //条件转移指令，转移到地址为a的指令
                t--;
                if (S[t]==0)
                    p = code[i].a;
                break;
            case RED:   //读数据并存入变量（相对地址为a，层次差为L）
                //cout << "请输入数值：";
                int temp;
                cin >> temp;
                S[base(code[i].l,S,b)+code[i].a] = temp;
                break;
            case WRT:   //将栈顶内容输出
                t--;
                cout << S[t];
                break;
        }
        // for (int i=0; i<t; i++){
        //     cout << S[i] << " ";
        // }
        // cout << endl;
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
    printtable();
    
    if (ga_error==0){
        cout << "0error!" << endl;
        cout << "successfully compiled..." << endl;
        cout << "********************** 目 标 代 码 **********************" << endl;
        print_code();
        
        interpret();
    }
    closefile();
}



