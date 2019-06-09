#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE 1
#define DIR 0

char *dest_path[10];

typedef struct FileOrDir
{
    char *name;                      
    int isFile;                      
    char *content;                   
    struct FileOrDir *parent;        
    struct FileOrDir *sibling;       
    struct FileOrDir *children_h;    
    struct FileOrDir *children_t;    
}FileOrDir;


void pwd(FileOrDir* now)
{
    FileOrDir* p=now;
    char s[200];
    char* t =s;
    *t='\0';
    t=strcat(t,"/");
    while(p->parent){
        t=strcat(t,p->name);
        t=strcat(t,"/");
        p=p->parent;
    }
    t=strcat(t,p->name);
    printf("%s\n",strrev(t));
}



void ls(FileOrDir *now)
{
    FileOrDir *fd = now->children_h;
    while (fd)
    {
        printf("%s ",fd->name);
        fd = fd->sibling;
    }
    printf("\n");
}



void split_filepath(char *src_path, char **dest_path, char *separator,int *count)
{
    char *temp = NULL;
    *count = 0;
    if (src_path == NULL || strlen(src_path) == 0 || separator == NULL || strlen(separator) == 0)
    {
        return;
    }
    temp = strtok(src_path, separator);
    while (temp != NULL)
    {
        *dest_path++ = temp;
        temp = strtok(NULL, separator);
        *count++;
    }
}



int find_dir(FileOrDir** targetDir, char* dir_name)
{
    int flag;
    FileOrDir* targetDir_child;
    targetDir_child = (*targetDir)->children_h;
    flag = 0;
    while (targetDir_child)
    {
        if ((strcmp(targetDir_child->name, dir_name) == 0) && (targetDir_child->isFile == DIR))
        {
            flag = 1;
            *targetDir = targetDir_child;
            break;
        }
        targetDir_child = targetDir_child->sibling;
    }
    return flag;
}


int find_file(FileOrDir** targetFile, char* file_name)
{
    int flag;
    FileOrDir* targetFile_child;
    targetFile_child = (*targetFile)->children_h;
    flag = 0;
    while (targetFile_child)
    {
        if ((strcmp(targetFile_child->name, file_name) == 0) && (targetFile_child->isFile == FILE))
        {
            flag = 1;
            *targetFile = targetFile_child;
            break;
        }
        targetFile_child = targetFile_child->sibling;
    }
    return flag;

}


void cat(char* filename, FileOrDir* now, FileOrDir* root)
{
    int i;
    FileOrDir *temp;
    int flag = 1;
    int count = 0;
    split_filepath(filename, dest_path, "/", &count);
    if (count <= 1)
    {
        printf("error\n");
    }
    else
    {
        if (strcmp(dest_path[0], ".") == 0)
        {
            temp = now;
            for (i = 1; i < count - 1; i++)
            {
                if (find_dir(&temp, dest_path[i]) == 0)
                {
                    flag = 0;
                    break;
                }
            }

            if (flag == 1)
            {
                if (find_file(&temp, dest_path[count-1]))//找到文件
                {
                    if (temp->content)//如果该文件曾被写入内容
                    {
                        printf("%s\n",temp->content);
                    }
                    else//如果该文件未曾被写入内容
                    {
                        printf("\n");
                    }
                }
                else//找不到对应的文件
                {
                    printf("error\n");
                }
            }
            else//找不到对应目录
            {
                printf("error\n");
            }
        }

        else if (strcmp(dest_path[0],"...") == 0)//从根目录进行查找
        {
            temp = root;
            for (i = 1; i < count - 1; i++)
            {
                if (find_dir(&temp, dest_path[i]) == 0)
                {
                    flag = 0;
                    break;
                }
            }
            if (flag == 1)
            {
                if (find_file(&temp, dest_path[count-1]))
                {
                    if (temp->content)
                    {
                        printf("%s\n",temp->content);
                    }
                    else
                    {
                        printf("\n");
                    }
                }
                else
                {
                    printf("error\n");
                }
            }
            else
            {
                printf("error\n");
            }    
        }    
    }
}



void makdir(FileOrDir* now, char* filename)
{
    FileOrDir *now_lastchild = now->children_t;
    FileOrDir *new_dir = (FileOrDir*)malloc(sizeof(FileOrDir));
    new_dir->name = (char*)malloc(sizeof(char)*strlen(filename));
    strcpy(new_dir->name, filename);
    new_dir->isFile = DIR;
    new_dir->parent = now;
    new_dir->sibling = NULL;
    new_dir->children_h = NULL;
    new_dir->children_t = NULL;
    if (now_lastchild)
    {
        now_lastchild->sibling = new_dir;
        now->children_t = new_dir;
    }
    else
    {
        now->children_h = now->children_t = new_dir;
    }
}



