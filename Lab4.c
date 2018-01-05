#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

void printdir(char *dir, int depth){
    struct dirent *entry;
    DIR *dp;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL){
        printf("failed to open dir\n");
        return;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL){
        lstat(entry->d_name, &statbuf);
        struct group* gid;
        struct passwd* uid;
        struct tm* tim;
        gid = getgrgid(statbuf.st_gid);
        uid = getpwuid(statbuf.st_uid);
        tim = localtime(&statbuf.st_ctime);

        if(S_ISDIR(statbuf.st_mode)){
            if(strcmp(entry->d_name,"..") == 0 || strcmp(entry->d_name,".") == 0);
            else{
                for(int i = 0;i < depth;i = i + 2){
                    printf("  |");
                    if(i == depth - 2) printf("  |——");
                }
                printf(" ");
                switch(statbuf.st_mode & S_IFMT){
                    case S_IFREG:  printf("-");    break;
                    case S_IFDIR:  printf("d");    break;
                    case S_IFLNK:  printf("l");    break;
                    case S_IFBLK:  printf("b");    break;
                    case S_IFCHR:  printf("c");    break;
                    case S_IFIFO:  printf("p");    break;
                    case S_IFSOCK: printf("s");    break;
                }
                for(int i = 8; i >= 0; i--){    
                    if(statbuf.st_mode & (1 << i)){
                        switch(i%3){
                            case 2: printf("r"); break;
                            case 1: printf("w"); break;
                            case 0: printf("x"); break;
                        }
                    }
                    else printf("-");
                }
                printf(" ");
                printf("%ld\t%s\t%s\t",statbuf.st_nlink,gid->gr_name,uid->pw_name);
                printf("%ld\t",statbuf.st_size);
                printf("%d-%d-%d %d:%d  \t",tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday,tim->tm_hour,tim->tm_min);
                printf("%s\n",entry->d_name);
                printdir(entry->d_name,depth+4);
            }
        }
        else{
            for(int i = 0;i < depth;i = i + 2){
                printf("  |");
                if(i == depth - 2) printf("  |——");
            }
            printf("  ");
            switch(statbuf.st_mode & S_IFMT){
                case S_IFREG:  printf("-");    break;
                case S_IFDIR:  printf("d");    break;
                case S_IFLNK:  printf("l");    break;
                case S_IFBLK:  printf("b");    break;
                case S_IFCHR:  printf("c");    break;
                case S_IFIFO:  printf("p");    break;
                case S_IFSOCK: printf("s");    break;
            }
            for(int i = 8; i >= 0; i--){    
                if(statbuf.st_mode & (1 << i)){
                    switch(i%3){
                        case 2: printf("r"); break;
                        case 1: printf("w"); break;
                        case 0: printf("x"); break;
                    }
                }
                else printf("-");
            }
            printf("  ");
            printf("%ld\t%s\t%s\t",statbuf.st_nlink,gid->gr_name,uid->pw_name);
            printf("%ld\t",statbuf.st_size);
            printf("%d-%d-%d %d:%d  \t",tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday,tim->tm_hour,tim->tm_min);
            printf("%s\n",entry->d_name);
        }
    }
    chdir("..");
    closedir(dp);
}

int main (int argc, char **argv){
    printdir(argv[1],0);
    return 0;
}