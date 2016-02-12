#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef struct ophashnode{
 
  char* opchar;                      //this is the struct for the hashtable that holds the c operators and hex letters
  char* opname;
  struct  ophashnode* next;
} ophashnode;


typedef struct ophashtable{
  int size;
  int items;
  ophashnode ** table;
} ophashtable;

ophashtable* root = NULL;

void destroy(){
  int i;
  int items;
  ophashnode* tmp;
  ophashnode* trash;
  ophashnode** tmp1;

  if (root == NULL||root->table == NULL)
    {                                                   //deletes hashtable
      return;
    }
  else
    {
      items = root->items;
      tmp1= root->table;
      for(i=0;i<items;i++)
	{
	  tmp = *tmp1;
	  while (tmp!=NULL)
	    {
	      if(tmp->next!=NULL)
		{
		  trash = tmp->next;
		  free(tmp->opchar);
		  free(tmp->opname); 
		  free(tmp);
		  tmp = trash;
		  i++;
		  continue;
		}
	      else
		{
		  free(tmp->opchar);
		  free(tmp->opname);
		  free(tmp);
		  tmp = NULL;

		}
	    }
	  tmp1++;
	}

      free(root->table);
      free(root);
      root = NULL;
    }
}


ophashtable* constructor(int size){
  int i;
  ophashnode** ptr = NULL;
  ophashtable* new = NULL;
  new = (ophashtable*)malloc(1*sizeof(ophashtable));
  new->table = (ophashnode**)malloc(size*sizeof(ophashnode*));
  ptr = new->table;
  for(i=0;i<size;i++)                                                   //creates hashtable
    {
      ptr[i] = NULL;
    }
  new->size = size;
  new->items = 0;
  return new;

}
int rol(unsigned value, int places){
  int WORD_LENGTH = (8*sizeof(value));
  return((value<<places)|(value>>(WORD_LENGTH-places)));

}                                                                     //hashing function
int hash(char const *input,int size){
  int result = 0x55555555;
  while (*input)
    {
      result ^= *input++;
      result = rol(result, 5);
    }
  

  return (abs(result)%size);
}


int set( char* opchar, char* opname)
{
  int hashindex;
  hashindex = hash(opchar, 100);
  ophashnode* new = (ophashnode*) malloc(sizeof(ophashnode));
  new->opname = (char*) malloc(sizeof(opname));
  strcpy(new->opname,opname);
  new->opchar = (char*) malloc(sizeof(opchar));
  strcpy(new->opchar,opchar);
  new->next = NULL;
  if(root->table[hashindex]==NULL)
    {
      root->items++;
 
      root->table[hashindex] = new;
      return 1;
    }                                                                   //insert into hashtable
  else
    {
      ophashnode* pointer = root->table[hashindex];
      while(pointer->next!=NULL)
        {
          if ((strcmp(pointer->opname,new->opname)==0))
	    {
	      return 0;
	    }
          else
            {
              pointer = pointer->next;
            }

	  if ((strcmp(pointer->opname,new->opname)==0))
	    {
	      return 0;
	    }

        }
       root->items++;
      if (strcmp(pointer->opname,new->opname)==0)
	{
	  return 0;
	}
      else
        {
          pointer->next = new;
          return 1;
        }

    }
}


char* get(char* opchar)
{
  char* err = "err";
  int hashindex = hash(opchar,100);
  ophashnode* pointer = root->table[hashindex];
  if(pointer == NULL)
    {
      return err;
    }
  else if(strcmp(pointer->opchar,opchar)==0)
    {
      return pointer->opname;
    }
  else
    {
      while(pointer!= NULL)
        {
	  if (strcmp(pointer->opchar,opchar)==0)
	    {
	      return pointer->opname;
	    }
	  else                                                    //retrieve from hashtable
	    {
	      pointer = pointer->next;
	    }
        }
      return err;
    }


}


typedef struct CLL
{
  int count;
  struct listnode* tail;
}CLL;
typedef struct listnode
{
  char data;
  struct listnode* next;
}listnode;                                  //so I read the input stream char by char and stored into a circular linked list, here are the structs and funcs for it


