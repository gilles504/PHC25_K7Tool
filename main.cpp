/*
  Name: 
  Copyright: 
  Author: G.Fetis
  Date: 26/12/07 19:12
  Description: a tool to convert PHC25 basic files to/from text files
*/

#include <windows.h>
#include "main.h"
#include <stdio.h>

/* Declare WindowsProcedure */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
/* Make the classname into a global variable */
char szClassName[] = "PHC25 K7 manipulation Tool";
HINSTANCE hThisInstance;

#define IDC_MAIN_TEXT   1001

unsigned char *g_phcFile=NULL;
int g_phcFileSize;

char * keyWords[]={
     "END",
     "FOR",
     "NEXT",
     "DATA",
     "INPUT",
     "DIM",
     "READ",
     "LET",
     "GOTO",
     "RUN",
     "IF",
     "RESTORE",
     "GOSUB",
     "RETURN",
     "REM",
     "STOP",
     "OUT",
     "ON",
     "LPRINT",
     "DEF",
     "POKE", //20
     "PRINT",
     "CONT",
     "LIST",
     "LLIST",
     "CLEAR",
     "COLOR",
     "PSET",
     "PRESET",
     "LINE",
     "PAINT",
     "SCREEN",
     "CLS",
     "LOCATE",
     "CONSOLE",
     "CLOAD",
     "CSAVE",
     "EXEC",
     "SOUND",
     "PLAY",
     "KEY",
     "LCOPY",
     "NEW",
     "CTON",
     "CTOFF",
     "SLOAD",
     "SSAVE",
     "ELSE", // 47
     "undoc1",
     "undoc2",
     "undoc3",
     "undoc4",
     "undoc5",
     "undoc6",
     "undoc7",
     "undoc8",
     "undoc9",
     "undoc10",
     "undoc11",
     "undoc12",
     "undoc13",
     "undoc14",
     "undoc15",
     "undoc16",
     "undoc17",
     "undoc18",
     "TAB(",
     "TO",
     "FN",
     "SPC(",
     "INKEY$",
     "THEN",
     "NOT", // 54 = x36
     "STEP",
     "+","-","*","/","^","AND","OR",">","=","<",
     "SGN", // 66
     "INT",
     "ABS",
     "USR",
     "FRE",
     "INP",
     "LPOS", // 72 = x48
     "POS",
     "SQR",
     "RND",
     "LOG",
     "EXP",
     "COS",
     "SIN",
     "TAN",
     "PEEK",
     "LEN",
     "SCRIN",
     "STR$", // 84
     "VAL",
     "ASC",
     "CHR$",
     "LEFT$",
     "RIGHT$",
     "MID$",
     "POINT",
     "CSRLIN",
     "STICK",
     "STRIG",
     "TIME"
};

#define NB_KEYWORDS (sizeof(keyWords)/sizeof(char*))

/*
 *  PHC to TXT convert stuff
 */
BOOL DecodePHCFile(char *fname) {
     FILE * f;
     int i=0;
     int line=0;
     int lineval=0;
     f=fopen(fname,"w");
     if (f==NULL) return FALSE;

// finds leading A5
     while (g_phcFile[i]!=0xA5) {
           i++;
           if (i>=g_phcFileSize) {fclose(f); return FALSE;}      
     }

// skips leading A5
     while (g_phcFile[i]==0xA5) {
           i++;
           if (i>=g_phcFileSize) {fclose(f); return FALSE;}      
     }

// skips filename
     i+=6;
     
     line=i;
     while ((g_phcFile[line]!=0x00) || (g_phcFile[line+1]!=0x00)) {
           if (line+1>=g_phcFileSize) {fclose(f); return FALSE;}
           line++;      
     }

     while ((g_phcFile[line]!=0xff) || (g_phcFile[line+1]!=0xff)) {
           if (line+1>=g_phcFileSize) {fclose(f); return FALSE;}  
           line++;    
     }

     line-=3;
          
     while (g_phcFile[i]!=0x00) {
           if (i>=g_phcFileSize) {fclose(f); return FALSE;}      
// decode line number
           lineval=(g_phcFile[line] | (g_phcFile[line+1]<<8));
           line-=4;
           fprintf(f,"%d ",lineval);
           
// reads the line
         while (g_phcFile[i]!=0x00) {
               if (i>=g_phcFileSize) {fclose(f); return FALSE;} 
               if (g_phcFile[i]>=0x80) 
                   if (g_phcFile[i]>=0x80+NB_KEYWORDS)
                      fprintf(f,"\\x%02x",g_phcFile[i]);
                   else
                   fprintf(f,"%s ",keyWords[g_phcFile[i]-0x80]);
               else
                   if (g_phcFile[i]>=0x20)
                      if (g_phcFile[i]=='\\')
                         fprintf(f,"\\\\",g_phcFile[i]);     
                      else
                         fprintf(f,"%c",g_phcFile[i]);     
                   else
                         fprintf(f,"\\x%02x",g_phcFile[i]);
               i++;
         }
         fprintf(f,"\n");     
         fflush(f);
         i++;
     }

     fclose(f);
     return TRUE;
}

