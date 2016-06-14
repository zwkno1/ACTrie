#pragma once

#include <algorithm>
#include <memory>
#include <queue>
#include <string>
#include <type_traits>


namespace detail
{

struct utf8_encoding
{
    template<typename Iter>
    static size_t word_count(Iter begin, Iter end)
    {
        size_t len = 0;
        while(begin != end)
        {
            len += ((*begin & 0xc0 )!= 0x80);
            ++begin;
        }
        return len;
    }
};


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

} // namespace detail

// actrie requires Ch is interget and Ch's size is one byte.
template<typename Ch = char,
         typename Encoding = detail::utf8_encoding,
         size_t CHUNK_SIZE = 1024>
class actrie
{
    static_assert(std::numeric_limits<Ch>::is_integer && (sizeof(Ch) == 1), "actrie requires Ch is interget and Ch's size is one byte.");

    enum { NEXT_SIZE = 256 };

    struct node
    {
        node()
            : next_{0}
            , fail_{0}
            , size_{0}
        {
        }

        node* next_[NEXT_SIZE];
        node* fail_;
        size_t size_;
    };

public:
    actrie()
        : root_(allocate_node())
    {
    }

    actrie(actrie && other)
	: pool_(std::move(other.pool_))
	, root_(other.root_)
    {
        other.root_ = nullptr;
    }

    actrie(const actrie &) = delete;

    actrie & operator=(const actrie &) = delete;

    bool insert(const std::basic_string<Ch> & str)
    {
        auto n = root_;
        for(auto ch : str)
        {
            auto idx = get_index(ch);

            //check if prefix already exsit
            if(n->size_)
                return false;

            if(!n->next_[idx])
                n->next_[idx] = allocate_node();
            n = n->next_[idx];
        }
        n->size_ = str.size();
        return true;
    }

    bool find(const std::basic_string<Ch> & str) const
    {
        auto n = root_;
        for(auto ch : str)
        {
            auto idx = get_index(ch);
            n = n->next_[idx];
            if(n->size_)
              return true;
        }
        return false;
    }

    size_t process(Ch * str, size_t & size, Ch mask = '*') const
    {
        return process_impl(str, size, mask);
    }

    size_t process(std::basic_string<Ch> & str, Ch mask = '*') const
    {
        size_t size = str.size();
        auto count =  process_impl(&str[0], size, mask);
        str.resize(size);
        return count;
    }

    // build actrie
    void build()
    {
        std::queue<node *> q;
        q.push(root_);
        while(!q.empty())
        {
            node * n = q.front();
            q.pop();

            if(n->size_)
            {
                for(auto & i : n->next_)
                {
                    i = root_;
                }
                continue;
            }

            for(int i = 0; i < NEXT_SIZE; ++i)
            {
                node * fail = (n == root_ ? root_ : n->fail_->next_[i]);
                if(n->next_[i])
                {
                    n->next_[i]->fail_ = fail;
                    q.push(n->next_[i]);
                }
                else
                {
                    n->next_[i] = fail;
                }
            }
        }
    }

    //for python
    std::string py_process(const std::string & str) const
    {
        std::string result = str;
        size_t size = result.size();
        auto count =  process_impl(&result[0], size, '*');
        result.resize(size);
        return result;
    }

private:
    size_t process_impl(Ch * str, size_t & size, Ch mask) const
    {
        auto n = root_;
        size_t count = 0;
        //characters before pos already processed
        size_t pos = 0;
        size_t offset = 0;

        for(size_t i = 0; i < size; ++i)
        {
            auto idx = get_index(str[i]);
            n = n->next_[idx];
            if(n->size_)
            {
                size_t word_begin = i - n->size_ + 1;
                auto real_len = Encoding::word_count(&str[word_begin], &str[i+1]);
                if(offset)
                    std::copy(&str[pos], &str[word_begin], &str[pos-offset]);
                std::fill_n(&str[word_begin - offset], real_len, mask);

                offset += n->size_ - real_len;
                pos = i+1;
                n = root_;

                ++count;
            }
        }

        if(offset && pos < size)
            std::copy(&str[pos], &str[size], &str[pos-offset]);
        size -= offset;
        return count;
    }

    inline node * allocate_node()
    {
        return pool_.allocate();
    }

    inline size_t get_index(Ch ch) const
    {
        return (typename std::make_unsigned<Ch>::type)ch;
    }

    detail::pool<node, CHUNK_SIZE> pool_;

    node * root_;
};

