#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student{
    char no[10];
    char name[20];
    int age;
    int ranking;
    struct student *next;
}student,*pstudent;


student *create(){
    student *stu = (student*)malloc(sizeof(student));
    memset(stu->no,0,sizeof(stu->no));
    memset(stu->name,0,sizeof(stu->name));
    stu->age = 0;
    stu->ranking = 0;
    stu->next = NULL;
    return stu;
}


void find(student *stu,int n){
    while(stu!=NULL&&(stu->ranking!=n))
        stu = stu->next;
    if (stu==NULL)
        printf("查找失败!\n");
    else
        printf("%s\n", stu->no);
}


void sort(student *stu){
    student *p = stu->next;
    student *q = p->next;
    int temp1;
    char temp2[20];
    for (; p!=NULL; p=p->next)
    {
        for (q = p->next; q!=NULL; q=q->next)
        {
            if (p->ranking > q->ranking)
            {
                temp1 = p->age;
                p->age = q->age;
                q->age = temp1;

                temp1 = p->ranking;
                p->ranking = q->ranking;
                q->ranking = temp1;

                strcpy(temp2,p->no);
                strcpy(p->no,q->no);
                strcpy(q->no,temp2);

                strcpy(temp2,p->name);
                strcpy(p->name,q->name);
                strcpy(q->name,temp2);
            }
        }
    }

    while(stu->next!=NULL){
        printf("%s ", stu->next->no);
        stu=stu->next;
    }
    printf("\n");
}



void append(student *stu){
    student *temp = (student*)malloc(sizeof(student));
    while(stu->next){
        stu=stu->next;
    }
    scanf("%s %s %d %d",temp->no,temp->name,&temp->age,&temp->ranking);
    temp->next=NULL;
    stu->next=temp;
}


void modify(student *stu,char no2[10]){
    while(stu!=NULL && strcmp((stu->no),no2)!=0){
        stu=stu->next;
    }
    scanf("%d",&stu->ranking);
    printf("%s %d\n",stu->no,stu->ranking);
}


void del(student *stu,char no2[10]){
    student *temp;
    while(stu->next!=NULL&&strcmp((stu->next->no),no2)!=0){
        stu=stu->next;
    }
    temp = stu->next;
    stu->next=temp->next;
    free(temp);
}

void insert(student *stu,char loc_no[10]){
    student *newStu = (student*)malloc(sizeof(student));
    while(stu!=NULL &&strcmp(stu->no,loc_no)!=0){
        stu=stu->next;
    }
    scanf("%s %s %d %d",newStu->no,newStu->name,&newStu->age,&newStu->ranking);
    newStu->next = stu->next;
    stu->next = newStu;
}


void display(student *stu){
    stu = stu->next;
    while(stu!=NULL){
        printf("%s ",stu->no);
        stu=stu->next;
    }
    printf("\n");
}

int main(){
    char no[10],command[20];
    int i,ranking;
    student *stu;
    stu = create();//头指针
    for (i = 0; i < 10; i++)
        append(stu);

    while((scanf("%s",command))!=EOF){

        if (strcmp(command,"sort")==0){
            sort(stu);
        }
        else if (strcmp(command,"find")==0){
            scanf("%d",&ranking);
            find(stu,ranking);
        }
        else if (strcmp(command,"append")==0)
        {
            append(stu);
            display(stu);
        }
        else if (strcmp(command,"modify")==0)
        {
            scanf("%s",no);
            modify(stu,no);
        }
        else if (strcmp(command,"delete")==0)
        {
            scanf("%s",no);
            del(stu,no);
            display(stu);
        }
        else if (strcmp(command,"insert")==0)
        {
            scanf("%s",no);
            insert(stu,no);
            display(stu);
        }
    }
    system("pause");
    return 0;
}