BOOL ReadPHCFile(char *fname) {
     FILE * f;
     f=fopen(fname,"rb");
     if (f==NULL) return FALSE;
     
     fseek(f,0,SEEK_END);
     g_phcFileSize=ftell(f);
     fseek(f,0,SEEK_SET);
     
     g_phcFile=(unsigned char *)malloc(g_phcFileSize*sizeof(char));
     
     fread(g_phcFile,g_phcFileSize,sizeof(char),f);
          
     fclose(f);
     return TRUE;
}

BOOL DoPHC2TXT(HWND hwnd)
{
   OPENFILENAME ofn;
   char szFileName[MAX_PATH];

   ZeroMemory(&ofn, sizeof(ofn));
   szFileName[0] = 0;

   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFilter = "phc files (*.phc)\0*.phc\0All Files (*.*)\0*.*\0\0";
   ofn.lpstrFile = szFileName;
   ofn.nMaxFile = MAX_PATH;
   ofn.lpstrDefExt = "phc";

   ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   if(GetOpenFileName(&ofn))
   {
// ajouter chargement et décodage fichier PHC ici
           if (ReadPHCFile(szFileName)==FALSE) {
                  MessageBox( hwnd, (LPSTR) "Bad PHC file",
                              (LPSTR) szClassName,
                              MB_ICONINFORMATION | MB_OK );
                                               
              return FALSE;
              }
   }
   else
       return FALSE;


   ofn.lpstrFilter = "txt files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0";
   ofn.lpstrDefExt = "txt";

   ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
         OFN_OVERWRITEPROMPT;
         
   if(GetSaveFileName(&ofn))
   {
// sauvegarder fichier TXT
               if (DecodePHCFile(szFileName)==FALSE) {
                  MessageBox( hwnd, (LPSTR) "Bad PHC file",
                              (LPSTR) szClassName,
                              MB_ICONINFORMATION | MB_OK );
                                               
              return FALSE;
              }
   }
   else
       return FALSE;
   
   return TRUE;
}


/*
 *  TXT to PHC convert stuff
 */
unsigned char *g_txtFile=NULL;
int g_txtFileSize;

char phc_buf[65535];
int phc_buf_size=0;
int phc_nb_lines=0;

void push_phc(char c) {
     if (phc_buf_size<65535) {
        phc_buf[phc_buf_size]=c;
        phc_buf_size++;
     }
}

char phc_buf2[65535];
int phc_buf2_size=0;

void push_phc2(char c) {
     if (phc_buf2_size<65535) {
        phc_buf2[phc_buf2_size]=c;
        phc_buf2_size++;
     }
}

int num_line;
char temp_line[65535];
char temp_line_size=0;

/*
  Algo de conversion:
       identifie une ligne complete
       decode partie numero de ligne => push 16 bits dans phc_buf;
       decode les mots-clef dans les parties hors "" => temp_line
              gestion des cas \\ et \x
       push de temp_line dans phc_buf
       
       en fin (resultat dans :
       push 0xA5...
       push du nom du fic
       push de chaque ligne
       push de 00 00
       push de la taille 
       push de chaque ligne en ordre inverse
       
 */
BOOL isNumber(char c) {
     if ((c>='0') && (c<='9'))
        return TRUE;
     else
         return FALSE;
}

#define myupper(c) ((( ((c)>='a') && ((c)<='z') )?((c)-'a'+'A'):(c) ))

int checkKeyWord(char *p) {
    int i,j;
    for (i=0;i<NB_KEYWORDS;i++) {
        for (j=0;j<strlen(keyWords[i]);j++) {
            if ((*(p+j))=='\0') break;
            if (myupper(*(p+j))!=keyWords[i][j]) break;
            }
            if (j==strlen(keyWords[i]))
                return i;
        }
    return -1;
    
}
 
