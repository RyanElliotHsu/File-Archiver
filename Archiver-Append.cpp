#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <string>

using namespace std;

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
    }
    else if(a==1)
    {
        //append
    }
    else if(x==1)
    {
        extract
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


