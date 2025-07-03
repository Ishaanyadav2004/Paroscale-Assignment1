#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<pthread.h>
#include<ctype.h>
#include<errno.h>

#define HASH_SIZE 32
#define MAX_THREADS 4


typedef struct Node{
  long key;
    struct Node*next;
}Node;

typedef struct{
    Node *head;
    pthread_mutex_t mutex;
}Table;


Table global_table[HASH_SIZE];

typedef struct{
char* filename;
long long startbyte;
long long endbyte;
}Thread;

//Hash Function
unsigned long long hashfnc(long long value)
{
    value=(value^(value>>30)*0xbf58476d1ce4e5b9ULL);
    value=(value^(value>>27)*0x94d049bb133111ebULL);
    value=value^(value>>31);

    return value;
}

void AddGlobalTable(long long value)
{

    unsigned long long hashVal=hashfnc(value);

    int idx=hashVal%HASH_SIZE;
pthread_mutex_lock(&global_table[idx].mutex); //lock the mutex

 Node* current=global_table[idx].head;
    while(current!=NULL)
    {
        if(current->key==value)
        {
            pthread_mutex_unlock(&global_table[idx].mutex);//unlock
           return ;
        }
        current=current->next;
    }

    Node*newNode=(Node*)malloc(sizeof(Node));
    if(!newNode)
    {
        fprintf(stderr,"Memory allocation failed");
        pthread_mutex_unlock(&global_table[idx].mutex);
        exit(EXIT_FAILURE);

    }

    newNode->key=value;
    newNode->next=global_table[idx].head;
    global_table[idx].head=newNode;

    pthread_mutex_unlock(&global_table[idx].mutex);

 

}

//Entry point Thread Function
void* entryPointFnc(void*arg)
{
    Thread*data=(Thread*)arg;
   FILE*fptr=fopen(data->filename,"r");
   if(!fptr)
{
    fprintf(stderr,"THread file failed to open");
    free(data);
    pthread_exit(NULL);
}

fseek(fptr,data->startbyte,SEEK_SET);

if(data->startbyte > 0)
{
    int ch;

    while(ftell(fptr)< data->endbyte &&  (ch=fgetc(fptr))!=EOF )
    {
        if(!isdigit(ch) && ch!='-' && ch!='+'){
        ungetc(ch,fptr);
    break;
    }
    }
}

char Buffer[256];
long long num;
while(ftell(fptr)<data->endbyte && fscanf(fptr ,"%255s",Buffer)==1)
{
    if(strlen(Buffer)>0 && (isdigit(Buffer[0])|| Buffer[0]=='-'|| Buffer[0]=='+'))
    {
        num=atoll(Buffer);
        AddGlobalTable(num);
    }
}

fclose(fptr);
free(data);
return NULL;
}



int main(int argc,char **argv)
{


    if(argc<2)
    {
        fprintf(stderr,"%s <Filename>\n",argv[0]);
        return EXIT_FAILURE;
    }

    struct stat st;
    char* filename=argv[1];
    if(stat(filename,&st)==-1)
    {
        fprintf(stderr,"Stat failed");
        return EXIT_FAILURE;
    }

    long long FileSize=st.st_size;
    if(FileSize==0)
    {
        printf("File is empty");
        return EXIT_SUCCESS;
    }

    for(int i=0;i<HASH_SIZE;i++)
    {
        global_table[i].head=NULL;
        if(pthread_mutex_init(&global_table[i].mutex,NULL)!=0)
        {
            perror("Mutex (mutex_init) failed");

        for(int j=0;j<i;j++)
        {
            pthread_mutex_destroy(&global_table[j].mutex);

        }
        return EXIT_FAILURE;
    }
}
long long ChunkSize=FileSize/MAX_THREADS;

if(ChunkSize==0 && FileSize>0)
{
    ChunkSize=1;
}
pthread_t threads[MAX_THREADS];


for(int i=0;i<MAX_THREADS;i++)
{
    Thread* data=(Thread*)malloc(sizeof(Thread));
    if(!data)
    {
        perror("malloc failed");
        for(int j=0;j<HASH_SIZE;j++)
        {
            pthread_mutex_destroy(&global_table[j].mutex);
        }

        return EXIT_FAILURE;
    }

    data->filename=filename;
    data->startbyte=(long long)i*ChunkSize;
    data->endbyte=(i==MAX_THREADS-1)? FileSize:(long long)(i+1)*ChunkSize;

    if(pthread_create(&threads[i],NULL,entryPointFnc,(void*)data)!=0){

   perror("pthread_create failed");
   free(data);
   for(int j=0;j<HASH_SIZE;j++)
   {
    pthread_mutex_destroy(&global_table[j].mutex);

   }
return EXIT_FAILURE;
}
}


for(int join_idx=0;join_idx<MAX_THREADS;join_idx++)
{
    pthread_join(threads[join_idx],NULL);
}


printf("Assignemnt-1:Paroscale Unique Numbers are:\n");
 long long total=0;
for(int iter=0;iter<HASH_SIZE;iter++)
{
    Node*current=global_table[iter].head;
    Node* nextNode;
    while(current!=NULL)
    {
        printf("%ld\n",current->key);
    nextNode=current->next;
        free(current);
        current=nextNode;
        total++;
    }
    //Destroy the mutext
    pthread_mutex_destroy(&global_table[iter].mutex);
}
printf("Total Unique Number are : %lld\n",total);


return EXIT_SUCCESS;
}