BOOL DecodeTXTFile(char *fname) {
     char logFname[MAX_PATH];
     FILE *log;
     FILE *f;
     unsigned char *start_p=g_txtFile;
     unsigned char *end_p=NULL;
     int i;
     int v;
     
     phc_nb_lines=0;
     
     strcpy(logFname,fname);
     strcat(logFname,".log");

     log=fopen(logFname,"w");
     if (log==NULL)
        return FALSE;
     
     while (1) {
     // find first number
     while ((start_p<(g_txtFile+g_txtFileSize)) && (!isNumber(*start_p))) {
           start_p++;
           }
           
     if (start_p>=(g_txtFile+g_txtFileSize)) break;
     
     end_p=start_p;
     // find first number
     while ((end_p<(g_txtFile+g_txtFileSize)) && (isNumber(*end_p))) {
           end_p++;
           }

     if (end_p>=(g_txtFile+g_txtFileSize)) break;

           
     strncpy(temp_line,(char *)start_p,(end_p-start_p));
     temp_line[end_p-start_p]='\0';
     sscanf(temp_line,"%d",&num_line);
     fprintf(log,"found line %d ",num_line);
     push_phc((num_line&0xFF00)>>8);
     push_phc(num_line&0xFF);
     
     
     start_p=end_p;

     // find end of line
     while ((end_p<(g_txtFile+g_txtFileSize)) && (*end_p!=0x0D)) {
           end_p++;
           }

     strncpy(temp_line,(char *)start_p,(end_p-start_p));
     temp_line[end_p-start_p]='\0';
     temp_line_size=(end_p-start_p);
     
     fprintf(log,"<%s>\n",temp_line);

     // line decode bloc
     {
        BOOL isString=FALSE;
        BOOL isComment=FALSE;
        BOOL followKeyword=TRUE;
        
        for (i=0;i<strlen(temp_line);i++) {
          if ((!isString) && (!isComment))
             if ((v=checkKeyWord(&temp_line[i]))!=-1) {
                fprintf(log,"[%s]",keyWords[v]);
                push_phc(v+0x80);
                i+=(strlen(keyWords[v])-1);
                if (v==14)
                   isComment=TRUE;
                followKeyword=TRUE;
                continue;
             }
             
          if (followKeyword && (temp_line[i]==' ')) {
             followKeyword=FALSE;
             continue;
          }
          if (temp_line[i]=='"') {
             if (isString) {
                isString=FALSE;
                }
             else {
                 isString=TRUE;
             }
             fprintf(log,"[%02x=%c]",'"','"');
             push_phc('"');
             followKeyword=FALSE;
             continue;
          }

          if (temp_line[i]=='\\') {
             if (temp_line[i+1]=='x') {
                char hex[3];
                int val;
                hex[0]=temp_line[i+2];
                hex[1]=temp_line[i+3];
                hex[2]='\0';
                sscanf(hex,"%x",&val);
                i+=3;
                fprintf(log,"[%02x]",val);
                push_phc(val);
                followKeyword=FALSE;
                continue;
             }
             else
             if (temp_line[i+1]=='\\') {
                i++;
               fprintf(log,"[%02x=%c]",'\\','\\');
               push_phc('\\');
               followKeyword=FALSE;
               continue;
             }
          } // of for (i=0...
          fprintf(log,"[%02x=%c]",temp_line[i],temp_line[i]);
          push_phc(temp_line[i]);
          followKeyword=FALSE;
          

        }
     } // of line decode bloc
     fprintf(log,"\n");
     push_phc('\0');
     phc_nb_lines++;
     start_p=end_p;


     } // of while (1)

     {
       int off=0;
       int off_dest=0;
       int off_line_info=phc_buf_size+phc_nb_lines*2+2+1;
       
       int len;
       unsigned int line_num;
       for (i=0;i<phc_nb_lines;i++) {
          // reads line num
          line_num=((((unsigned char)phc_buf[off])<<8) | ((unsigned char)phc_buf[off+1]))&0xFFFF;
          off+=2;
          // we can use strlen here since line is null terminated
          len=strlen(&phc_buf[off]);
          
          fprintf(log,"Line %d len %d off %d\n",line_num,len,off);
          // now build the final buffer (without header)
          
          strncpy(&phc_buf2[off_dest],&phc_buf[off],len+1);
          
          
          off_line_info-=4;
          off+=len+1;
          off_dest+=len+1;
       }
     }
     f=fopen(fname,"wb");
     if (f==NULL) return FALSE;

	 char header[]={0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,'T','E','S','T',0,0};
     fwrite(header,sizeof(header),sizeof(char),f);
	 
     fwrite(phc_buf,phc_buf_size,sizeof(char),f);
     
     fclose(f);
     fclose(log);
     return TRUE;
}

