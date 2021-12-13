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
#include<unistd.h>
#include<sys/stat.h>

#define BSIZE 1024

using namespace std;
//============================================================================

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
    char buffer[BSIZE];
    int files_size=100;
    int files_num=0;
    struct stat statbuf ;


    //interpreting command line input
    //if number of inline parameters are less than 4 the code won't run
	if(argc<5)
	{
        perror("Please input all parameters\n");
        exit(1);
	}

	for(int i=1;i<argc;i++)
    {   
        //setting the number of readers and writers
        if(strcmp(argv[i],"-c")==0)
        {   
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
            archive=argv[i];

        }
    }

    if(c==1)
    {
        //archive
        off_t data_pos=20;
        off_t meta_pos=0;
        off_t curr_pos=0;

        int archivefd;
        archivefd = creat(archive.c_str(),0644);        //created file
        int count=0;
        curr_pos=lseek(archivefd,data_pos,SEEK_SET);
        if(write(archivefd,buffer,buffer.size())<0)
            perror("write");

        while(files_num>0)
        {
            if (stat(files[count],&statbuf)==-1)
            {
                perror(" Failed to get file status ");
                exit(1);
            }
            if((statbuf.st_mode & S_IFMT) == S_IFREG)
            {
                //file
                int filex;
                if ((filex = open(files[count] , O_RDONLY)) < 0)
                {
                    perror("open");
                    exit(1);
                }
                int n=0;
                curr_pos=lseek(archivefd,data_pos,SEEK_SET);
                while((n=read(filex, buffer, sizeof(buffer)))>0)
		        {
                    write(archivefd,buffer,n);
                    curr_pos+=n;
                }
                //curr_pos=lseek(archivefd,curr_pos,SEEK_SET);
            }
            else if((statbuf.st_mode & S_IFMT) == S_IFDIR)
            {
                //dir
            }
            else if((statbuf.st_mode & S_IFMT) == S_IFLNK)
            {
                //symblink
            }

            count++;
            files_num--;
        }
        //when the data from all files has been put together append the metadata section

    }
    else if(a==1)
    {
        //append
    }
    else if(x==1)
    {
        //extract
    }
    else if(m==1)
    {
        //print metadata
    }
    else if(p==1)
    {
        //show hierarchy
    }


}
