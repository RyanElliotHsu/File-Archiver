//============================================================================
// Name         : Archiver-Append.cpp
// Author       : Ryan Elliot Hsu, Sneheel Sarangi
// Date Created : 2021.12.12
// Date Modified: 2021.12.12
// Copyright    : All rights are reserved
// Description  : Assignment #4 Archiver Append
//============================================================================

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<dirent.h>

#define BSIZE 1024

using namespace std;
//============================================================================

int archivefd;
off_t curr_pos;
off_t data_pos;
off_t meta_pos;
char buffer[BSIZE];

struct Metadata
{
    string type;
    string name;
    string curr_pos;
    string numcontent;
    string st_mode;
    string size;
    string st_dev;
    string st_ino;
    string nlink;
    string uid;
    string gid;
    string atime;
    string mtime;
    string ctime;
};

void clears(struct Metadata &meta)
{
    meta.name="";
    meta.type="";
    meta.st_mode="";
    meta.curr_pos="";
    meta.numcontent="";
    meta.size="";
}
int getstruct(struct Metadata &meta)
{
    string meta_buf="";
    char meta_bufx[2]="0";
    char loc[20];
    int count=0;
    int what_data=0;
    while((count=read(archivefd, meta_bufx, 1))>0)
    {   
        //cout<<meta_bufx<<"\n";
        if(strcmp(meta_bufx,"?")==0)
        {   
            if(what_data==0)
                meta.type=meta_buf;
            else if(what_data==1)
                meta.name=meta_buf;
            else if(what_data==2)
                meta.curr_pos=meta_buf;
            else if(what_data==3)
                meta.numcontent=meta_buf;
            else if(what_data==4)
                meta.st_mode=meta_buf;
            else if(what_data==5)
                meta.size=meta_buf;
            //cout<<meta_buf<<"\n";
            meta_buf="";
            memset(meta_bufx, 0, sizeof(meta_bufx));
            what_data++;
        }
        
        else if(strcmp(meta_bufx,":")==0)
        {
            //Enter last parameter
            meta.size=meta_buf;
            meta_buf="";
            memset(meta_bufx, 0, sizeof(meta_bufx));
            return 0;
        }

        else
        {
            // printf("HELLO %s, %s A\n",meta_bufx,meta_buf.c_str());
            meta_buf+=meta_bufx;
            //cout<<meta_buf<<"\n\n";
            memset(meta_bufx, 0, sizeof(meta_bufx));
        }
    }
    return -1;   
}

