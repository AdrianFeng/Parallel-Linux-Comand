// UCLA CS 111 Lab 1 command reading
//  team group
//  Zhen Feng 
//  Yi Wang 

#include "command.h"
#include "command-internals.h"
#include "alloc.h"
#include <error.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

struct command_stream
{
  command_t comm;
  struct command_stream *next;
};
//stack for making a tree
struct stack
{
    command_t command[20];
    int item; //number of item
};
void pop(struct stack *sta)
{
    sta->item--;
}
void push(struct stack *sta, command_t com)
{
    sta->command[sta->item]=com;
    sta->item++;
}
int empty(struct stack *sta)
{
    return sta->item==0;
}
int sizeofstack(struct stack *sta)
{
    return sta->item;
}
command_t element(struct stack *sta,int n)
{
  return sta->command[n];
}
command_t top(struct stack *sta)
{
    return sta->command[sta->item-1];
}
typedef struct charstack //stack for reading
{
    char newchar;
    int linenumber;
    int doubl;
    struct charstack * next;
}charstack_t;

void charpush( charstack_t ** top, char thing, int line, int dou)
{
    charstack_t * newone;
    newone=checked_malloc(sizeof(charstack_t));
    newone->newchar=thing;
    newone->linenumber=line;
    newone->doubl=dou;
    newone->next=*top;
    *top=newone;
}

char gettopchar(charstack_t * top)
{
    if (top!=NULL){
        return top->newchar;}
    else {return 0;}
}
int gettopline(charstack_t * top)
{
    if (top!=NULL){
        return top->linenumber;}
    else {return 0;}
}
int gettopdou(charstack_t * top)
{
    if (top!=NULL){
        return top->doubl;}
    else {return 0;}
}
void popchar(charstack_t ** top)
{
        charstack_t * tmp =*top;
        *top=(*top)->next;
        free(tmp);
}
int checkspecificandpop(charstack_t **top, char thing)
{
  charstack_t * curr = * top;
  charstack_t * prev = *top;
 
  while (curr!=NULL)
    {
      if ( curr->newchar == thing ){
	if(curr == *top)
	  {
	    free(curr);
	    *top =(*top)->next;
	    return 1;
	  }
	else {
	  prev->next = curr->next;
	  free(curr);
	  return 1;
	}
      }
      prev = curr;
      curr = curr->next;
      
    }
  return 0;
}

int Isword (char thing) //check word
{
    if(thing=='!'||thing=='%'||(thing>='+'&&thing<=':')||(thing>='@'&&thing<='Z')||thing=='^'||thing=='_'||(thing>='a'&&thing<='z'))
    {return 1;}
    else
    {return 0;}
}