void  addEnd (char val,CLL* list)
{
  listnode* tmp = NULL;
  listnode* new = NULL;
  new = (listnode*)malloc(1*sizeof(listnode));
  new -> data = val;
  new -> next = NULL;
  if (list->tail == NULL)
    {

      list->tail = new;                                     //insert into linked list
      list-> tail->next = NULL;
      list->count++;
      return;
    }
  else if (list->tail->next == NULL)
    {
      tmp =list->tail;
      list-> tail = new;
      list-> tail->next = tmp;
      tmp->next =list-> tail;
      list->count++;
      return;
    }
  else
    {
      tmp = list ->tail;
      list-> tail = new;
      list-> tail->next = tmp ->next;
      tmp->next = list->tail;
      list->count++;
      return;
    }
}
char* CLLtoStr(CLL* list)
{
  listnode* point;
  int count = list->count;
  char* str = NULL;
  str = (char *) calloc(count,sizeof(char));
  int ptr;
  if(list->tail == NULL)
    {
      free(str);
      str = NULL;
      return NULL;
    }
  else if(list->tail->next == NULL)
    {
      point = list->tail;
    }
  else                                                       //turn linked list into a string
    {
      point = list->tail->next;
    }
  for(ptr = 0; ptr<count;ptr++)
    {
      str[ptr] = point->data;
      point = point->next;
    }
  return str;
}
void deleteCLL (CLL* list)
{
  if(list->tail == NULL)
    {
      return;
    }
  listnode* ptr = list->tail->next;                          //delete a linked list
  listnode* tmp;
  tmp = ptr;
  int count = list->count;
  if(ptr == NULL)
    {
      free(list->tail);
      list->tail = NULL;
      list->count = 0;
      return;
    }
  while (count!=1)
    {

      ptr = ptr->next;
      free(tmp);
      tmp = ptr;
      count--;
    }
  free(ptr);
  ptr = NULL;
  tmp = NULL;
  list->tail = NULL;
  list->count = 0;
  return;
}





typedef enum{

  word,
  decimalint,
  octint,
  hexint,
  floatpnt,
  coperator,
  invalid,
  keyword


} tokentype;

/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */


struct TokenizerT_ {
  tokentype thistoken;
  char* token;



};

typedef struct TokenizerT_ TokenizerT;

/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 * 
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */
/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */

