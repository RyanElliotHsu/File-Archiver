//============================================================================
// Name         : main.cpp
// Author       : Ryan Elliot Hsu, Sneheel Sarangi
// Date Created : 2021.12.12
// Date Modified: 2021.12.19
// Copyright    : All rights are reserved
// Description  : Assignment #4 Main
//============================================================================

#include"functions.cpp"
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
    int files_size=100;
    int files_num=0;
    struct stat statbuf ;
    string metadata="";
    int num_contents=0;         //to count the number of contents within a directory

    //interpreting command line input
    //if number of inline parameters are less than 4 the code won't run
	if(argc<3)
	{
        perror("Please input all parameters.\n");
        exit(1);
	}

	for(int i=1;i<argc;i++)
    {   
        printf("%s\n",argv[i]);
        
        //checking if the flag for archiving is true
        if(strcmp(argv[i],"-c")==0)
        {   
            //archiving
            printf("Archiving...\n\n");
            c=1;
            i++;
            archive=argv[i];
            i++;
            start=i;
            while(i<argc)
            {   
                //storing the user inputed file names
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

        //checking if the flag for extraction is true
        else if (strcmp(argv[i],"-x")==0)
        {   
            //setting filename
            x=1;
            i++;
            archive=argv[i];
            i++;
            start=i;
            while(i<argc)
            {
                //storing the user inputed file names
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

        //checking if the flag for appending is true
        else if (strcmp(argv[i],"-a")==0)
        {   
            a=1;
            i++;
            archive=argv[i];
            i++;
            start=i;
            while(i<argc)
            {
                //storing the user inputed file names
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

        //checking if the flag for printing metadata is true
        else if (strcmp(argv[i],"-m")==0)
        {   
            m=1;
            i++;
            archive=argv[i];
        }

        //checking if the flag for printing hierarchy is true
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
        data_pos=20;
        meta_pos=0;
        curr_pos=0;
        string path;
        archivefd = creat(archive.c_str(),0644);        //created file
        int count=0;
        curr_pos=lseek(archivefd,data_pos,SEEK_SET);

        while(files_num>0)
        {   
            //error checking for retrieving inode data
            if (stat(files[count].c_str(),&statbuf)==-1)
            {
                perror(" Failed to get file status ");
                exit(1);
            }
            if((statbuf.st_mode & S_IFMT) == S_IFREG)
            {
                //file
                int filex;
                //error checking for opening file
                if ((filex = open(files[count].c_str(), O_RDONLY)) < 0)
                {
                    perror("open");
                    exit(1);
                }
                int n=0;
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
                    metadata += "?";
                    metadata += to_string(statbuf.st_uid);
                    metadata += "?";
                    metadata += to_string(statbuf.st_gid);
                    metadata += ":";
                    data_pos=curr_pos;

                    rewinddir(dirx);    //rewinding the directory to traverse again 
                    while ((dirp = readdir(dirx)) != NULL )
                    {
                        path="";
                        path+=files[count];
                        path+="/";
                        if(strcmp(dirp->d_name,"..")==0||strcmp(dirp->d_name,".")==0)
                            continue;
                        archiver(dirp,path,metadata);   //recursively call archiver function to traverse another directory
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
        metadata="";
        if((archivefd=open(archive.c_str(),O_RDWR))<0)
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
            metadata+= buffer;
        }

        //write more data
        curr_pos=lseek(archivefd,meta_pos,SEEK_SET);

             while(files_num>0)
        {   
            //error checking for getting inode data
            if (stat(files[count].c_str(),&statbuf)==-1)
            {
                perror(" Failed to get file status ");
                exit(1);
            }
            if((statbuf.st_mode & S_IFMT) == S_IFREG)
            {
                //file
                int filex;
                //error check for opening file
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
                    metadata += "?";
                    metadata += to_string(statbuf.st_uid);
                    metadata += "?";
                    metadata += to_string(statbuf.st_gid);
                    metadata += ":";
                    data_pos=curr_pos;
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
        meta_pos = lseek(archivefd, 0, SEEK_CUR);
        count=write(archivefd,metadata.c_str(),1.5*metadata.size());
        //write location of metadata to header
        curr_pos = lseek(archivefd,0,SEEK_SET);
        char str[20];
        sprintf(str, "%ld", meta_pos);
        int w=write(archivefd,str,sizeof(str));
        close(archivefd);
    }
        else if(x==1)
    {   
        cout<<"Extracting...\n";
        //extract
        if((archivefd=open(archive.c_str(),O_RDONLY))<0)
            perror("open");
        
        char loc[20];
        int count = read(archivefd,loc,20);
        meta_pos=atoi(loc);
        //position to address of metadata
        curr_pos=lseek(archivefd,meta_pos,SEEK_SET);
        count=0;
        struct Metadata meta;
        if(argc==3)
        {
            int check=getstruct(meta);
            while(check!=-1)
            {    
                if(meta.type=="d")  //check if metadata type is directory
                {
                    int nums = stoi(meta.numcontent);
                    //create directory
                    mkdir(meta.name.c_str(),stoi(meta.st_mode));
                    while(nums>0)
                    {
                        extractor(meta.name);
                        nums--;
                    }
                }
                else    //extracting for file type
                {
                    off_t store_pos = lseek(archivefd,0,SEEK_CUR);
                    curr_pos=lseek(archivefd,stoi(meta.curr_pos),SEEK_SET);
                    int n=0;
                    int num=0;
                    char bufx[2];
                    int filex= creat(meta.name.c_str(),0644);        //created file
                    while((n=read(archivefd, bufx, 1))>0)
                    {
                        write(filex,bufx,1);
                        curr_pos+=n;
                        num+=n;
                        memset(bufx,0,sizeof(bufx));
                        if(num==stoi(meta.size))
                            break;
                    }

                    change_attributes(filex,meta);  //change owner, groups, and rights of file
                    curr_pos=lseek(archivefd,store_pos,SEEK_SET);
                }
                check=getstruct(meta);
            }
        }

        else
        { 
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
                            //recursively call extractor function to traverse directory
                            extractor(path);
                            nums--;
                        }
                    }
                    else
                    {
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
    }

    else if(m==1)
    {
        //print metadata
        printf("Printing metadata...\n\n");
        if((archivefd=open(archive.c_str(),O_RDONLY))<0)
            perror("open");
        
        char loc[20];
        int count = read(archivefd,loc,20);
        meta_pos=atoi(loc);
        curr_pos=lseek(archivefd,meta_pos,SEEK_SET);
        struct Metadata meta;
        while(getstruct(meta)!=-1)          
        {
            cout<<"File Name: "<<meta.name<<"\n";
            cout<<"Owner    : "<<meta.uid<<"\n";
            cout<<"Groups   : "<<meta.gid<<"\n";
            cout<<"Rights   : "<<meta.st_mode<<"\n\n";
            clears(meta);
        }

    }
    
    else if(p==1)
    {
        //show hierarchy
        printf("Printing hierarchies...\n");
        if((archivefd=open(archive.c_str(),O_RDONLY))<0)
            perror("open");
        
        char loc[20];
        int count = read(archivefd,loc,20);
        meta_pos=atoi(loc);
        curr_pos=lseek(archivefd,meta_pos,SEEK_SET);
        struct Metadata meta;
        while(getstruct(meta)!=-1)          
        {
            cout<<meta.name<<meta.numcontent<<"\n";
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