void change_attributes(int fd,struct Metadata meta)
{
    fchmod(fd,stoi(meta.st_mode));
    fchown(fd,stoi(meta.uid),stoi(meta.gid));        
}
void printer(string pathx)
{
    // char meta_buf[50];
    // char meta_bufx[1];
    // int count;
    // int skip=0;
    // while((count=read(archivefd, meta_bufx, 1))>0)
    // {
    //     if(skip==0)
    //     {    
    //         if(meta_bufx=="?")
    //         {
    //             if(strcmp(meta_buf,"F")==0)
    //             {    
    //                 memset(meta_buf, 0, sizeof(meta_buf));
    //                 count=read(archivefd,meta_bufx,1);
    //                 while(strcmp(meta_bufx,"?")!=0)
    //                 {
    //                     strcat(meta_buf,meta_bufx);
    //                     count=read(archivefd,meta_bufx,1);    
    //                 }
    //                 printf("%s%s",pathx.c_str(),meta_buf);
    //             }
    //             else if (strcmp(meta_buf,"D")==0)
    //             {
    //                 pathx+"-";
    //                 memset(meta_buf, 0, sizeof(meta_buf));
    //                 count=read(archivefd,meta_bufx,1);

    //                 while(strcmp(meta_bufx,"?")!=0)
    //                 {
    //                     strcat(meta_buf,meta_bufx);
    //                     count=read(archivefd,meta_bufx,1);    
    //                 }
    //                 printf("%s%s",pathx.c_str(),meta_buf);
    //                 printer(pathx,num_f);
                    
    //             }
    //             memset(meta_buf, 0, sizeof(meta_buf));
    //             skip=1;
    //         }

    //         else
    //         {
    //             strcat(meta_buf,meta_bufx);
    //         }
    //     }
    //     else
    //     {
    //         while(meta_bufx!=":")
    //         {
    //             count=read(archivefd,meta_bufx,1); 
    //         }
    //         memset(meta_buf, 0, sizeof(meta_buf));
    //         skip=0;
    //         continue;
    //     }
    // }

    struct Metadata meta;
    pathx+="-";
    int check=getstruct(meta);
    if(check!=-1)
    {    
        cout<<pathx<<meta.name<<"\n";
        if(meta.type=="d")
        {
            int nums = stoi(meta.numcontent);
            while(nums>0)
            {
                printer(pathx);
                nums--;
            }
        }
    }
}
void extractor(string pathx)
{
    pathx+="/";
    struct Metadata meta;
    int check=getstruct(meta);
    if(check!=-1)
    {    
        if(meta.type=="d")
        {
            int nums = stoi(meta.numcontent);
            //create directory
            pathx+=meta.name;
            mkdir(pathx.c_str(),stoi(meta.st_mode));
            while(nums>0)
            {
                extractor(pathx);
                nums--;
            }
        }
        else
        {
            off_t store_pos = lseek(archivefd,0,SEEK_CUR);
            curr_pos=lseek(archivefd,stoi(meta.curr_pos),SEEK_SET);
            int n=0;
            int num=0;
            char bufx[2];
            pathx+=meta.name;
            int filex= creat(pathx.c_str(),0644);        //created file
            while((n=read(archivefd, bufx, 1))>0)
            {
                write(filex,bufx,1);
                curr_pos+=n;
                num+=n;
                memset(bufx,0,sizeof(bufx));
                if(num==stoi(meta.size))
                    break;
            }

            change_attributes(filex,meta);
            curr_pos=lseek(archivefd,store_pos,SEEK_SET);
        }
    }
}
void archiver(struct dirent* dirpx,string pathx, string &metadata)
{
    struct stat statbuf;
    string path=pathx;
    string name= dirpx->d_name;
    path=path+name;
    cout<<path<<"\n";
    int num_contents=0;
    if (stat(path.c_str(),&statbuf)==-1)
    {
        perror(" Failed to get file status ");
        exit(1);
    }
    if((statbuf.st_mode & S_IFMT) == S_IFREG)
    {
        //file
        int filex;
        if ((filex = open(path.c_str(), O_RDONLY)) < 0)
        {
            perror("open");
            exit(1);
        }
        int n=0;
        curr_pos=lseek(archivefd,data_pos,SEEK_SET);
        //appending all inode information to metadata
        metadata += "f";
        metadata += "?";
        metadata += name;
        metadata += "?";
        metadata += to_string(data_pos);
        metadata += "?";
        metadata += to_string(num_contents);                //append ? after every element as delimiter
        metadata += "?";
        metadata += to_string(statbuf.st_mode);
        metadata += "?";
        metadata += to_string(statbuf.st_size);
        // metadata += "?";
        // metadata += statbuf.st_dev;
        // metadata += "?";
        // metadata += statbuf.st_ino;
        // metadata += "?";
        // metadata += statbuf.st_nlink;
        // metadata += "?";
        // metadata += statbuf.st_uid;
        // metadata += "?";
        // metadata += statbuf.st_gid;
        // metadata += "?";
        // metadata += statbuf.st_atime;
        // metadata += "?";
        // metadata += statbuf.st_mtime;
        // metadata += "?";
        // metadata += statbuf.st_ctime;
        metadata += ":";
        while((n=read(filex, buffer, sizeof(buffer)))>0)
        {
            write(archivefd,buffer,n);
            curr_pos+=n;
        }
        data_pos=curr_pos;
        meta_pos=curr_pos;
        //curr_pos=lseek(archivefd,curr_pos,SEEK_SET);
    }
    else if((statbuf.st_mode & S_IFMT) == S_IFDIR)
    {
        //dir
        DIR * dirx ;
        struct dirent *dirp ;
        if ((dirx = opendir(path.c_str())) == NULL)
            fprintf (stderr , " cannot open %s \n",dirpx->d_name);
        else {
            num_contents=0;
            while ((dirp = readdir (dirx)) != NULL )
            {
                if(strcmp(dirp->d_name,"..")==0||strcmp(dirp->d_name,".")==0)
                    continue;
                num_contents++;
            }
            //appending all inode information to metadata
            metadata += "d";
            metadata += "?";
            metadata += name;
            metadata += "?";
            metadata += to_string(data_pos);
            metadata += "?";
            metadata += to_string(num_contents);                //append ? after every element as delimiter
            metadata += "?";
            metadata += to_string(statbuf.st_mode);
            metadata += "?";
            metadata += to_string(statbuf.st_size);
            // metadata += "?";
            // metadata += statbuf.st_dev;
            // metadata += "?";
            // metadata += statbuf.st_ino;
            // metadata += "?";
            // metadata += statbuf.st_nlink;
            // metadata += "?";
            // metadata += statbuf.st_uid;
            // metadata += "?";
            // metadata += statbuf.st_gid;
            // metadata += "?";
            // metadata += statbuf.st_atime;
            // metadata += "?";
            // metadata += statbuf.st_mtime;
            // metadata += "?";
            // metadata += statbuf.st_ctime;
            metadata += ":";
            path+="/";
            rewinddir(dirx);
            while ((dirp = readdir (dirx)) != NULL )
            {
                if(strcmp(dirp->d_name,"..")==0||strcmp(dirp->d_name,".")==0)
                    continue;
                archiver(dirp,path,metadata);
            }
            closedir(dirx);
        }

    }
    else if((statbuf.st_mode & S_IFMT) == S_IFLNK)
    {
        //symblink
    }

    curr_pos = lseek(archivefd,0,SEEK_SET);
    char str[20];
    sprintf(str, "%ld", meta_pos);
    int w=write(archivefd,str,20);
}



