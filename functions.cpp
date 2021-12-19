//============================================================================
// Name         : functions.cpp
// Author       : Ryan Elliot Hsu, Sneheel Sarangi
// Date Created : 2021.12.12
// Date Modified: 2021.12.19
// Copyright    : All rights are reserved
// Description  : Assignment #4 Functions
//============================================================================

#include"header.hpp"
using namespace std;

//============================================================================
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
            else if(what_data==6)
                meta.uid=meta_buf;
            else if(what_data==7)
                meta.gid=meta_buf;
            meta_buf="";
            memset(meta_bufx, 0, sizeof(meta_bufx));
            what_data++;
        }
        
        else if(strcmp(meta_bufx,":")==0)
        {
            //Enter last parameter
            meta.gid=meta_buf;
            meta_buf="";
            memset(meta_bufx, 0, sizeof(meta_bufx));
            return 0;
        }

        else
        {
            meta_buf+=meta_bufx;
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
            cout<<meta.numcontent<<meta.st_mode;
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
        curr_pos=lseek(archivefd,0,SEEK_CUR);
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
        metadata += "?";
        metadata += to_string(statbuf.st_uid);
        metadata += "?";
        metadata += to_string(statbuf.st_gid);
        metadata += ":";
        while((n=read(filex, buffer, sizeof(buffer)))>0)
        {
            write(archivefd,buffer,n);
            curr_pos+=n;
        }
        data_pos=curr_pos;
        meta_pos=curr_pos;
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
            metadata += "?";
            metadata += to_string(statbuf.st_uid);
            metadata += "?";
            metadata += to_string(statbuf.st_gid);
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

}