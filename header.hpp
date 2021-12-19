//============================================================================
// Name         : header.hpp
// Author       : Ryan Elliot Hsu, Sneheel Sarangi
// Date Created : 2021.12.12
// Date Modified: 2021.12.19
// Copyright    : All rights are reserved
// Description  : Assignment #4 Header
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
//offset for currest position, data position, metadata position
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
    string uid;
    string gid;
};