TokenizerT *TKCreate( char * ts ) {

  TokenizerT* new = (TokenizerT*)malloc(sizeof(TokenizerT));
  char * ptr= ts;
  char * key ="key word";
  char* z = "err";
  if(*ptr == '0'){
    ptr++;
    if(*ptr == 'x'||*ptr == 'X'){
      new->thistoken = hexint;
      new->token = (char*)malloc(sizeof(ts));
      strcpy(new->token,ts);
      free(ts);
      return new;
      
    }
    else if(ptr!=NULL&&*ptr!='\0'){
      while(ptr!=NULL&&*ptr!='\0'){                                 //So i take the token string, I.D. the tokentype, then store the token and type in memory
	if(((int)(*ptr-'0'))>=8){
	
	new->thistoken = decimalint;
	new->token = (char*)malloc(sizeof(ts));
	strcpy(new->token,ts);
	free(ts);
	return new;
      }
      ptr++;
    }
      
    new->thistoken=octint;
    new->token = (char*)malloc(sizeof(ts));
    strcpy(new->token,ts);
    free(ts);
    return new;
    
    }
    if(*ptr == '\0'){
      new->thistoken = decimalint;
      new->token = (char*)malloc(sizeof(ts));
      strcpy(new->token,ts);
      free(ts);
      return new;

    }
  }
  char* ptr1;
  if(isalpha(*ptr)){
    new->thistoken = word;
    new->token = (char*)malloc(sizeof(ts));
    strcpy(new->token,ts);
    if((strcmp(get(new->token),key)==0)){
      new ->thistoken = keyword;            
    }
    free(ts);
    return new;

  }
  else if(isdigit(*ptr)){
    new->thistoken = decimalint;
    ptr1 = ptr;
    while(isdigit(*ptr1)){
      ptr1++;
    }
    if (*ptr1 == '.'){
      new->thistoken = floatpnt;
      
    }

    new->token = (char*)malloc(sizeof(ts));
    strcpy(new->token,ts);
    free(ts);
    return new;

    
  }

  if(strcmp(get(ts),z)!=0){
    new->thistoken = coperator;
    new->token = (char*)malloc(sizeof(ts));
    strcpy(new->token,ts);
    free(ts);
    return new;

  }
  else{
    new->thistoken = invalid;
    new->token = (char*)malloc(sizeof(ts));
    strcpy(new->token,ts);
    free(ts);
    return new;


  }
  return NULL;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy( TokenizerT * tk ) {
  free(tk->token);
  free(tk);
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken( char**  tkstr ) {
  char* ptr = *tkstr;
  char* ptr1;
  char* token;                                         //So I read the input stream char by char and construct the token to pass to TKCreate.
  char t[2];                                          //I changed the parameter to allow me to have this method advance the input stream pointer.
  char u[3];
  char* z = "hex";
  char* err = "err";
  CLL* list = (CLL*) malloc(sizeof(CLL));
  list->count = 0;
  
  if(isalpha(*ptr)){
  
    for(ptr1 = ptr;ptr1!=NULL&&isalpha(*ptr1);ptr1++){
      addEnd(*ptr1,list);
    }
    *tkstr = ptr1;
    addEnd('\0',list);
    token = CLLtoStr(list);
    deleteCLL(list); 
    return token;

  }
  else if(isdigit(*ptr)){
    if(*ptr == '0'){
      addEnd(*ptr,list);
      ptr++;
      if(*ptr == 'x'||*ptr == 'X'){
	addEnd(*ptr,list);
	ptr++;
	for(ptr1 = ptr;ptr1!=NULL&&*ptr1!='\0';ptr1++){
	  if(isdigit(*ptr1)){
	    addEnd(*ptr1,list);  
	  }
	  else if(isalpha(*ptr1)){
	    
	    t[0] = *ptr1;
	    t[1] = '\0';
	    if(strcmp(get(t),z)==0){
	      addEnd(*ptr1,list);
	    }
	    else{
	      break;
	    }
	  }
	  else{
	    break;
	  }
	  
	}
	*tkstr = ptr1;
	addEnd('\0',list);
	token = CLLtoStr(list);
	deleteCLL(list); 
	return token;
      }
      else if(isdigit(*ptr)){
	if(((int)(*ptr - '0'))<=7 ){
	  while(isdigit(*ptr)&&(((int)(*ptr - '0'))<=7)){
	    addEnd(*ptr,list);
	    ptr++;
	  }
	  *tkstr = ptr;
	  addEnd('\0',list);
	  token = CLLtoStr(list);
	  deleteCLL(list); 
	  return token;
	}
	else{
	  while(isdigit(*ptr)){
	    addEnd(*ptr,list);
	    ptr++;
	  }
	  *tkstr = ptr;
          addEnd('\0',list);
          token = CLLtoStr(list);
          deleteCLL(list); 
	  return token;

	}
	
      }
      else{
	*tkstr = ptr;
	addEnd('\0',list);
	token = CLLtoStr(list);
	deleteCLL(list); 
	return token;
      }
    }
    else{
      
      while(isdigit(*ptr)){
	addEnd(*ptr,list);
	ptr++;
      }
      if(*ptr == '.'){
	addEnd(*ptr,list);
	ptr++;
	while(isdigit(*ptr)){
	  addEnd(*ptr,list);
	  ptr++;
	}
	if(*ptr == 'e'){
	  addEnd(*ptr,list);
	  ptr++;
	  if(isdigit(*ptr)){
	    while(isdigit(*ptr)){
	      addEnd(*ptr,list);
	      ptr++;
	    }
	  }
	  else if(*ptr == '-'){
	    addEnd(*ptr,list);
	    ptr++;
	    while(isdigit(*ptr)){
              addEnd(*ptr,list);
              ptr++;
            }
	    
	  }
	}
      }
    }
  
  
  *tkstr = ptr;
  addEnd('\0',list);
  token = CLLtoStr(list);
  deleteCLL(list); 
  return token;
  
  }
  
  else if(isalpha(*ptr)){
    while(isalpha(*ptr)){
      addEnd(*ptr,list);
      ptr++;
    }
    *tkstr = ptr;
    addEnd('\0',list);
    token = CLLtoStr(list);
    deleteCLL(list);
    return token;
  }
  else{
    t[0] = *ptr;
    t[1]= '\0';
    if(strcmp(get(t),err)!=0){
      addEnd(*ptr,list);
      u[0]=*ptr;
      ptr++;
      u[1]= *ptr;
      u[2]= '\0';
      if(strcmp(get(u),err)!=0){
	addEnd(*ptr,list);
	ptr++;
	*tkstr = ptr;
	addEnd('\0',list);
	token = CLLtoStr(list);
	deleteCLL(list);
	return token;
      }
      else{
	*tkstr = ptr;
        addEnd('\0',list);
        token = CLLtoStr(list);
        deleteCLL(list);
        return token;
      }
    }
    else{
      addEnd(*ptr,list);
      ptr++;
      *tkstr = ptr;
      addEnd('\0',list);
      token = CLLtoStr(list);
      deleteCLL(list);
      return token;
    }
    
    
  }
  

  return "0";
}

void printToken(TokenizerT* tk){
  char * str = tk->token;
  char* err = "err";
  tokentype thistype;
  thistype = tk->thistoken;
  switch(thistype){

  case word:
    printf("word \"%s\"\n",tk->token);
    break;
  case keyword:
    printf("key word \"%s\"\n",tk->token);
    break;
  case decimalint:
    printf("decimal integer \"%s\"\n",tk->token);       //Prints out the appropriate text depending on tokentype
    break;
  case octint:
    printf("octal integer \"%s\"\n",tk->token);
    break;
  case hexint:
    printf("hex integer \"%s\"\n",tk->token);
    break;
  case floatpnt:
    printf("floating point \"%s\"\n",tk->token);
    break;
  case coperator:
    if(strcmp(get(tk->token),err)==0){
      if(*str == ' '||*str == '\t'||*str == '\v'||*str == '\f'||*str == '\n'||*str== '\r'){
	break;
      }     
    }
    else{
    printf("%s \"%s\"\n",get(tk->token),tk->token);
    break;
    }
  case invalid:

    if(strcmp(get(tk->token),err)==0){
      if(*str == ' '||*str == '\t'||*str == '\v'||*str == '\f'||*str == '\n'||*str== '\r'){
        break;
      }
    }


    printf("invalid token \"%s\"\n",tk->token);
    break;
  default:
    printf("sumtin wrong \"%s\"\n",tk->token);
    break;


    
  }


}

/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) {


  if (argc!=2){
    return 0;
  }
  root = constructor(100);
  
  set("auto","key word");
  set("break","key word");
  set("case","key word");
  set("char","key word");
  set("const","key word");
  set("continue","key word");
  set("default","key word");
  set("do","key word");
  set("double","key word");
  set("else","key word");
  set("enum","key word");
  set("extern","key word");
  set("float","key word");
  set("for","key word");
  set("onto","key word");
  set("if","key word");
  set("int","key word");
  set("long","key word");
  set("register","key word");
  set("return","key word");
  set("short","key word");
  set("signed","key word");
  set("sizeof","key word");
  set("static","key word");
  set("struct","key word");
  set("switch","key word");
  set("typedef","key word");
  set("union","key word");
  set("unsigned","key word");
  set("void","key word");
  set("volatile","key word");
  set("while","key word");
  
  set("+","addition operator");
  set("-","subtraction operator");
  set("*","multiplication operator");
  set("/","division operator");
  set("%","modulus operator");
  set("++","increment operator");
  set("--","decrement operator");
  set("==","equivalency operator");
  set("!=","nonequivalency operator");
  set(">","greater than operator");                  //load all operators and hex letters in hashtable
  set("<","less than operator");
  set(">=","greater than or equal to");
  set("<=","less than or equal to ");
  set("&&","logical and");
  set("||","logical or");
  set("!","logical not");
  set("&","bitwise and");
  set("|","bitwise or");
  set("^","bitwise xor");
  set("~","bitwise flip");
  set("<<","bitwise leftshift");
  set(">>","bitwise rightshift");
  set("=","assignment operator");
  set("+=","add and assign");
  set("-=","subtract and assign");
  set("*=","multiply and assign");
  set("/=","divide and assign");
  set("%=","modulus and assign");
  set("&=","bitwise and,then assign");
  set("^=","bitwise xor,then assign");
  set("[","left brace");
  set("]","right brace");
  set("{","left curly brace");
  set("}","right curly brace");
  set(";","semi colon");
  set("'","single quote");
  set("\"","double quote");
  set("\\\\","line comment");
  set("\\*","comment from");
  set("*\\","comment till");
  set("_","underscore");
  set("(","left parentheses");
  set(")","right parentheses");
  set("$","dolla");
  set("#","pound symbol");
  set("@","at symbol");
  set("`","idk");
  set("\\","backslash");
  set(",","comma");
  set(".","period");
  set(":","colon");
  set("a","hex");
  set("b","hex");
  set("c","hex");
  set("d","hex");
  set("e","hex");
  set("f","hex");
  set("A","hex");
  set("B","hex");
  set("C","hex");
  set("D","hex");
  set("E","hex");
  set("F","hex");

  char* s = argv[1];
  TokenizerT* new;    //take input stream
  char* ptr;
  
  for(ptr = s;ptr!=NULL&&*ptr!='\0';){

    new = TKCreate(TKGetNextToken(&ptr));              //tokenize input stream
    printToken(new);                                   //output token
    TKDestroy(new);                               //destroy token
  }

  
  destroy();  
  return 0;



}
