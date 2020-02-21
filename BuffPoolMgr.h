#ifndef BUFFPOOLMGR_H
#define BUFFPOOLMGR_H

#include "File.h"

class BuffPoolMgr
{
public:
    BuffPoolMgr(const char filename[1024], int num_frames, int page_size)
        :filename_(filename), num_frames_(num_frames), page_size_(page_size),
         file_(filename, page_size)
    {
        available_ = new bool[num_frames];
        block_index_ = new int[num_frames];
        dirty_ = new bool[num_frames];

        frames_ = new char*[num_frames];
        for (int i = 0; i < num_frames; ++i)
        {
            frames_[i] = new char[page_size];
            available_[i] = true;
            dirty_[i] = false;
            block_index_[i] = -1;
        }

    }

    ~BuffPoolMgr()
    {
        delete[] available_;
        delete[] block_index_;
        delete[] dirty_;

        for (int i = 0; i < num_frames_; ++i)
        {
            delete[] frames_[i];
        }
        delete[] frames_;
    }

    //=========================================================
    // User Methods
    //=========================================================

    void ui(bool &ex)
    {
        std::cout << "[0] Fetch a page into memory\n"
                  << "[1] Write a frame\n"
                  << "[2] Shutdown\n"
                  << "Frames:";
        for (int i = 0; i < num_frames_; ++i)
        {
            int size = strlen(frames_[i]);
            std::cout << '[';
            if (dirty_[i]) std::cout << '*';
            if (!available_[i]) std::cout << block_index_[i] << ':'; 
            for (int k = 0; k < size; ++k)
            {
                std::cout << frames_[i][k];
            }
            std::cout << ']';
        }
        std::cout << '\n';

        int option;
        std::cout << "option: ";
        std::cin >> option;

        switch (option)
        {
            case 0:
                read();
                break;
            case 1:
                write();
                break;
            case 2:
                exit();
                ex = true;
                return;
            default:
                std::cout << "invalid option" << std::endl;
                break;
        }

        std::cout << std::endl;
    }

private:
    //==========================================================================
    // Private Methods
    //==========================================================================
    bool has_room(int &frame_num)
    {
        for (int i = 0; i < num_frames_; ++i)
        {
            if (available_[i])
            {
                frame_num = i;
                return true;
            }
        }
        return false;
    }

    bool is_page_in_frame(int page)
    {
        for (int i = 0; i < num_frames_; ++i)
        {
            if (block_index_[i] == page) return true;
        }
        return false;
    }

    int get_frame_id(int page)
    {
        for (int i = 0; i < num_frames_; ++i)
        {
            if (block_index_[i] == page) return i;
        }
        return -1;
    }

    void read()
    {
        int page;
        int frame_num = 0;
        
        std::cout << "which page? ";
        std::cin >> page;

        if (is_page_in_frame(page))
        {
            std::cout << "page " << page << " is already fetched ... "
                      << "frame id is "
                      << get_frame_id(page)
                      << std::endl;
            return;
        }
        
        if (has_room(frame_num))
        {   
            file_.read_file(page, frames_[frame_num]);
            available_[frame_num] = false;
            block_index_[frame_num] = page;
            return;
        }
        else
        {
            std::cout << "which frame to remove? ";
            std::cin >> frame_num;
            if (dirty_[frame_num])
            {
                int old_page = block_index_[frame_num];
                
                std::cout << "frame " << frame_num << " is dirty ... "
                          << "write to block " << old_page
                          << std::endl;
                
                file_.write_file(old_page, frames_[frame_num]);
                dirty_[frame_num] = false;
                file_.read_file(page, frames_[frame_num]);
                block_index_[frame_num] = page;
                return;
            }
            else
            {
                std::cout << "frame " << frame_num << " is not dirty ... "
                          << "no write"
                          << std::endl;
            }
            file_.read_file(page, frames_[frame_num]);
            block_index_[frame_num] = page;
            return;
        }
    }

    void write()
    {
        int page;
        std::cout << "which page? ";
        std::cin >> page;
        if (!is_page_in_frame(page))
        {
            std::cout << "page " << page << " is not loaded into buffer"
                      << std::endl;
        }
        else
        {
            int frame = get_frame_id(page);
            dirty_[frame] = true;
            std::cout << "enter " << page_size_ << " characters: ";
            for (int i = 0; i < page_size_; ++i)
            {
                std::cin >> frames_[frame][i];
            }
        }
        return;
    };

    void exit()
    {
        for (int i = 0; i < num_frames_; ++i)
        {
            int page = block_index_[i];
            
            if (dirty_[i])
            {
                std::cout << "frame " << i << " is dirty ... "
                          << "write to block " << page
                          << std::endl;
                file_.write_file(page, frames_[i]);
                dirty_[i] = false;
            }
        }
        std::cout << "halt" << std::endl;
    }
    
    //==========================================================================
    // Member variables
    //==========================================================================
    File file_;
    const char* filename_;

    char ** frames_;
    bool* available_;
    int* block_index_;
    bool* dirty_;

    int num_frames_;
    int page_size_;
};


#endif