BOOL ReadTXTFile(char *fname) {
     FILE * f;
     f=fopen(fname,"rb");
     if (f==NULL) return FALSE;
     
     fseek(f,0,SEEK_END);
     g_txtFileSize=ftell(f);
     fseek(f,0,SEEK_SET);
     
     g_txtFile=(unsigned char *)malloc(g_txtFileSize*sizeof(char));
     
     if (g_txtFile==NULL) return FALSE;
     
     fread(g_txtFile,g_txtFileSize,sizeof(char),f);
          
     fclose(f);
     return TRUE;
}

BOOL DoTXT2PHC(HWND hwnd)
{
   OPENFILENAME ofn;
   char szFileName[MAX_PATH];

   ZeroMemory(&ofn, sizeof(ofn));
   szFileName[0] = 0;

   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFilter = "txt files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0";
   ofn.lpstrFile = szFileName;
   ofn.nMaxFile = MAX_PATH;
   ofn.lpstrDefExt = "txt";

   ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   if(GetOpenFileName(&ofn))
   {
// ajouter chargement et décodage fichier PHC ici
           if (ReadTXTFile(szFileName)==FALSE) {
                  MessageBox( hwnd, (LPSTR) "Bad TXT file",
                              (LPSTR) szClassName,
                              MB_ICONINFORMATION | MB_OK );
                                               
              return FALSE;
              }
   }
   else
       return FALSE;


   ofn.lpstrFilter = "phc files (*.phc)\0*.phc\0All Files (*.*)\0*.*\0\0";
   ofn.lpstrDefExt = "phc";

   ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
         OFN_OVERWRITEPROMPT;
         
   if(GetSaveFileName(&ofn))
   {
// sauvegarder fichier TXT
               if (DecodeTXTFile(szFileName)==FALSE) {
                  MessageBox( hwnd, (LPSTR) "Bad TXT file",
                              (LPSTR) szClassName,
                              MB_ICONINFORMATION | MB_OK );
                                               
              return FALSE;
              }
   }
   else
       return FALSE;
   
   return TRUE;
}


int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)

{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application is saved */
    WNDCLASSEX wincl;        /* Datastructure for the windowclass */
    HMENU menu;              /* Handle of the menu */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Ctach double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mousepointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL; /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use lightgray as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);

    /* Register the window class, if fail quit the program */
    if(!RegisterClassEx(&wincl)) return 0;

    /* The class is registered, lets create the program*/
    hwnd = CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "PHC25 K7 manipulation Tool",         /* Title Text */
           WS_OVERLAPPEDWINDOW, /* defaultwindow */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window end up on the screen */
           320,                 /* The programs width */
           200,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a childwindow to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow(hwnd, nFunsterStil);

    menu = LoadMenu(hThisInstance, MAKEINTRESOURCE(ID_MENU));
    SetMenu(hwnd, menu);

    /* Run the nessageloop. It will run until GetMessage( ) returns 0 */
    while(GetMessage(&messages, NULL, 0, 0))
    {
           /* Send message to WindowProcedure */
           DispatchMessage(&messages);
    }

    /* The program returvalue is 0 - The value that PostQuitMessage( ) gave */
    return messages.wParam;
}

/* This function is called by the Windowsfunction DispatchMessage( ) */
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
       case WM_COMMAND:
           switch( wParam )
           {
             case IDM_FILEPHC2TXT:
                  DoPHC2TXT(hwnd);
                  return 0;
             case IDM_FILETXT2PHC:
                  DoTXT2PHC(hwnd);
                  return 0;

             case IDM_FILEEXIT:
                  SendMessage( hwnd, WM_CLOSE, 0, 0L );
                  return 0;

             case IDM_HELPABOUT:
                  MessageBox (NULL, "PHC25 K7 Tool G.Fetis 2007" , "About", 1);
                  return 0;

           }
           break;

      case WM_CLOSE:
           DestroyWindow( hwnd );
           return 0;

      case WM_DESTROY:
           PostQuitMessage (0);
           return 0;

      break;
      default:                   /* for messages that we don't deal with */
      return DefWindowProc(hwnd, message, wParam, lParam);
   }
  return 0;
}
