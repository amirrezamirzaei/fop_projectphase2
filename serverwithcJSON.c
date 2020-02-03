#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "cJSON.c"
#include "cJSON_Utils.h"
#include <winsock2.h>
#define MAX 80
#define PORT 12345
#define SA struct sockaddr
cJSON *membersid,*memberauthtoken,*channelmember,*memberactivechannel,*memberrefreshid;
char input[10000],authtoken[33];
int server_socket, client_socket;
void addmembertochannel(char username[],char thischannel[]);
void removeuser(char username[],char thischannel[]);
void sendjson(char type[],char content[]);
void listenserver();
void registeruser();
void authtokengenerator();
void loginuser();
void logout();
void createchannel();
void channelmembers();
void joinchannel();
void leavechannel();
void sendmessages();
void refresh();
void searchmember();
void searchmessage();
int doesithavesubstring(char *a,char *b);
int main()
{
    mkdir("user");
    mkdir("channel");
    membersid=cJSON_CreateObject();
    memberauthtoken=cJSON_CreateObject();
    memberrefreshid=cJSON_CreateObject();
    memberactivechannel=cJSON_CreateObject();
    channelmember=cJSON_CreateObject();
     while(1)
    {
        listenserver();
        memset(input, 0, sizeof(input));
        recv(client_socket, input, sizeof(input), 0);
        printf("FROM CLIENT:%s\n",input);
        char tmp1[100],tmp2[100];
    sscanf(input,"%s %s",tmp1,tmp2);
    if(strcmp(tmp1,"register")==0)
    {
        registeruser();
        continue;
    }
      if(strcmp(tmp1,"login")==0)
    {
        loginuser();
        continue;
    }
        if  (strcmp(tmp1,"create")==0)
    {
        createchannel();
        continue;
    }
    if(strcmp(tmp1,"logout")==0)
    {
        logout();
        continue;
    }
     if(strcmp(tmp1,"join")==0)
    {
        joinchannel();
        continue;
    }
     if(strcmp(tmp1,"leave")==0)
    {
        leavechannel();
        continue;
    }
      if(strcmp(tmp1,"send")==0)
    {
        sendmessages();
        continue;
    }
     if(strcmp(tmp1,"refresh")==0)
    {
        refresh();
        continue;
    }
      if(strcmp(tmp1,"channel")==0)
    {
        channelmembers();
        continue;
    }
    if(strcmp(tmp1,"searchmember")==0)
    {
        searchmember();
        continue;
    }
    if(strcmp(tmp1,"searchmessage")==0)
    {
        searchmessage();
        continue;
    }
    }
    return 0;
}
void removeuser(char username[],char thischannel[])
{
    int i,j;
   cJSON *ar=cJSON_GetObjectItemCaseSensitive(channelmember,thischannel);
   for(i=0;i<cJSON_GetArraySize(ar);i++)
   {
       char *a=cJSON_PrintUnformatted(cJSON_GetArrayItem(ar,i));
       printf("from removeuser:%s\n",a);
       char username1[100];
       for(j=1;j<strlen(a);j++)
       username1[j-1]=a[j];
       username1[j-2]='\0';
       if(strcmp(username,username1)==0)
        break;
   }
  cJSON_DeleteItemFromArray(ar,i);
   char *out=cJSON_PrintUnformatted(ar);

   printf("from removeuserasi:%s\n",out);
}
void addmembertochannel(char username[],char thischannel[])
{
char *existchannel;
   existchannel=cJSON_PrintUnformatted(cJSON_GetObjectItemCaseSensitive(channelmember,thischannel));
   if(existchannel==NULL)
   {
        cJSON *ar;
        ar=cJSON_CreateArray();
        cJSON_AddItemToArray(ar,cJSON_CreateString(username));
        cJSON_AddItemReferenceToObject(channelmember,thischannel,ar);
        char *out=cJSON_PrintUnformatted(channelmember);
    printf("----%s====",out);
        return;
   }
    cJSON *ar=cJSON_GetObjectItemCaseSensitive(channelmember,thischannel);
    cJSON_AddItemToArray(ar,cJSON_CreateString(username));
    char *out=cJSON_PrintUnformatted(channelmember);
    printf("----%s====",out);
}
void listenserver() //copy shode az kelas kargah shabake
{
    closesocket(client_socket);
    closesocket(server_socket);
    struct sockaddr_in server, client;
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d", err);
        return;
    }
    // Create and verify socket
    server_socket = socket(AF_INET, SOCK_STREAM, 6);
	if (server_socket == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d", WSAGetLastError() );
    else
        printf("Socket successfully created.");

    // Assign IP and port
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);
    // Bind newly created socket to given IP and verify
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
    {
        printf("Socket binding failed.");
        exit(0);
    }
    else
        printf("Socket successfully bound.");

    // Now server is ready to listen and verify
    if ((listen(server_socket, 5)) != 0)
    {
        printf("Listen failed.");
        exit(0);
    }
    else
        printf("Server listening.");

    // Accept the data packet from client and verify
    int len = sizeof(client);
    client_socket = accept(server_socket, (SA *)&client, &len);
    if (client_socket < 0)
    {
        printf("Server accceptance failed.");
        exit(0);
    }
    else
        printf("Server acccepted the client.\n");
}
void sendjson(char type[],char content[])
{
    char *out;
    cJSON *output;
    output = cJSON_CreateObject();
    cJSON_AddItemToObject(output, "type", cJSON_CreateString(type));
    cJSON_AddItemToObject(output, "content", cJSON_CreateString(content));
    out=cJSON_PrintUnformatted(output);
    printf("\nTO CLIENT:%s",out);
    send(client_socket, out, strlen(out), 0);
    cJSON_Delete(output);
}
void registeruser()
{
 char tmp1[100],tmp2[100],tmp3[110];
  sscanf(input,"%*s %s %s",tmp1,tmp2);
  tmp1[strlen(tmp1)-1]='\0';
  FILE *tmpuser;
  strcpy(tmp3,"user\\");
  strcat(tmp3,tmp1);
  strcat(tmp3,".json");
  tmpuser=fopen(tmp3,"r");
  if(tmpuser!=NULL)
  {
      sendjson("Error","");
      fclose(tmpuser);
      return;
  }
  if(tmpuser==NULL)
  {
  FILE *user;
  strcpy(tmp3,"user\\");
  strcat(tmp3,tmp1);
  strcat(tmp3,".json");
  user=fopen(tmp3,"w");
  fprintf(user,"%s\n%s",tmp1,tmp2);
  fclose(user);
  }
  sendjson("Successful","");
  cJSON_AddItemToObject(membersid,tmp1,cJSON_CreateString("0"));
  fclose(tmpuser);
}
void authtokengenerator()
{
    for(int i=0;i<32;i++)
    {
        int a=rand()%3;
        int c;
        if(a==0)
        {
            c=rand()%26+65;
            authtoken[i]=c;
        }
         if(a==1)
        {
            c=rand()%26+97;
            authtoken[i]=c;
        }
        if(a==2)
        {
            c=rand()%10+48;
            authtoken[i]=c;
        }
    }
}
void loginuser()
{
  char tmp1[100],tmp2[100],tmp3[110];
  FILE *tmpuser;
  sscanf(input,"%*s %s %s",tmp1,tmp2);
  tmp1[strlen(tmp1)-1]='\0';
  strcpy(tmp3,"user\\");
  strcat(tmp3,tmp1);
  strcat(tmp3,".json");
  tmpuser=fopen(tmp3,"r+");
  if(tmpuser==NULL)
  {
      sendjson("Error","");
      fclose(tmpuser);
      return;
  }
  char password[100];
  fscanf(tmpuser,"%*s %s",password);
  if(strcmp(password,tmp2)!=0)
  {
      sendjson("Error","wrong password");
      fclose(tmpuser);
      return;
  }
  printf("%s",tmp1);
   char *isonline;
   isonline=cJSON_PrintUnformatted(cJSON_GetObjectItemCaseSensitive(membersid,tmp1));
   if(isonline==NULL)
   {
        cJSON_AddItemToObject(membersid,tmp1,cJSON_CreateString("1"));
   }
   else
   if(strcmp(isonline,"\"1\"")==0)
   {
     sendjson("Error","user already signed in");
      return;
   }
   cJSON_DeleteItemFromObject(membersid,tmp1);
   cJSON_AddItemToObject(membersid,tmp1,cJSON_CreateString("1"));
  authtokengenerator();
  authtoken[32]='\0';
    cJSON *output;
    output = cJSON_CreateObject();
    cJSON_AddItemToObject(output, "type", cJSON_CreateString("AuthToken"));
    cJSON_AddItemToObject(output, "content", cJSON_CreateString(authtoken));
   char *out=cJSON_PrintUnformatted(output);
   printf("%s\n",out);
    send(client_socket, out, strlen(out), 0);
     cJSON_Delete(output);
   cJSON_AddItemToObject(memberauthtoken,authtoken,cJSON_CreateString(tmp1));
   out=cJSON_PrintUnformatted(memberauthtoken);
   printf("%s\n",out);
}
void logout()
{
   char tmp1[32];
   int i;
  sscanf(input,"%*s %s",tmp1);
  cJSON *check=cJSON_GetObjectItemCaseSensitive(memberauthtoken,tmp1);
  if(check==NULL)
  {
    sendjson("Error","auth token doesnt match");
    return;
  }
  char *a=cJSON_PrintUnformatted(check);
  char username[100];
  for(i=1;i<strlen(a);i++)
    username[i-1]=a[i];
  username[i-2]='\0';
  printf("\n%s\n ",username);
  sendjson("Successful","");
  cJSON_DeleteItemFromObject(membersid,username);
  cJSON_AddItemToObject(membersid,username,cJSON_CreateString("0"));
  a=cJSON_PrintUnformatted(membersid);
  printf("\n%s",a);
  cJSON_DeleteItemFromObject(memberauthtoken,tmp1);
  return;
}
void createchannel()
{
   char tmp1[100],tmp2[100],tmp3[110];
  sscanf(input,"%*s %*s %s %s",tmp1,tmp2);
  cJSON *check=cJSON_GetObjectItemCaseSensitive(memberauthtoken,tmp2);
  if(check==NULL)
  {
    sendjson("Error","auth token doesnt match");
    return;
  }
   char *a=cJSON_PrintUnformatted(check);
  char username[100];
  int i;
  for(i=1;i<strlen(a);i++)
    username[i-1]=a[i];
  username[i-2]='\0';
  printf("\n%s\n ",username);


  tmp1[strlen(tmp1)-1]='\0';
  FILE *channel;
  strcpy(tmp3,"channel\\");
  strcat(tmp3,tmp1);
  strcat(tmp3,".json");
  channel=fopen(tmp3,"r");
  if(channel!=NULL)
  {
      sendjson("Error","channel with this name already exist");
      fclose(channel);
      return;
  }
  if(channel==NULL)
  {
  strcpy(tmp3,"channel\\");
  strcat(tmp3,tmp1);
  strcat(tmp3,".json");
  channel=fopen(tmp3,"w");
  char gar[100],gar1[100];
  strcpy(gar,username);
  strcat(gar," created channel");
    strcpy(gar1,username);
    strcat(gar1," joined channel");
    cJSON *test,*ar,*bigitem,*test1;
    ar=cJSON_CreateArray();
    test1 = cJSON_CreateObject();
    cJSON_AddItemToObject(test1, "sender", cJSON_CreateString("server"));
    cJSON_AddItemToObject(test1, "content", cJSON_CreateString(gar1));
    test = cJSON_CreateObject();
    cJSON_AddItemToObject(test, "sender", cJSON_CreateString("server"));
    cJSON_AddItemToObject(test, "content", cJSON_CreateString(gar));
    cJSON_AddItemToArray(ar,test);
     cJSON_AddItemToArray(ar,test1);
     bigitem=cJSON_CreateObject();
     cJSON_AddItemToObject(bigitem,"messages",ar);
     char *a=cJSON_PrintUnformatted(bigitem);
     printf("\n%s",a);
     fprintf(channel,"%s",a);
     fclose(channel);
     sendjson("Succesful","");
     addmembertochannel(username,tmp1);
    cJSON_DeleteItemFromObjectCaseSensitive(memberactivechannel,username);
    cJSON_AddItemToObject(memberactivechannel,username,cJSON_CreateString(tmp1));
    cJSON_DeleteItemFromObjectCaseSensitive(memberrefreshid,username);
    cJSON_AddItemToObject(memberrefreshid,username,cJSON_CreateNumber(0));
  }
}
void channelmembers()
{
   char tmp1[32];
  sscanf(input,"%*s %*s %s",tmp1);
 cJSON *check=cJSON_GetObjectItemCaseSensitive(memberauthtoken,tmp1);
  if(check==NULL)
  {
      puts("wdjwifdjwi");
    sendjson("Error","auth token doesnt match");
    return;
  }
   char *a=cJSON_PrintUnformatted(check);
   printf("--%s",a);
  char username[100],channelname[100];
  int i;
  for(i=1;i<strlen(a);i++)
    username[i-1]=a[i];
  username[i-2]='\0';
  printf("\n%s\n ",username);
  check=cJSON_GetObjectItemCaseSensitive(memberactivechannel,username);
   if(check==NULL)
  {
    sendjson("Error","not joined in any channel");
    return;
  }
  a=cJSON_PrintUnformatted(check);
  for(i=1;i<strlen(a);i++)
    channelname[i-1]=a[i];
  channelname[i-2]='\0';
  printf("\n%s\n ",channelname);

   //existchannel=cJSON_PrintUnformatted(cJSON_GetObjectItemCaseSensitive(channelsmembers,thischannel));
  cJSON *ar=cJSON_GetObjectItemCaseSensitive(channelmember,channelname);
  cJSON *bigitem=cJSON_CreateObject();
  cJSON_AddItemToObject(bigitem,"type",cJSON_CreateString("list"));
  cJSON_AddItemReferenceToObject(bigitem,"content",ar);
   // cJSON_AddItemToObject(bigitem,"content",ar);
    char *out=cJSON_PrintUnformatted(bigitem);
    printf("%s",out);
    send(client_socket, out, strlen(out), 0);
}
void joinchannel()
{
    char tmp1[100],tmp2[100],tmp3[110];
  sscanf(input,"%*s %*s %s %s",tmp1,tmp2);
   cJSON *check=cJSON_GetObjectItemCaseSensitive(memberauthtoken,tmp2);
  if(check==NULL)
  {
    sendjson("Error","auth token doesnt match");
    return;
  }
   char *a=cJSON_PrintUnformatted(check);
   printf("--%s",a);
  char username[100],channelname[100];
  int i;
  for(i=1;i<strlen(a);i++)
    username[i-1]=a[i];
  username[i-2]='\0';
  printf("\n%s\n ",username);
  tmp1[strlen(tmp1)-1]='\0';
  FILE *channel;
  strcpy(tmp3,"channel\\");
  strcat(tmp3,tmp1);
  strcat(tmp3,".json");
  channel=fopen(tmp3,"r");
  if(channel==NULL)
  {
      sendjson("Error","channel with this name doesnt exist");
      fclose(channel);
      return;
  }
    channel=fopen(tmp3,"r");

    char linescan[2000],tmpp[200];
    strcpy(tmpp,username);
    strcat(tmpp," joined.");
    fgets(linescan,2000,channel);
    printf("\n%s\n",linescan);
    cJSON *output=cJSON_Parse(linescan);
    cJSON *ar=cJSON_GetObjectItemCaseSensitive(output,"messages");
    cJSON *newmessage=cJSON_CreateObject();
    cJSON_AddItemToObject(newmessage,"sender",cJSON_CreateString("server"));
    cJSON_AddItemToObject(newmessage,"content",cJSON_CreateString(tmpp));
    cJSON_AddItemToArray(ar,newmessage);
    char *out=cJSON_PrintUnformatted(output);
    printf("/n%s\n",out);
    fclose(channel);
     channel=fopen(tmp3,"w");
     fprintf(channel,"%s",out);
    fclose(channel);
    sendjson("Successful","");
     addmembertochannel(username,tmp1);
    cJSON_DeleteItemFromObjectCaseSensitive(memberactivechannel,username);
    cJSON_AddItemToObject(memberactivechannel,username,cJSON_CreateString(tmp1));
    cJSON_DeleteItemFromObjectCaseSensitive(memberrefreshid,username);
    cJSON_AddItemToObject(memberrefreshid,username,cJSON_CreateNumber(0));
}
void leavechannel()
{
    char tmp1[32],tmp3[100];
  sscanf(input,"%*s %s",tmp1);
   cJSON *check=cJSON_GetObjectItemCaseSensitive(memberauthtoken,tmp1);
  if(check==NULL)
  {
      puts("wdjwifdjwi");
    sendjson("Error","auth token doesnt match");
    return;
  }
   char *a=cJSON_PrintUnformatted(check);
   printf("--%s",a);
  char username[100],channelname[100];
  int i;
  for(i=1;i<strlen(a);i++)
    username[i-1]=a[i];
  username[i-2]='\0';
  printf("\n%s\n ",username);

   check=cJSON_GetObjectItemCaseSensitive(memberactivechannel,username);
   if(check==NULL)
  {
    sendjson("Error","not joined in any channel");
    return;
  }
  a=cJSON_PrintUnformatted(check);
  for(i=1;i<strlen(a);i++)
    channelname[i-1]=a[i];
  channelname[i-2]='\0';
  printf("\n%s\n ",channelname);
    strcpy(tmp3,"channel\\");
  strcat(tmp3,channelname);
  strcat(tmp3,".json");
    FILE *openchannel=fopen(tmp3,"r");
 char linescan[2000],tmpp[200];
    strcpy(tmpp,username);
    strcat(tmpp," left.");
    fgets(linescan,2000,openchannel);
    printf("\n%s\n",linescan);
    cJSON *output=cJSON_Parse(linescan);
    cJSON *ar=cJSON_GetObjectItemCaseSensitive(output,"messages");
    cJSON *newmessage=cJSON_CreateObject();
    cJSON_AddItemToObject(newmessage,"sender",cJSON_CreateString("server"));
    cJSON_AddItemToObject(newmessage,"content",cJSON_CreateString(tmpp));
    cJSON_AddItemToArray(ar,newmessage);
    char *out=cJSON_PrintUnformatted(output);
    printf("/n%s\n",out);
    fclose(openchannel);
    openchannel=fopen(tmp3,"w");
    fprintf(openchannel,"%s",out);
    fclose(openchannel);
    removeuser(username,channelname);
    sendjson("Successful","");
    cJSON_DeleteItemFromObjectCaseSensitive(memberactivechannel,username);
    cJSON_DeleteItemFromObjectCaseSensitive(memberrefreshid,username);
}
void sendmessages()
{
    int i,j;
    char tmp3[100];
    char message[200],tmptoken[32];
    puts("h");
    for( i=5;;i++)
    {
        if(input[i]!=',')
        message[i-5]=input[i];
        else
        break;
    }
    message[i-5]='\0';
    for(j=0;j<32;j++)
    tmp3[j]=input[i+j+2];
    tmp3[32]='\0';
     cJSON *check=cJSON_GetObjectItemCaseSensitive(memberauthtoken,tmp3);
  if(check==NULL)
  {
    sendjson("Error","auth token doesnt match");
    return;
  }
   char *a=cJSON_PrintUnformatted(check);
   printf("--%s",a);
  char username[100],channelname[100];
  for(i=1;i<strlen(a);i++)
    username[i-1]=a[i];
  username[i-2]='\0';
  printf("\n%s\n ",username);
    check=cJSON_GetObjectItemCaseSensitive(memberactivechannel,username);
   if(check==NULL)
  {
    sendjson("Error","not joined in any channel");
    return;
  }
  a=cJSON_PrintUnformatted(check);
  for(i=1;i<strlen(a);i++)
    channelname[i-1]=a[i];
  channelname[i-2]='\0';
  printf("\n%s\n ",channelname);
  strcpy(tmp3,"channel\\");
  strcat(tmp3,channelname);
  strcat(tmp3,".json");
    FILE *openchannel=fopen(tmp3,"r");
 char linescan[2000],tmpp[200];
    fgets(linescan,2000,openchannel);
    printf("\n---%s----\n",linescan);
    cJSON *output=cJSON_Parse(linescan);
    cJSON *ar=cJSON_GetObjectItemCaseSensitive(output,"messages");
    cJSON *newmessage=cJSON_CreateObject();
    cJSON_AddItemToObject(newmessage,"sender",cJSON_CreateString(username));
    cJSON_AddItemToObject(newmessage,"content",cJSON_CreateString(message));
    cJSON_AddItemToArray(ar,newmessage);
    char *out=cJSON_PrintUnformatted(output);
    printf("/n---%s----\n",out);
    fclose(openchannel);
    openchannel=fopen(tmp3,"w");
    fprintf(openchannel,"%s",out);
    fclose(openchannel);
    sendjson("Successful","");
}
void refresh()
{
  char tmp1[32],tmp3[100];
  sscanf(input,"%*s %s",tmp1);
   cJSON *check=cJSON_GetObjectItemCaseSensitive(memberauthtoken,tmp1);
  if(check==NULL)
  {
      puts("wdjwifdjwi");
    sendjson("Error","auth token doesnt match");
    return;
  }
   char *a=cJSON_PrintUnformatted(check);
   printf("--%s",a);
  char username[100],channelname[100];
  int i;
  for(i=1;i<strlen(a);i++)
    username[i-1]=a[i];
  username[i-2]='\0';
  printf("\n%s\n ",username);

   check=cJSON_GetObjectItemCaseSensitive(memberactivechannel,username);
   if(check==NULL)
  {
    sendjson("Error","not joined in any channel");
    return;
  }
  a=cJSON_PrintUnformatted(check);
  for(i=1;i<strlen(a);i++)
    channelname[i-1]=a[i];
  channelname[i-2]='\0';
  printf("\n%s\n ",channelname);
    strcpy(tmp3,"channel\\");
  strcat(tmp3,channelname);
  strcat(tmp3,".json");
  int refreshcounter=0;
  cJSON *x=cJSON_GetObjectItemCaseSensitive(memberrefreshid,username);
  char* t=cJSON_PrintUnformatted(x);
   printf("---%s----",t);
   for(int i=0;i<strlen(t);i++)
   {
       refreshcounter=refreshcounter*10+t[i]-'0';
   }
   printf("---%d----",refreshcounter);
    FILE *openchannel=fopen(tmp3,"r");
 char linescan[4000];
    fgets(linescan,4000,openchannel);
   cJSON *output=cJSON_Parse(linescan);
    cJSON *ar=cJSON_GetObjectItemCaseSensitive(output,"messages");
    int arraysize=cJSON_GetArraySize(ar);
     for(int i=0;i<refreshcounter;i++)
  {
      cJSON_DeleteItemFromArray(ar,0);
  }
   cJSON *bigitem=cJSON_CreateObject();
    cJSON_AddItemToObject(bigitem,"type",cJSON_CreateString("list"));
    cJSON_AddItemReferenceToObject(bigitem,"content",ar);
  char *out=cJSON_PrintUnformatted(bigitem);
  printf("%s",out);
    send(client_socket,out, strlen(out), 0);
    fclose(openchannel);
    cJSON_DeleteItemFromObjectCaseSensitive(memberrefreshid,username);
    cJSON_AddItemToObject(memberrefreshid,username,cJSON_CreateNumber(arraysize));
}
void searchmember()
{
     char membertobesearched[1000],tmp2[100];
  sscanf(input,"%*s %s %s",membertobesearched,tmp2);
  cJSON *check=cJSON_GetObjectItemCaseSensitive(memberauthtoken,tmp2);
  if(check==NULL)
  {
    sendjson("Error","auth token doesnt match");
    return;
  }
  char *a=cJSON_PrintUnformatted(check);
  char username[1000];
  int i;
  for(i=1;i<strlen(a);i++)
    username[i-1]=a[i];
  username[i-2]='\0';
  membertobesearched[strlen(membertobesearched)-1]='\0';
  check=cJSON_GetObjectItemCaseSensitive(memberactivechannel,username);
  char *s=cJSON_PrintUnformatted(check);
  char channelname[1000];
   for(i=1;i<strlen(s);i++)
    channelname[i-1]=s[i];
  channelname[i-2]='\0';
  printf("\n%s\n ",channelname);
  check=cJSON_GetObjectItemCaseSensitive(channelmember,channelname);
   s=cJSON_PrintUnformatted(check);    ////listmemberha
  int j;
  int arraysize=cJSON_GetArraySize(check);
  for(i=0;i<arraysize;i++)
  {
      char stmp[1000];
      s=cJSON_PrintUnformatted(cJSON_GetArrayItem(check,i));
      for(j=1;j<strlen(s);j++)
    stmp[j-1]=s[j];
    stmp[j-2]='\0';
      if(strcmp(stmp,membertobesearched)==0)
      {
         send(client_socket,"TRUE",4,0);
         return;
      }
  }
  send(client_socket,"FALSE",5,0);
}
void searchmessage()
{
     char messagetobesearched[1000],tmp2[100];
  sscanf(input,"%*s %s %s",messagetobesearched,tmp2);
  cJSON *check=cJSON_GetObjectItemCaseSensitive(memberauthtoken,tmp2);
  if(check==NULL)
  {
    sendjson("Error","auth token doesnt match");
    return;
  }
  char *a=cJSON_PrintUnformatted(check);
  char username[1000];
  int i;
  for(i=1;i<strlen(a);i++)
    username[i-1]=a[i];
  username[i-2]='\0';
  messagetobesearched[strlen(messagetobesearched)-1]='\0';
  check=cJSON_GetObjectItemCaseSensitive(memberactivechannel,username);
  char *s=cJSON_PrintUnformatted(check);
  char channelname[1000];
   for(i=1;i<strlen(s);i++)
    channelname[i-1]=s[i];
  channelname[i-2]='\0';
  printf("\nhhhhhhhhh%s\n %s ",channelname,messagetobesearched);
  char tmp3[100];
  strcpy(tmp3,"channel\\");
  strcat(tmp3,channelname);
  strcat(tmp3,".json");
    FILE *openchannel=fopen(tmp3,"r");
 char linescan[2000],tmpp[200];
    fgets(linescan,2000,openchannel);
  cJSON *t=cJSON_Parse(linescan);
  t=cJSON_GetObjectItemCaseSensitive(t,"messages");
  fclose(openchannel);
  for(int i=0;i<cJSON_GetArraySize(t);i++)
  {
      cJSON *a=cJSON_GetArrayItem(t,i);
      if(strcmp(cJSON_PrintUnformatted(cJSON_GetObjectItemCaseSensitive(a,"sender")),"\"server\"")==0)
        continue;
        char *b=cJSON_PrintUnformatted(cJSON_GetObjectItemCaseSensitive(a,"content"));
        if(doesithavesubstring(b,messagetobesearched)==1)
        {
             send(client_socket,b,strlen(b),0);
             return;
        }
  }
  send(client_socket,"no result found",15,0);
}
int doesithavesubstring(char *a,char *b)
{
    int counter=0;
    for(int i=1;i<strlen(a)-1;i++)
    {
         if((a[i]==' ')&&(counter==strlen(b)))
      {
          return(1);
      }
        if(counter>=strlen(b))
            counter=0;
      if(a[i]==b[counter])
      {
          counter++;
          continue;
      }
      if((a[i]==' ')||(a[i]!=b[counter]))
      {
          counter=0;
          continue;
      }
    }
    if(counter==strlen(b))
        return(1);
    return(0);
}