int Istoken(char thing) //check token
{
    if (thing==';' || thing=='|' || thing=='&' || thing=='(' || thing==')'  || thing == '<' || thing == '>')
    {return 1;}
    else
    {return 0;}
}
int Isother(char thing) //check is other like commant
{
    if (thing =='#' || thing == ' ' || thing == '\n' || thing == '\t')
    {return 1;}
    else{return 0;}
}
int checkchar(char thing)
{
    if (Isword(thing)) {return 1;}
    else if (Istoken(thing)){return 2;}
    else if (Isother(thing)){return 3;}
    else {return 4;}
}
char* check (char * string, int* count) //new string check function return pointer and update the count as the number of lines
{
    int newsize;
    int m=0;
    int i=0;
    int a=0;
    int b=0;
    int size=(int)strlen(string);
    if (string==NULL){ fprintf(stderr, "nothing input !"); exit(-1);}
    char tmpnow;
    char tmpbefore=string[a];
    char* new = (char*)malloc(3*size*sizeof(char));
    int currentline=1;
    int charflag=checkchar(string[a]);
    int wordsflag=0;
    int commentflag=0;
    int newlineflag=0;
    int selfmodify =0;
    int self1=0;
    int tmp;
    charstack_t * topp=NULL;
    
    if (charflag==1 || string[a]=='(' || string[a]=='#' || string[a]==' ' || string[a] == '\n')
    {
        if (charflag==1){new[b]=string[a]; a++;b++; wordsflag=1;}
        else
        {
            switch (string[a]) {
                case '(':
                    charpush(&topp, '(',currentline,1);new[b]=string[a]; a++;b++;wordsflag=0;break;
                case '#':
                    commentflag=1;a++;break;
                case ' ':
                    a++;break;
                case '\n':
                    newlineflag=1;a++;break;}
        }
    }
    else {fprintf(stderr,"Line number : %d , %c is not supposed to be at the beginning of the line \n ",currentline,string[a]);exit(-1);}
    while (a<size)
    {
        tmpnow=string[a];
        charflag=checkchar(tmpnow);
        if (commentflag==0)
        {
            if (charflag==1)
            {
	      selfmodify=0;
                if (wordsflag==0 && tmpbefore != ' '){ new[b]=' ';b++; }
                new[b]=tmpnow;a++;b++;
                wordsflag=1;
                newlineflag=0;
		tmpbefore=tmpnow;
		charstack_t * tmp3= topp;
		char tmp4;
		if (topp!= NULL)
		  {
		    while (tmp3 != NULL)
		      {
			if ( tmp3->newchar == '(' ) {tmp3=tmp3->next;continue;}
			else
			  {
			    tmp4=tmp3->newchar;
			    checkspecificandpop(&topp,tmp4);
			    break;
			  }
		      }
		  }
                //if (topp!=NULL && gettopchar(topp) != '(' && gettopchar(topp) != ')'){ popchar(&topp);}
                continue;
            }
            else if (charflag == 3)
            {
                switch (tmpnow) {
                    case '#':
                        commentflag=1;a++;tmpbefore=tmpnow;continue;break;
                    case ' ':
		         new[b]=tmpnow;b++;a++;tmpbefore=tmpnow;continue;break;
                    case '\t':
                        new[b]=' ';b++;a++;tmpbefore=' ';continue;break;
                    case '\n':
		      new[b]=' ';b++;
		      new[b]=tmpnow;
		      if(newlineflag==1){currentline++;a++;continue;break;}
		      ///////////////////////////////////////////
		      if (gettopchar(topp)== '(' && wordsflag==1) { new[b]=';';selfmodify=b;
			charpush(&topp, ';',currentline,1);tmp=wordsflag; wordsflag=0;currentline++;a++;b++;newlineflag=1;tmpbefore=tmpnow;continue;break;}
		      /////////////////////////////////////////////
		      if (wordsflag==0){
			if (gettopchar(topp) == '<' || gettopchar(topp) == '>')
			  {fprintf(stderr, "Line number : %d , %c cannot be just before newline \n ",currentline,gettopchar(topp));exit(-1);}
			new[b]=' ';
			if ( gettopchar(topp) == 0 || self1==1 ) { new[b]='\n';self1=0;}
		      }
                       a++;b++;currentline++;newlineflag=1;tmpbefore=tmpnow;continue;break;}
            }
            else if (charflag==2)
            {
                switch (tmpnow) {
                    case ';':
		      selfmodify=0;self1=0;
                        if (newlineflag==1)
                        {fprintf(stderr, "Line number : %d , %c cannot be just after newline \n ",currentline,tmpnow);exit(-1);}
                        if (gettopchar(topp)==';')
                        {fprintf(stderr, "Line number : %d , %c repeatted \n ",gettopline(topp),tmpnow);exit(-1);}
                        if (wordsflag==0)
                        {fprintf(stderr, "Line number : %d , no words before %c \n ",currentline,tmpnow);exit(-1);}
                        if(wordsflag==1 && tmpbefore != ' ') {new[b]=' ';b++;}
                        charpush(&topp, ';',currentline,1);wordsflag=0;new[b]=tmpnow;b++;a++;tmpbefore=tmpnow;continue;break;
                    case '&':
		      selfmodify=0;self1=0;
                        if (newlineflag==1)
                        {fprintf(stderr, "Line number : %d , %c cannot be just after newline \n ",currentline,tmpnow);exit(-1);}
                        if (((a+1)<size) && string[a+1]!='&')
                        {fprintf(stderr, "Line number : %d , %c is single, not accepted \n ",currentline,tmpnow);exit(-1);}
                        if (gettopchar(topp)=='&')
                        {fprintf(stderr, "Line number : %d , %c%c repeatted \n ",gettopline(topp),tmpnow,tmpnow);exit(-1);}
                        if (wordsflag==0)
                        {fprintf(stderr, "Line number : %d , no words before %c%c \n ",currentline,tmpnow,tmpnow);exit(-1);}
                        if(wordsflag==1 && tmpbefore != ' ') {new[b]=' ';b++;}
                        charpush(&topp, '&',currentline,2);wordsflag=0;new[b]=tmpnow;new[b+1]=tmpnow;
                        b=b+2;a=a+2;tmpbefore=tmpnow;continue;break;
                    case '|':
		      self1=0;
		      selfmodify=0;
                        if (newlineflag==1)
                        {fprintf(stderr, "Line number : %d , %c cannot be just after newline \n ",currentline,tmpnow);exit(-1);}
                        if(wordsflag==1 && tmpbefore != ' ') {
			  new[b]=' ';b++;}
                        if ( ((a+1)<size) && string[a+1]!='|'){
                            if (gettopchar(topp)=='|' && gettopdou(topp)==1)
                            {fprintf(stderr, "Line number : %d , %c repeatted  \n ",currentline,tmpnow);exit(-1);}
                            if (wordsflag==0)
                            {fprintf(stderr, "Line number : %d , no words before %c \n ",currentline,tmpnow);exit(-1);}
                            charpush(&topp, '&',currentline,1);wordsflag=0;new[b]=tmpnow;b=b+1;a=a+1;
                        }else{
                            if (gettopchar(topp)=='|' && gettopdou(topp)==2)
                            {fprintf(stderr, "Line number : %d , %c%c repeatted \n ",currentline,tmpnow,tmpnow);exit(-1);}
                            if (wordsflag==0)
                            {fprintf(stderr, "Line number : %d , no words before %c%c \n ",currentline,tmpnow,tmpnow);exit(-1);}
                            charpush(&topp, '&',currentline,2);wordsflag=0;new[b]=tmpnow;new[b+1]=tmpnow;b=b+2;a=a+2;
                        }tmpbefore=tmpnow;continue;break;
                    case '<':
		      selfmodify=0;self1=0;
                        if (newlineflag==1)
                        {fprintf(stderr, "Line number : %d , %c cannot be just after newline \n ",currentline,tmpnow);exit(-1);}
                         if(wordsflag==1 && tmpbefore != ' ') {new[b]=' ';b++;}
			 if (((a+1)<size) && string[a+1]!='<'){
                            if (gettopchar(topp)=='<' && gettopdou(topp)==1)
                            {fprintf(stderr, "Line number : %d , %c repeatted \n ",currentline,tmpnow);exit(-1);}
                            if (wordsflag==0)
                            {fprintf(stderr, "Line number : %d , no words before %c \n ",currentline,tmpnow);exit(-1);}
                            charpush(&topp, '<',currentline,1);wordsflag=0;new[b]=tmpnow;b=b+1;a=a+1;
                        }else{
                            if (gettopchar(topp)=='<' && gettopdou(topp)==2)
                            {fprintf(stderr, "Line number : %d , %c%c repeatted \n ",currentline,tmpnow,tmpnow);exit(-1);}
                            if (wordsflag==0)
                            {fprintf(stderr, "Line number : %d , no words before %c%c \n ",currentline,tmpnow,tmpnow);exit(-1);}
                            charpush(&topp, '<',currentline,2);wordsflag=0;new[b]=tmpnow;new[b+1]=tmpnow;b=b+2;a=a+2;
                        }tmpbefore=tmpnow;continue;break;
                    case '>':
		      selfmodify=0;self1=0;
                        if (newlineflag==1)
                        {fprintf(stderr, "Line number : %d , %c cannot be just after newline \n ",currentline,tmpnow);exit(-1);}
                        if(wordsflag==1 && tmpbefore != ' ') {new[b]=' ';b++;}
                        if (((a+1)<size) && string[a+1]!='>'){
                            if (gettopchar(topp)=='>' && gettopdou(topp)==1)
                            {fprintf(stderr, "Line number : %d , %c repeatted \n ",currentline,tmpnow);exit(-1);}
                            if (wordsflag==0)
                            {fprintf(stderr, "Line number : %d , no words before %c \n ",currentline,tmpnow);exit(-1);}
                            charpush(&topp, '>',currentline,1);wordsflag=0;new[b]=tmpnow;b=b+1;a=a+1;
                        }else{
                            if (gettopchar(topp)=='>' && gettopdou(topp)==2)
                            {fprintf(stderr, "Line number : %d , %c%c repeatted \n ",currentline,tmpnow,tmpnow);exit(-1);}
                            if (wordsflag==0)
                            {fprintf(stderr, "Line number : %d , no words before %c%c \n ",currentline,tmpnow,tmpnow);exit(-1);}
                            charpush(&topp, '>',currentline,2);wordsflag=0;new[b]=tmpnow;new[b+1]=tmpnow;b=b+2;a=a+2;
                        }tmpbefore=tmpnow;continue;break;
                    case '(':
		      selfmodify=0;self1=0;
		      if (wordsflag==1)
			  {
			    if (newlineflag==0)
			      {fprintf(stderr, "Line number : %d , there are words just before %c \n",currentline,tmpnow);exit(-1);}
			    else
			      {if (gettopchar(topp)=='(')
				  {fprintf(stderr,"Line number : %d, there are words just before %c \n ", currentline,tmpnow);exit(-1);}
			      }
			  }
                        if(tmpbefore != ' ') {new[b]=' ';b++;}
                        charpush(&topp, '(',currentline,1);newlineflag=0;new[b]=tmpnow;b=b+1;a=a+1;tmpbefore=tmpnow;wordsflag=0;continue;break;
                    case ')':
		      self1=1;
                        if (wordsflag==0 && selfmodify == 0){fprintf(stderr, "Line number : %d , there are no words before %c \n",currentline,tmpnow);exit(-1);}
			if (selfmodify !=0) {popchar(&topp);new[selfmodify] = ' ';selfmodify=0;wordsflag=tmp;}
                        if (checkspecificandpop(&topp, '(') ==0 ) {fprintf(stderr, "Line number : %d , ( and ) doesn't match \n",currentline);exit(-1);}
                        if(tmpbefore != ' ') {new[b]=' ';b++;}new[b]=tmpnow;b=b+1;a=a+1;tmpbefore=tmpnow;newlineflag=0;continue;
                        break;}
            }
            else {fprintf(stderr, "Line number : %d , %c is not in the shell syntax subset \n ",currentline,tmpnow);exit(-1);}
        }
        else
        {
            if (tmpnow=='\n'){a++;commentflag=0;}
            else {a++; continue;}
        }
    }
    if (gettopchar(topp) !=0 )
      {
	if (checkspecificandpop(&topp,'(') == 1)
	  {fprintf(stderr, "Line number : %d , ( and ) doesn't match \n ",gettopline(topp));
	    exit(-1);}
	else 
	  {
	    fprintf(stderr, "Line number : %d , single token can not be put at the end of file \n",currentline);
            popchar(&topp);
	    exit(-1);
	  }
      }
    while (gettopchar(topp) !=0 ) {popchar(&topp);}
    if ( b == 0) {fprintf(stderr, "nothing input !  \n");exit(-1);}
    if (new[b-1] != '\n')
      {
	new[b]='\n';
	b=b+1;
      }
    newsize=b;
    while (m<newsize)
    {
      if (new[m]=='\n'){i=i+1;}
      // printf("%c",new[m]);
	m+=1;
    }
    *count=i;
    return new;//should be changed
}
void cleanstack(struct stack *sta) //make node for last 3 element in stack;
{
  if(sizeofstack(sta)>=3)
            {
                struct command *head=malloc(sizeof(struct command));
                struct command *left=malloc(sizeof(struct command));
                struct command *right=malloc(sizeof(struct command));
                head=top(sta);
                pop(sta);
                right=top(sta);
                pop(sta);
                left=top(sta);
                pop(sta);
                head->u.command[0]=left;
                head->u.command[1]=right;
                push(sta,head);                
            }
}
void checkstack(struct stack *myorderstack,struct stack *mystack)
{
   int change=0;
     while(sizeofstack(myorderstack)>=2&&change==0)
	      {
		struct command *second=top(myorderstack);
		pop(myorderstack);
		struct command *first=top(myorderstack);
		pop(myorderstack);
		int levelfirst;
		int levelsecond;
		if(first->type==AND_COMMAND||first->type==OR_COMMAND)
		  {
	      	    levelfirst=2;
		  }
		if(first->type==SEQUENCE_COMMAND)
		  {
		    levelfirst=1;
		  }
		if(first->type==PIPE_COMMAND)
		  {
		    levelfirst=3;
		  }
		if(first->type==SIMPLE_COMMAND)
		  {
		    levelfirst=0;
		  }
		if(second->type==AND_COMMAND||second->type==OR_COMMAND)
		  {
		    levelsecond=2;
		  }
		if(second->type==SEQUENCE_COMMAND)
		  {
		    levelsecond=1;
		  }
		if(second->type==PIPE_COMMAND)
		  {
		    levelsecond=3;
		  }
		if(second->type==SIMPLE_COMMAND)
		  {
		    levelsecond=0;
		  }
		if(levelfirst!=0&&levelsecond!=0)
		  {
		    if(levelfirst>=levelsecond)
		      {
			struct command *node=top(mystack);
			pop(mystack);
			push(mystack,first);
			push(myorderstack,second);
			cleanstack(mystack);
			push(mystack,node);
	              }
		    else
		      {
			push(myorderstack,first);
			push(myorderstack,second);
			change=1;
		      }
		  }
		else
		  {
		    push(myorderstack,first);
		    push(myorderstack,second);
		    change=1;
		  }
	      }
}
int isword(char* str) //check if it is a word
{
    int size=(int)strlen(str);
    int i=0;
    for(i=0;i<size;i++)
    {
        if(str[i]=='!'||str[i]=='%'||(str[i]>='+'&&str[i]<=':')||(str[i]>='@'&&str[i]<='Z')||str[i]=='^'||str[i]=='_'||(str[i]>='a'&&str[i]<='z'))
        {
            continue;
        }
        else
        {
            return 0;//false
        }
    }
    return 1;//true
}
char** pr2(char* str,char *pattern,size_t * size)
{
    char **word;
    size_t si=1024;
    word=(char **)checked_malloc(si*sizeof(char*));
    int i=0;
    size_t count=0;
    word[i]= strtok (str,pattern);
    while (word[i] != NULL)
    {
        count++;
	if(count>si-1)
	  {
	    word=(char **)checked_grow_alloc(word,&si);
	  }
        word[++i] = strtok (NULL, pattern);
    }
    *size=count;
    return word;
}
void checkerror(struct stack *myord,int n,char **str,int size,int index)
{
   static char const command_label[][10] = { "AND", "SEQUENCE", "OR", "PIPE" };
   struct command *node=checked_malloc(sizeof(struct command));
   switch (n)
     {
     case 0:
       node->type=AND_COMMAND;
       break;
     case 1:
       node->type=SEQUENCE_COMMAND;
       break;
     case 2:
       node->type=OR_COMMAND;
       break;
     case 3:
       node->type=PIPE_COMMAND;
       break;
     }
   //push(sta,node);
   push(myord,node);
   if(index+1<size)
     {
       index++;
       if(!isword(str[index])&&strcmp(str[index],"(")!=0)
	 {
	   fprintf(stderr,"After %s COMMAND is nothing!\n",command_label[n]);
	   exit(-1);
	 }
     }
   else
     {
       fprintf(stderr,"After %s COMMAND is nothing!\n",command_label[n]);
       exit(-1);
     }
}
command_t tree(char** str,int n)
{
    int size=n;
    int i=0;
    int j=0;
    char AND_command[]="&&";
    char SEQUENCE_command[]=";";
    char OR_command[]="||";
    char PIPE_command[]="|";
    char IN_PUT[]="<";
    char OUT_PUT[]=">";
    char OPEN[]="(";
    char CLOSE[]=")";
    int level=0;
    int count=0; //count the how many word ;
    struct stack mystack;
    struct stack myorderstack;
    mystack.item=0;
    myorderstack.item=0;
    int startindex=0;
    int countleft=0;
    for(i=0;i<size;i++)
    {
        if(isword(str[i]))
        {
	  if(count==0)
	    {
	      startindex=i;
	    }
          count++;
        }
        else
        {
            if(count>0)
            {                
                char **word=(char**)malloc((count+1)*sizeof(char*));
                for(j=0;j<count;j++)
                {
                    word[j]=str[startindex+j];
                }
                word[count]=NULL;
                command_t node=(command_t)checked_malloc(sizeof(struct command));
                node->u.word=word;
                node->type=SIMPLE_COMMAND;
		if(strcmp(str[i],IN_PUT)==0||strcmp(str[i],OUT_PUT)==0)
		    {
		      if(i+1<size)
			{
			  i++;
			  if(isword(str[i]))
			    {
			      if(strcmp(str[i-1],IN_PUT)==0)
				node->input=str[i];
			      if(strcmp(str[i-1],OUT_PUT)==0)
				node->output=str[i];
		  	    }
			  else
			    {
			      if(strcmp(str[i-1],IN_PUT)==0)
				fprintf(stderr,"follow < is not a word \n");
			      if(strcmp(str[i-1],OUT_PUT)==0)
				fprintf(stderr,"follow > is not a word \n");				
			      exit(-1);
			    }
			}
		      else
			{
			  if(strcmp(str[i-1],IN_PUT)==0)
			    fprintf(stderr,"there is not anything after < \n");
			  if(strcmp(str[i-1],OUT_PUT)==0)
			    fprintf(stderr,"there is not anything after > \n");
			  exit(-1);
			}
		    }
		if(i+1<size)
		{
		  i++;
		  if(strcmp(str[i],IN_PUT)==0||strcmp(str[i],OUT_PUT)==0)
		    {
		      if(i+1<size)
			{
			  i++;
			  if(isword(str[i]))
			    {
			      if(strcmp(str[i-1],IN_PUT)==0)
				node->input=str[i];
			      if(strcmp(str[i-1],OUT_PUT)==0)
				node->output=str[i];
		  	    }
			  else
			    {
			      if(strcmp(str[i-1],IN_PUT)==0)
				fprintf(stderr,"follow < is not a word \n");
			      if(strcmp(str[i-1],OUT_PUT)==0)
				fprintf(stderr,"follow > is not a word \n");				
			      exit(-1);
			    }
			}
		     else
			{
			  if(strcmp(str[i-1],IN_PUT)==0)
			    fprintf(stderr,"there is not anything after < \n");
			  if(strcmp(str[i-1],OUT_PUT)==0)
			    fprintf(stderr,"there is not anything after > \n");
			  exit(-1);
			}
		    }
		  else
		  {
		    i--;
		  }
		}
                count=0;
                push(&mystack,node);
	    }
	    checkstack(&myorderstack,&mystack); //check order of stack
            if(strcmp (str[i],AND_command) == 0)
            {     
	      checkerror(&myorderstack,0,str,size,i);//and push into stack;
            }
            else if(strcmp (str[i],OR_command) == 0)
            {
              checkerror(&myorderstack,2,str,size,i);
            }
            else if(strcmp (str[i],PIPE_command) == 0)
            {
              checkerror(&myorderstack,3,str,size,i); 
            }
            else if(strcmp (str[i],SEQUENCE_command) == 0)
            {
	      checkerror(&myorderstack,1,str,size,i);
            }
            else if(strcmp(str[i],OPEN)==0)
            {
	      countleft++;
	      command_t node=(command_t)checked_malloc(sizeof(struct command));
	      node->type=SIMPLE_COMMAND; //consider left 
	      push(&myorderstack,node);
	    }
            else if (strcmp(str[i],CLOSE)==0)
            {
	      countleft--;
	      if(countleft<0)
		{
		  fprintf(stderr,"subshell dont match");
		  exit(-1);
		}
	      command_t node=(command_t)checked_malloc(sizeof(struct command));
	      node->type=SUBSHELL_COMMAND;
	      node->input=NULL;
	      node->output=NULL;
	      command_t head;
	      if(empty(&mystack))
		{
		  fprintf(stderr,"subshell dont contain any command");
		  exit(-1);
		}
	      else
		{
		  while(top(&myorderstack)->type!=SIMPLE_COMMAND&&!empty(&myorderstack))
		     {
		       push(&mystack,top(&myorderstack));
		       pop(&myorderstack);
		       cleanstack(&mystack);
		     }
		  if(top(&myorderstack)->type==SIMPLE_COMMAND)
		    {
		      pop(&myorderstack);
		    }
		  else
		    {
		      fprintf(stderr,"no match subshell\n");
		      exit(-1);
		    }
		  head=top(&mystack);
		  node->u.subshell_command=head;
		  pop(&mystack);
		  push(&mystack,node);
		}
            }
        }
    }
    if(count!=0) //finish the rest word;
    {
        count=count+1;
        char **word=(char**)checked_malloc((count+1)*sizeof(char*));
        for(j=0;j<count;j++)
        {
            word[j]=str[startindex+j];
        }
	word[count]=NULL;
	command_t node=(command_t)checked_malloc(sizeof(struct command));
        node->u.word=word;
        node->type=SIMPLE_COMMAND;
        node->input=NULL;
        node->output=NULL;
        count=0;
        push(&mystack,node);
    }
    checkstack(&myorderstack,&mystack);
    while(!empty(&myorderstack))
      {
	push(&mystack,top(&myorderstack));
	pop(&myorderstack);
	cleanstack(&mystack);
      }
    command_t head=(command_t)checked_malloc(sizeof(struct command));
    head=top(&mystack);
    pop(&mystack);
    if(countleft!=0)
      {
	fprintf(stderr,"subshell () dont match");
	exit(-1);
      }
    return head;
    }
