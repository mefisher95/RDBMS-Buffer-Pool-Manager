#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <string>
#include <vector>


class File
{
public:
    //==========================================================================
    // Constructor
    //==========================================================================
    File(const char filename[1024], int page_size = 4)
        :page_size_(page_size)
    {
        strcpy(filename_, filename);
        fd_ = open(filename, O_RDWR | O_CREAT, 0644);
        if (fd_ < 0)
        {
            std::cout << "open fail\n";
            std::cout << "errno: " << errno << std::endl;
            std::cout << "strerror: " << strerror(errno) << std::endl;
        }
    }

    //==========================================================================
    // Destructor
    //==========================================================================

    ~File() { close_file(); }

    //==========================================================================
    // GET Methods
    //==========================================================================
    
    std::string filename() const { return filename_; }
    int fd() const { return fd_; }
    int get_page_size() const { return page_size_; }

    //==========================================================================
    // RDWR Methods
    //==========================================================================
    
    void read_file(int page, char buff[])
    {        
        lseek(fd_, page * page_size_, SEEK_CUR);    
        read(fd_, buff, page_size_);
        lseek(fd_, 0, SEEK_SET);
    }

    void write_file(int page, char buff[])
    {        
        lseek(fd_, page * page_size_, SEEK_CUR);
        write(fd_, buff, page_size_);
        lseek(fd_, 0, SEEK_SET);
    }

    void close_file() { close(fd_); }


private:

    //==========================================================================
    // Member Variables
    //==========================================================================
    
    char filename_[1024];
    int fd_;
    int page_size_;
};


#endif
