#ifndef print_keywords_h
#define print_keywords_h

#include "settings.h"
#include "global_variables.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



// tab_string is needed to be able switch between spaces "    " and tabs '\t'
char * tab_string = "    ";



inline int max(int a, int b){
    return a > b ? a : b;
}

#define KW_FUNCT_ARRAY_SIZE (3)

// struture to store text keyword text  space,tab,newline, function to execute  before/after printig the keyword
typedef struct t_kw_settings {
    int nl_before;
    int tab_before;
    int space_before;

    int nl_after;
    int tab_after;
    int space_after;

    char * text;

    int (*funct_before[KW_FUNCT_ARRAY_SIZE])();
    int (*funct_after [KW_FUNCT_ARRAY_SIZE])();

} t_kw_settings;



void debug_kw_settings(t_kw_settings s){
    extern FILE * yyout;
    fprintf(yyout,"\nspace_before %d , tab_before %d , nl_before %d , space_after %d , tab_after %d , nl_after %d\n , text %s "
           ,s.space_before,s.tab_before,s.nl_before,s.space_after,s.tab_after,s.nl_after, s.text);
    //printf("after %X %X %X\n", s.funct_after[0],s.funct_after[1],s.funct_after[2]);//debug string
    //printf("before %X %X %X\n", s.funct_before[0],s.funct_before[1],s.funct_before[2]);//debug string
}



int debug_p();// TODO : make separate .c and .h files



int new_line() {
    extern FILE * yyout;
    int i=0;
    fprintf(yyout,"\n");
    for(i=0;i<currindent;i++)
        fprintf(yyout,"%s",tab_string);
}



int sp_b(t_kw_settings s, int no_nl, int no_space ){
// sp_b - spacing before
    extern FILE * yyout;
    int i=0, minus_sp=0, minus_tb=0, minus_nl=0;
    static int prev_nl=0, prev_tab=0, prev_space=0; // settings saved from previously printed (key)word for spacing after it

    if(s.text[0]!='\0')
    {
        // spacing from last (key)word
        for(i=0; i < prev_nl; i++) fprintf(yyout,"\n");
        if(!s.nl_before){
            if( prev_nl > 0 ) for(i=0; i<currindent; i++)   fprintf(yyout,"%s",tab_string); // tabs - for indentation
            for(i=0; i < prev_tab   ; i++) fprintf(yyout,"%s",tab_string);
            for(i=0; i < prev_space ; i++) fprintf(yyout," ");
        }

        // spacing before (key)word
        for(i=0; i < s.nl_before - prev_nl ; i++) fprintf(yyout,"\n"); // new lines
        minus_tb=(s.nl_before?0:prev_tab);
        minus_sp=(s.nl_before?0:prev_space);

        if(s.nl_before > 0 ) for(i=0; i<currindent; i++) fprintf(yyout,"%s",tab_string); // tabs - for general indentation
        for(i=0; i < s.tab_before - minus_tb; i++)       fprintf(yyout,"%s",tab_string); // tabs
        for(i=0; i < s.space_before - minus_sp; i++)  fprintf(yyout," "); // spaces

        // save settings for next function call
        prev_nl    = s.nl_after;
        prev_tab   = s.tab_after;
        prev_space = s.space_after;
    }
    else
    {   // save settings, but not just overwrite
        prev_nl    += s.nl_after;
        prev_tab   += s.tab_after;
        prev_space = max(s.space_after, prev_space);
    }
}



void kw_print(t_kw_settings s){
    extern FILE * yyout;
    int i=0;
    for(i=0; i < KW_FUNCT_ARRAY_SIZE && s.funct_before[i] != NULL ; i++)
        s.funct_before[i]();

    sp_b(s, 0, 0);

    fprintf(yyout,"%s",s.text);
    for(i=0; i < KW_FUNCT_ARRAY_SIZE && s.funct_after[i] != NULL ; i++)
        s.funct_after[i]();
}



void echo_print(char * txt){
    extern FILE * yyout;
    int i=0, space_cnt=0, nl_cnt=0, length, nbr;
    char *tmp_txt;

    t_kw_settings s;
    s.nl_before=s.tab_before=s.space_before=s.nl_after=s.tab_after=s.space_after=0;
    
    //count blank characters at the end of the text
    length = strlen(txt);
    for(i=length-1; txt[i]==' '  && i>=0; i--) space_cnt++;
    for(          ; txt[i]=='\n' && i>=0; i--) nl_cnt++; // 'i=..' omited to continue from where last loop has finished

    // Prepare text for print (i is used with value set by last loop)
    nbr=i+1;
    s.text = (char*) malloc((nbr+1)*sizeof(char));
    strncpy(s.text, txt, nbr);
    s.text[nbr]='\0';

    // Spacing
    s.nl_after = nl_cnt;
    s.space_after = space_cnt;
    sp_b(s,0,0);

    // Print
    fprintf(yyout,"%s",s.text);
    free(s.text);
}



#define T_KW_SETTINGS_MACRO( NAME , ... ) \
    t_kw_settings NAME ;
#include "t_kw_settings_list.def"
#undef T_KW_SETTINGS_MACRO



void init_all_settings(){
    #define T_KW_SETTINGS_MACRO( NAME,nlb,tb,sb,nla,ta,sa,TEXT , fb1,fb2,fb3,fa1,fa2,fa3) \
        NAME.nl_before    = nlb;    \
        NAME.tab_before   = tb;     \
        NAME.space_before = sb;     \
                                    \
        NAME.nl_after     = nla;    \
        NAME.tab_after    = ta;     \
        NAME.space_after  = sa;     \
        NAME.text         = TEXT;   \
                                    \
        NAME.funct_before[0] = fb1; \
        NAME.funct_before[1] = fb2; \
        NAME.funct_before[2] = fb3; \
        NAME.funct_after [0] = fa1; \
        NAME.funct_after [1] = fa2; \
        NAME.funct_after [2] = fa3;
    #include "t_kw_settings_list.def"
    #undef T_KW_SETTINGS_MACRO
}


#endif
