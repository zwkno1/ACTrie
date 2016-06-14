#pragma once

#include <memory>

template<typename T, size_t CHUNK_SIZE>
class pool
{
    struct chunk
    {
        std::unique_ptr<chunk> next_;
        T data_[CHUNK_SIZE];
    };

public:
    pool() 
        : head_(nullptr)
        , used_(CHUNK_SIZE)
    {
    }

    pool(pool && other) 
        : head_(std::move(other.head_))
	, used_(other.used_)
    {
        other.used_ = CHUNK_SIZE;
    }

    pool(const pool &) = delete;
    
    pool & operator=(const pool &) = delete;

    inline T * allocate()
    {
        if(used_ == CHUNK_SIZE)
        {
            auto c = std::unique_ptr<chunk>(new chunk{});
            c->next_ = std::move(head_);
            head_ = std::move(c);
            used_ = 0;
        }
        return &head_->data_[used_++];
    }

private:
    std::unique_ptr<chunk> head_;
    size_t used_;
};