int main(int argc, char* argv[])
{
    bool c=0;
    bool x=0;
    bool a=0;
    bool m=0;
    bool p=0;
    int start=0;
    string archive="";
    string* files =new string[100];
    int files_size=100;
    int files_num=0;
    struct stat statbuf ;
    string metadata="";
    int num_contents=0;         //to count the number of contents within a directory


    //interpreting command line input
    //if number of inline parameters are less than 4 the code won't run
	if(argc<3)
	{
        perror("Please input all parameters\n");
        exit(1);
	}

	for(int i=1;i<argc;i++)
    {   
        printf("%s\n",argv[i]);
        //archiving
        if(strcmp(argv[i],"-c")==0)
        {   
            printf("Archiving");
            c=1;
            i++;
            archive=argv[i];
            i++;
            start=i;
            while(i<argc)
            {
                files[i-start]=argv[i];
                files_num++;
                i++;
                if(files_num>=files_size)
                {
                    string* tempfiles= new string[files_size*2];
                    for(int j=0;j<files_num;j++)
                    {
                        tempfiles[j]=files[j];
                    }
                    delete files;
                    files=tempfiles;
                    files_size=files_size*2; 
                }
            }

        }
        //setting filename
        else if (strcmp(argv[i],"-x")==0)
        {   
            x=1;
            i++;
            archive=argv[i];
            i++;
            start=i;
            while(i<argc)
            {
                files[i-start]=argv[i];
                files_num++;
                i++;
                if(files_num>=files_size)
                {
                    string* tempfiles= new string[files_size*2];
                    for(int j=0;j<files_num;j++)
                    {
                        tempfiles[j]=files[j];
                    }
                    delete files;
                    files=tempfiles;
                    files_size=files_size*2; 
                }
            }
        }
        else if (strcmp(argv[i],"-a")==0)
        {   
            a=1;
            i++;
            archive=argv[i];
            i++;
            start=i;
            while(i<argc)
            {
                files[i-start]=argv[i];
                files_num++;
                i++;
                if(files_num>=files_size)
                {
                    string* tempfiles= new string[files_size*2];
                    for(int j=0;j<files_num;j++)
                    {
                        tempfiles[j]=files[j];
                    }
                    delete files;
                    files=tempfiles;
                    files_size=files_size*2; 
                }
            }

        }
        else if (strcmp(argv[i],"-m")==0)
        {   
            m=1;
            i++;
            archive=argv[i];
        }
        else if (strcmp(argv[i],"-p")==0)
        {   
            p=1;
            i++;
            printf("Hierarchies");
            archive=argv[i];

        }
    }

    if(c==1)
    {
        //archive
        data_pos=20;
        meta_pos=0;
        curr_pos=0;
        string path;
        archivefd = creat(archive.c_str(),0644);        //created file
        int count=0;
        curr_pos=lseek(archivefd,data_pos,SEEK_SET);
        // if(write(archivefd,buffer,sizeof(buffer))<0)
        //     perror("write");

        while(files_num>0)
        {   
            if (stat(files[count].c_str(),&statbuf)==-1)
            {
                perror(" Failed to get file status ");
                exit(1);
            }
            if((statbuf.st_mode & S_IFMT) == S_IFREG)
            {
                //file
                int filex;
                if ((filex = open(files[count].c_str(), O_RDONLY)) < 0)
                {
                    perror("open");
                    exit(1);
                }
                int n=0;
                //curr_pos=lseek(archivefd,data_pos,SEEK_SET);
                curr_pos=lseek(archivefd,0,SEEK_CUR);
                //appending all inode information to metadata
                metadata += "f";
                metadata += "?";
                metadata += files[count];
                metadata += "?";
                metadata += to_string(curr_pos);
                metadata += "?";
                metadata += "0";                //append ? after every element as delimiter
                metadata += "?";
                metadata += to_string(statbuf.st_mode);
                metadata += "?";
                metadata += to_string(statbuf.st_size);
                // metadata += "?";
                // metadata += statbuf.st_dev;
                // metadata += "?";
                // metadata += statbuf.st_ino;
                // metadata += "?";
                // metadata += statbuf.st_nlink;
                // metadata += "?";
                // metadata += statbuf.st_uid;
                // metadata += "?";
                // metadata += statbuf.st_gid;
                // metadata += "?";
                // metadata += statbuf.st_atime;
                // metadata += "?";
                // metadata += statbuf.st_mtime;
                // metadata += "?";
                // metadata += statbuf.st_ctime;
                metadata += ":";
                while((n=read(filex, buffer, sizeof(buffer)))>0)
		        {
                    write(archivefd,buffer,n);
                    curr_pos+=n;
                }
                meta_pos=curr_pos;
                data_pos=curr_pos;
                close(filex);
            }
            else if((statbuf.st_mode & S_IFMT) == S_IFDIR)
            {
                //dir
                //condition to skip if .. or .

                DIR * dirx ;
	            struct dirent *dirp ;
                if ((dirx = opendir(files[count].c_str())) == NULL)
                    fprintf (stderr , " cannot open %s \n",files[count].c_str());
                else {
                    num_contents=0;
                    while ((dirp = readdir (dirx)) != NULL )
                    {
                        if(strcmp(dirp->d_name,"..")==0||strcmp(dirp->d_name,".")==0)
                            continue;
                        num_contents++;
                    }
                    //appending all inode information to metadata
                    metadata += "d";
                    metadata += "?";
                    metadata += files[count];
                    metadata += "?";
                    metadata += to_string(data_pos);
                    metadata += "?";
                    metadata += to_string(num_contents);                //append ? after every element as delimiter
                    metadata += "?";
                    metadata += to_string(statbuf.st_mode);
                    metadata += "?";
                    metadata += to_string(statbuf.st_size);
                    // metadata += "?";
                    // metadata += statbuf.st_dev;
                    // metadata += "?";
                    // metadata += statbuf.st_ino;
                    // metadata += "?";
                    // metadata += statbuf.st_nlink;
                    // metadata += "?";
                    // metadata += statbuf.st_uid;
                    // metadata += "?";
                    // metadata += statbuf.st_gid;
                    // metadata += "?";
                    // metadata += statbuf.st_atime;
                    // metadata += "?";
                    // metadata += statbuf.st_mtime;
                    // metadata += "?";
                    // metadata += statbuf.st_ctime;
                    metadata += ":";
                    data_pos=curr_pos;
                    cout<<metadata<<"\n";
                    rewinddir(dirx);
                    while ((dirp = readdir(dirx)) != NULL )
                    {
                        path="";
                        path+=files[count];
                        path+="/";
                        if(strcmp(dirp->d_name,"..")==0||strcmp(dirp->d_name,".")==0)
                            continue;
                        archiver(dirp,path,metadata);
                    }
                    closedir(dirx);
                    meta_pos=curr_pos;
                    data_pos=curr_pos;
                }

            }
            else if((statbuf.st_mode & S_IFMT) == S_IFLNK)
            {
                //symblink

            }

            count++;
            files_num--;
        }
        //when the data from all files has been put together append the metadata section
        //cout<<metadata<<metadata.size();
        meta_pos = lseek(archivefd, 0, SEEK_CUR);
        count=write(archivefd,metadata.c_str(),metadata.size());
        //write location of metadata to header
        curr_pos = lseek(archivefd,0,SEEK_SET);
        char str[20];
        sprintf(str, "%ld", meta_pos);
        int w=write(archivefd,str,sizeof(str));
        close(archivefd);
    }
    else if(a==1)
    {
        //append
        string path="";
        if(archivefd=open(archive.c_str(),O_RDONLY)<0)
            perror("open");
        char loc[20];
        int count = read(archivefd,loc,20);
        int count_f=0;
        meta_pos=atoi(loc);
        curr_pos=lseek(archivefd,meta_pos,SEEK_SET);
        //copy all metadata to a buffer
        string buffer_str="";
        while((count=read(archivefd, buffer, sizeof(buffer)))>0)
        {
            buffer_str+= buffer;
        }

        //write more data
        curr_pos=lseek(archivefd,meta_pos,SEEK_SET);

           while(files_num>0)
        {
            if (stat(files[count].c_str(),&statbuf)==-1)
            {
                perror(" Failed to get file status ");
                exit(1);
            }
            if((statbuf.st_mode & S_IFMT) == S_IFREG)
            {
                //file
                int filex;
                if ((filex = open(files[count].c_str(), O_RDONLY)) < 0)
                {
                    perror("open");
                    exit(1);
                }
                int n=0;
                curr_pos=lseek(archivefd,data_pos,SEEK_SET);

                //appending all inode information to metadata
                metadata += "f";
                metadata += "?";
                metadata += files[count];
                metadata += "?";
                metadata += to_string(data_pos);
                metadata += "?";
                metadata += "0";                //append ? after every element as delimiter
                metadata += "?";
                metadata += to_string(statbuf.st_mode);
                metadata += "?";
                metadata += to_string(statbuf.st_size);
                // metadata += "?";
                // metadata += statbuf.st_dev;
                // metadata += "?";
                // metadata += statbuf.st_ino;
                // metadata += "?";
                // metadata += statbuf.st_nlink;
                // metadata += "?";
                // metadata += statbuf.st_uid;
                // metadata += "?";
                // metadata += statbuf.st_gid;
                // metadata += "?";
                // metadata += statbuf.st_atime;
                // metadata += "?";
                // metadata += statbuf.st_mtime;
                // metadata += "?";
                // metadata += statbuf.st_ctime;
                metadata += ":";

                while((n=read(filex, buffer, sizeof(buffer)))>0)
		        {
                    write(archivefd,buffer,n);
                    curr_pos+=n;
                }
                data_pos=curr_pos;
                meta_pos=curr_pos;
                //curr_pos=lseek(archivefd,curr_pos,SEEK_SET);
            }
            else if((statbuf.st_mode & S_IFMT) == S_IFDIR)
            {
                //dir
                //condition to skip if .. or .

                DIR * dirx ;
	            struct dirent *dirp ;
                if ((dirx = opendir(files[count].c_str())) == NULL)
                    fprintf (stderr , " cannot open %s \n",files[count].c_str());
                else {
                    num_contents=0;
                    while ((dirp = readdir (dirx)) != NULL )
                    {
                        if(strcmp(dirp->d_name,"..")==0||strcmp(dirp->d_name,".")==0)
                            continue;
                        num_contents++;
                    }
                    //appending all inode information to metadata
                    metadata += "d";
                    metadata += "?";
                    metadata += files[count];
                    metadata += "?";
                    metadata += to_string(data_pos);
                    metadata += "?";
                    metadata += to_string(num_contents);                //append ? after every element as delimiter
                    metadata += "?";
                    metadata += to_string(statbuf.st_mode);
                    metadata += "?";
                    metadata += to_string(statbuf.st_size);
                    // metadata += "?";
                    // metadata += statbuf.st_dev;
                    // metadata += "?";
                    // metadata += statbuf.st_ino;
                    // metadata += "?";
                    // metadata += statbuf.st_nlink;
                    // metadata += "?";
                    // metadata += statbuf.st_uid;
                    // metadata += "?";
                    // metadata += statbuf.st_gid;
                    // metadata += "?";
                    // metadata += statbuf.st_atime;
                    // metadata += "?";
                    // metadata += statbuf.st_mtime;
                    // metadata += "?";
                    // metadata += statbuf.st_ctime;
                    metadata += ":";
                    rewinddir(dirx);
                    while ((dirp = readdir(dirx)) != NULL )
                    {
                        path="";
                        path+=files[count];
                        path+="/";
                        if(strcmp(dirp->d_name,"..")==0||strcmp(dirp->d_name,".")==0)
                            continue;
                        archiver(dirp,path,metadata);
                    }
                    closedir(dirx);
                    data_pos=curr_pos;
                    meta_pos=curr_pos;
                }

            }
            else if((statbuf.st_mode & S_IFMT) == S_IFLNK)
            {
                //symblink

            }

            count++;
            files_num--;
        }
        //when the data from all files has been put together append the metadata section
        count=lseek(archivefd,meta_pos,SEEK_SET);
        count=write(archivefd,metadata.c_str(),metadata.size());

        //write location of metadata to header
        curr_pos = lseek(archivefd,0,SEEK_SET);
        char str[20];
        sprintf(str, "%ld", meta_pos);
        int w=write(archivefd,str,20);


    }
    else if(x==1)
    {
        //extract
        if ((archivefd = open(archive.c_str(),O_RDONLY))<0)     //opening archive file to read from
        {
            perror("open");
            exit(1);
        }
        //getting the position of the metadata
        char loc[20];
        int count = read(archivefd,loc,20);
        meta_pos=atoi(loc);
        curr_pos=lseek(archivefd,meta_pos,SEEK_SET);
        struct Metadata meta;
        count=0;
        while(getstruct(meta)!=-1)
        {
            if(meta.name==files[count])
            {
                //extract file
                cout<<"File found!\n";
                if(meta.type=="d")
                {
                    int nums = stoi(meta.numcontent);
                    //create directory
                    string path=meta.name;
                    mkdir(path.c_str(),stoi(meta.st_mode));
                    while(nums>0)
                    {
                        extractor(path);
                        nums--;
                    }
                }
                else
                {
                    //meta_pos=lseek(archivefd,0,SEEK_CUR);           //store meta location
                    curr_pos=lseek(archivefd,stoi(meta.curr_pos),SEEK_SET);
                    int n=0;
                    int num=0;
                    char bufx[2];
                    int filex= creat(files[count].c_str(),0644);        //created file
                    while((n=read(archivefd, bufx, 1))>0)
                    {
                        write(filex,bufx,1);
                        curr_pos+=n;
                        num+=n;
                        memset(bufx,0,sizeof(bufx));
                        if(num==stoi(meta.size))
                            break;
                    }

                    change_attributes(filex,meta);

                    count++;
                    curr_pos=lseek(archivefd,meta_pos,SEEK_SET);
                    if(count==files_num)
                    {
                        printf("Success! Everything is extracted");
                        break;
                    }
                }
            }
            else
            {
                continue;
            }
        }
        

    }
    else if(m==1)
    {
        //print metadata

        printf("Printing metadata....\n");
        if((archivefd=open(archive.c_str(),O_RDONLY))<0)
            perror("open");
        
        //printf("%d\n",archivefd);
        char loc[20];
        int count = read(archivefd,loc,20);
        //printf("%s\n",loc);
        meta_pos=atoi(loc);
        //printf("%d\n",count);
        curr_pos=lseek(archivefd,meta_pos,SEEK_SET);
        //printf("%ld\n",curr_pos);
        struct Metadata meta;
        while(getstruct(meta)!=-1)          
        {
            cout<<meta.name<<"\n";
            cout<<meta.size<<"\n";
            cout<<meta.name<<"\n";
            clears(meta);
        }
        



    }
    else if(p==1)
    {
        //show hierarchy
        printf("Printing hierarchies....\n");
        if((archivefd=open(archive.c_str(),O_RDONLY))<0)
            perror("open");
        
        // printf("%d\n",archivefd);
        char loc[20];
        int count = read(archivefd,loc,20);
        // printf("%s\n",loc);
        meta_pos=atoi(loc);
        // printf("%d\n",count);
        curr_pos=lseek(archivefd,meta_pos,SEEK_SET);
        // printf("%ld\n",curr_pos);
        struct Metadata meta;
        while(getstruct(meta)!=-1)          
        {
            cout<<meta.name<<meta.numcontent<<"\n";
            //printf("%s\n",meta.name.c_str());
            if(meta.type=="f")
                continue;
            else if(meta.type=="d")
            {
                int nums = stoi(meta.numcontent);
                while(nums>0)
                {
                    printer(" ");
                    nums--;
                }
            }
            clears(meta);
        }



    }


}
