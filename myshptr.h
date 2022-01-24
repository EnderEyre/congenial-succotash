#ifndef MYSHPTR_H
#define MYSHPTR_H
#include <atomic>
#include <cstddef>

namespace test
{
template <class P>
class shared_ptr
{
    class object_holder
    {
        P* m_object;
        std::atomic<long> m_rcnt;
    public:
        explicit object_holder(P* obj) : m_object(obj), m_rcnt(0) {}
        P* getObject() const
        {
            return m_object;
        }
        void inc()
        {
            ++m_rcnt;
        }
        long get_m_rcnt()
        {
            return m_rcnt;
        }
        void dec()
        {
            --m_rcnt;
        }
        ~object_holder()
        {
            delete m_object;
        }

    } *m_obj;

public:
    shared_ptr() noexcept : m_obj(nullptr) {}

    shared_ptr(std::nullptr_t) noexcept : m_obj(nullptr) { std::cout<<"not useless!"<<std::endl; };

explicit shared_ptr (P* pointer) :
        m_obj(new object_holder(pointer)) {} // may throw bad_alloc
    ~shared_ptr()
    {
        release();
    }
    shared_ptr(const shared_ptr& other) noexcept
    {
        *this = other;
    }
    shared_ptr(shared_ptr&& other) noexcept
    {
        *this = std::move(other);
    }
    shared_ptr &operator=(const shared_ptr& other) noexcept
    {
        if(&other == this)
        {
            return *this;
        }
        this->m_obj = other.m_obj;
        if(this->m_obj)
            this->m_obj->inc();
        return *this;
    }
    shared_ptr &operator=(shared_ptr &&other) noexcept
    {
        if(&other == this)
        {
            return *this;
        }
        this->release();
        this->m_obj = other.m_obj;
        other.m_obj = nullptr;
        return *this;

    }
    operator bool() const noexcept
    {
        if (m_obj)
        {
            return m_obj->getObject() != nullptr;
        }
        else return false;
    }
    size_t use_count() noexcept
    {
        if(m_obj)
            return m_obj->get_m_rcnt()+1;
        else return 0;
    }
    P &operator*() const noexcept
    {
        if(m_obj)
            return *m_obj->getObject();
        return *reinterpret_cast<P*>(0); // dont you dare.
        /*
        I could just leave it with warning,
        or throw exception,
        but behaviour should be undefined when you dereference nullptr.
        */
    }
    P *operator->() const noexcept
    {
        if(m_obj)
            return m_obj->getObject();
        return nullptr;
    }
    P *get() const noexcept
    {
        if(m_obj)
            return m_obj->getObject();
        return nullptr;
    }
    void reset() noexcept
    {
        *this = shared_ptr();
    }
    void reset(P* other)
    {
        *this = shared_ptr(other);
    }

private:
    void release()
    {
        if (m_obj)
        {
            m_obj->dec();
            if(m_obj->get_m_rcnt() == -1)
                delete m_obj;
        }
    }

};

}
#endif // MYSHPTR_H