command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  int numberoflines=0;
  int numberofelement=0;
  int byte=get_next_byte(get_next_byte_argument);
  char *buffer;
  size_t size=1024;
  size_t count=0;
  buffer=(char *)checked_malloc(size*sizeof(char));
  while(byte!=EOF)
    {
      buffer[count]=(char)byte;
      count++;
      if(count>size-2)
	{
	  size=size*2;
	  buffer=(char*)checked_grow_alloc(buffer,&size);
	}
      if(byte<0)
	{
	  fprintf(stderr,"error for reading argument");
	  exit(-1);
	}
      byte=get_next_byte(get_next_byte_argument);
    }
  char* ha=check(buffer, &numberoflines);
  char pattern1[]=" "; //pattern space;
  char pattern2[]="\n"; //endline
  char **word2;
  char **word1;
  int i=0;
  word2=pr2(ha,pattern2,&size);
  int newsize=size;
  struct command_stream *head=checked_malloc(sizeof(struct command_stream));
  struct command_stream *mystream=head;
  for(i=0;i<newsize;i++)
     {
       if(word2[i]!=NULL)
	 {
	   word1=pr2(word2[i],pattern1,&size);
	   struct command_stream *node=checked_malloc(sizeof(struct command_stream));
	   node->comm=tree(word1,(int)size);
	   head->next=node;
	   head=head->next;
	 }
     }
  struct command_stream *it=mystream->next;
  return it; //tree head node;
}
int a=0;
struct command_stream* it; 
command_t
read_command_stream (command_stream_t s)
{
  command_t com;
  if(a==0)
    {
      it=s;
    }
  else
    {
      it=it->next;
    }
  if(it!=NULL)
    {
      com=it->comm;
    }
  else
    {
      return NULL;
    }
  a++;
  return com; 
}