void touch(FileOrDir* now, char* filename)
{
    FileOrDir* now_lastchild = now->children_t;
    FileOrDir* new_file = (FileOrDir*)malloc(sizeof(FileOrDir));
    new_file->name = (char*)malloc(sizeof(char)*strlen(filename));
    strcpy(new_file->name, filename);

    new_file->isFile = FILE;
    new_file->parent = now;
    new_file->sibling = NULL;
    new_file->children_h = NULL;
    new_file->children_t = NULL;
    new_file->content = NULL;
    if (now_lastchild)
    {
        now_lastchild->sibling = new_file;
        now->children_t = new_file;
    }
    else
    {
        now->children_h = now->children_t = new_file;
    }
}



void tac(char* filename, char* content, FileOrDir* now, FileOrDir* root)
{
    int i;
    FileOrDir *temp;
    int count = 0;
    int flag = 1;
    split_filepath(filename, dest_path, "/", &count);
    if (count <= 1)
    {
        printf("error\n");
    }
    else
    {
        if (strcmp(dest_path[0], ".") == 0)
        {
            temp = now;
            for (i = 1; i < count - 1; i++)
            {
                if (find_dir(&temp, dest_path[i]) == 0)
                {
                    flag = 0;
                    break;
                }
            }
            if (flag == 1)
            {
                if (find_file(&temp, dest_path[count-1]))
                {
                    if (temp->content)
                    {
                        free(temp->content);
                        temp->content = NULL;
                    }
                    temp->content = (char*)malloc(sizeof(char)*strlen(content));
                    strcpy(temp->content, content);
                }
                else
                {
                    printf("error\n");
                }
                
            }
            else
            {
                printf("error\n");
            }
        }

        else if (strcmp(dest_path[0], "...") == 0)
        {
            temp = root;
            for (i = 0; i < count - 1; i++)
            {
                if (find_dir(&temp, dest_path[i]) == 0)
                {
                    flag = 0;
                    break;
                }
            }
            if (flag == 1)
            {
                if (find_file(&temp, dest_path[count - 1]))
                {
                    if (temp->content)
                    {
                        free(temp->content);
                        temp->content = NULL;
                    }
                    temp->content = (char*)malloc(sizeof(char)*strlen(content));
                    strcpy(temp->content, content);
                }
                else
                {
                    printf("error\n");
                }  
            }
            else
            {
                printf("error\n");
            }
        }
        else
        {
            printf("error\n");
        }
    }
}



void cd(FileOrDir** now, FileOrDir* root)
{
    int i;
    FileOrDir *temp;
    int flag = 1;
    int count = 0;
    char filename[50];
    if (getchar() == '\n')
    {
        *now = root;
    }
    else
    {
        scanf("%s",filename);
        split_filepath(filename, dest_path, "/", &count);
        if (count == 0)
        {
            if (strcmp(filename, "..") == 0)//上一级目录
            {
                if (*now != root)
                {
                    *now = (*now)->parent;
                }
                
            }
            else //字符串
            {
                temp = *now;
                if (find_dir(&temp, filename))
                {
                    *now = temp;
                }
                else
                {
                    printf("error\n");
                } 
            }
            
        }

        else//从根目录开始查找
        {
            temp = root;
            flag = 1;
            for (i = 1; i < count; i++)
            {
                if (find_dir(&temp, dest_path[i]) == 0)
                {
                    flag == 0;
                    break;
                }
            }
            if (flag == 1)
            {
                *now = temp;
            }
            else
            {
                printf("error\n");
            }
        }
    }
}


int main()
{
    int k;
    int num;//待输入的命令数
    //char *cmd; 错误,野指针,未分配内存
    char cmd[50];//待输入的命令
    char filename[50];
    char content[50];
    FileOrDir *now;
    FileOrDir *root;
    root = (FileOrDir*)malloc(sizeof(FileOrDir));
    root->name = "...";
    root->isFile = DIR;
    root->parent = NULL;
    root->sibling = NULL;
    root->children_h = NULL;
    root->children_t = NULL;

    now = root;

    scanf("%d",&num);
    for ( k = 0; k < num; k++)
    {
        scanf("%s",cmd);
        if (strcmp(cmd, "pwd") == 0)
        {
            pwd(now);
        }
        else if (strcmp(cmd, "ls") == 0)
        {
            ls(now);
        }
        else if (strcmp(cmd, "cat") == 0)
        {
            scanf("%s",filename);
            cat(filename, now, root);
        }
        else if (strcmp(cmd, "makdir") == 0)
        {
            scanf("%s",filename);
            makdir(now, filename);
        }
        else if (strcmp(cmd, "touch") == 0)
        {
            scanf("%s",filename);
            touch(now, filename);
        }
        else if (strcmp(cmd, "tac") == 0)
        {
            scanf("%s",filename);
            gets(content);
            tac(filename, content, now, root);
        }
        else if (strcmp(cmd, "cd") == 0)
        {
            cd(&now, root);
        }
        else
        {
            printf("error\n");
        }
    }
    return 0;
}
