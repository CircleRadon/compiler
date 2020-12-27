#include <bits/stdc++.h>
using namespace std;

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

void readline();
void sendError(int type);
void prog();
void block();
void vardecl();
void condecl();
void Const();
void proc();
void body();
void statement();
void lexp();
void exp();
void term();
void factor();

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
    block();
}

/*
<block> -> [<condecl>][<vardecl>][<proc>]<body>
*/
void block(){
    if ((word+1)->key=="keyword"){
        sendError(3);
        word++;
    }
    if (word->value=="const"&&word->key=="keyword"){
        word++;
        condecl();
    }
    while (word->key!="keyword"){
        sendError(3);
        word++;
    }
    if (word->value=="var"&&word->key=="keyword"){
        word++;
        vardecl();
    }
    while (word->key!="keyword"){
        sendError(3);
        word++;
    }
    if (word->value=="procedure"&&word->key=="keyword"){
        word++;
        proc();
    }
    while (word->key!="keyword"){
        sendError(3);
        word++;
    }
    // if (word->key=="ID"){
    //     if ((word+1)->value==":="){
    //         sendError(5); //missing const
    //         condecl();
    //     }
    //     else if ((word+1)->value=="("){
    //         sendError(6);  //missing procedure
    //         proc();
    //     }
    //     else if ((word+1)->key=="ID"){
    //         if ((word+2)->value==":="){
    //             sendError(7); //const spell error
    //             word++;
    //             condecl();
    //         }
    //         else if ((word+2)->value=="("){
    //             sendError(8);  //procedure spell error
    //             word++;
    //             proc();
    //         }
    //     }
    //     else{
    //         sendError(9);  //undeclared identifier
    //         while (word->value!=";"){
    //             word++;
    //         }
    //         word++;
    //     }
	// }
    body();
}

/*
<vardecl> -> var <id>{,<id>};
*/
void vardecl(){
    if (word->key=="ID"){
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
void condecl(){
    if (word->key=="ID"){
        word++;
        Const();
    }
    else{
        if ((word+1)->value==":="){
            sendError(11);  //invalid id
            word++;
            Const();
        }
        else{
            sendError(10); //missing id
            Const();
        }
    }
    while(word->value==","&&word->key=="SOP"){
        word++;
        if (word->key=="ID"){
            word++;
            Const();
        }
        else{
             if ((word+1)->value==":="){
                sendError(11);  //invalid id
                word++;
                Const();
            }
            else{
                sendError(10); //missing id
                Const();
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
void Const(){
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
        word++;
    }
    else{
        sendError(15);  //missing int
    }
}


/*
<proc> -> procedure <id> ([<id>{,<id>}]) ;<block>{;<proc>}
*/
void proc(){
    if (word->key=="ID"){
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
    block();

    while(word->value==";"&&word->key=="EOP"){
        word++;
        if (word->value=="procedure"&&word->key=="keyword"){
            word++;
            proc();
        }
        else {
            if ((word+1)->value=="("){
                sendError(6);  //missing procedure
                proc();
            }
            else if ((word+2)->value=="("){
                sendError(8);  //procedure spell error
                word++;
                proc();
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
void body(){
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
    statement();

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
        statement();
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
void statement(){
    //<id> := <exp>
    if (word->key=="ID"){
        word++;
        if (word->value==":="&&word->key=="assignment"){
            word++;
            exp();
        }
        else{           
            if (word->value=="="){
                sendError(13);
                word++;
                exp();
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
        lexp();
        if (word->value=="then"&&word->key=="keyword"){
            word++;
        }
        else{
            sendError(20);  //missing then
            // while (!(word->value==";"||word->value=="end")){
            //     word++;
            // }   
        }
        statement();
        if (word->value=="else"&&word->key=="keyword"){
            word++;
            statement();
        }
    }

    //while <lexp> do <statement>
    else if (word->value=="while"&&word->key=="keyword"){
        word++;
        lexp();
        if (word->value=="do"&&word->key=="keyword"){
            word++;
        }
        else{
            sendError(21);  //missing do
            // while (!(word->value==";"||word->value=="end")){
            //     word++;
            // }   
        }
        statement();
    }
    //call <id> ([<exp>{,<exp>}])
    else if (word->value=="call"&&word->key=="keyword"){
        word++;
        if (word->key=="ID"){
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

        int last=1;  //,|id
        while(word->value==","||word->key=="AOP"||word->key=="ID"||word->key=="INT"||word->value=="("){
            if (word->value==","&&last==0){  //,
                word++;
                last = 1;
            }
            else if (word->value!=","&&last==1){ //exp
                exp();
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
            word++;
        }
        else{
            sendError(10);
        }
        int last = 0;
        while (word->value==","||word->key=="ID"){
            if (word->value==","&&last==0){
                word++;
            }
            else if (word->key=="ID"&&last==1){
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
        exp();
        int last=0;  //,|exp
        while(word->value==","||word->key=="AOP"||word->key=="ID"||word->key=="INT"||word->value=="("){
            if (word->value==","&&last==0){  //,
                word++;
                last = 1;
            }
            else if (word->value!=","&&last==1){ //exp
                exp();
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
        body();
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
void lexp(){
    if (word->value=="odd"){
        word++;
        exp();
    }
    else{
        exp();
        if (word->key=="LOP"){
            word++;
            exp();
        }
        else{
            sendError(22); //Missing the compare operator
        }
    }
}

/*
<exp> -> [+|-]<term>{<aop><term>}
*/
void exp(){
    if (word->key=="AOP"){  //+|-
        word++;
    }
    term();
    if (word->key=="AOP"){
        word++;
        term();
    }
}

/*
<term> -> <factor>{<mop><factor>}
*/
void term(){
    factor();
    if(word->key=="MOP"){
        word++;
        factor();
    }
}

/*
<factor> -> <id>|<integer>|(<exp>)
*/
void factor(){
    if (word->key=="ID"){
        word++;
    }
    else if (word->key=="INT"){
        word++;
    }
    else if (word->key=="("&&word->value=="SOP"){
        word++;
        exp();
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
    }
    closefile();
}


