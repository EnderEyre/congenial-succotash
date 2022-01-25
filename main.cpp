#include <iostream>
#include <vector>
#include "myshptr.h"
#include <memory>
#include <cmath>

struct whatsoever
{
    int x;
    int y;
    double z;
    static void print()
    {
        std::cout<<"invoked from static function"<<std::endl;
    }
    friend std::ostream& operator<<(std::ostream &os, const whatsoever &w)
    {
        return os<<w.x<<" "<<w.y<<" "<<w.z;
    }
};

//used just to print count.

void prcnt(long sz)
{
    std::cout << "count: " << sz << std::endl;
}
// here we want to show that in case of passing shared pointer by value we copy it, i.e. increment refcount.
template <class P>
void pass_value(test::shared_ptr<P> s)
{
    prcnt(s.use_count());
    std::cout<<*s<<std::endl;
}

// a little playground instead of depending on test framework.

int main()
{
    test::shared_ptr<std::string> brackets{new std::string};
    *brackets = std::string("brackets");
    std::cout<<*brackets<<std::endl;

    // nullptr_t
    {
        std::string *s = nullptr;
        test::shared_ptr<std::string> custom(s);
        std::shared_ptr<std::string> standard(s);
        test::shared_ptr<std::string> custom_from_null(nullptr);
        std::shared_ptr<std::string> standard_from_nullptr(nullptr);
        auto custom_copy = custom;
        auto standard_copy = standard;
        auto std_from_nptr_cp = standard_from_nullptr;
        auto cstm_fr_nptr_cp = custom_from_null;

        std::cout<<"custom use count "<<custom_copy.use_count()<<" "<<cstm_fr_nptr_cp.use_count()<<std::endl;
        std::cout<<"standard use count"<<standard_copy.use_count()<<" "<<std_from_nptr_cp.use_count()<<std::endl;
        whatsoever *another_nullptr = nullptr;
        std::shared_ptr<whatsoever> standard_pointer(another_nullptr);
        test::shared_ptr<whatsoever> custom_pointer(another_nullptr);
        standard_pointer->print();
        custom_pointer->print();
        std::cout<<std::boolalpha<<(standard_pointer.get() == custom_pointer.get())<<std::endl;
    }
    {
        test::shared_ptr<std::string> s(new std::string()); // make a pointer
        *s = "amIalive?"; // work with it
        s->append("!");

        std::vector<test::shared_ptr<std::string>> v(10,s); // spread it to increase refcount
        prcnt(s.use_count()); // show
        pass_value(s); // increase once
        prcnt(s.use_count()); // show decrease
        s.reset(); // and again
        for(size_t i = 0; i<9; i++)
        {
            prcnt(v[i].use_count());
            v[i].reset(); // and again
        }
        s = std::move(v[9]); // last man standing.
        prcnt(s.use_count());
    }
    {
        test::shared_ptr<whatsoever>(nullptr); // anonymous
        pass_value(test::shared_ptr<double>(new double(std::log(15.1542))));
        // test empty
        test::shared_ptr<whatsoever> emp;
        emp.reset();
        emp.reset(new whatsoever{5,4,3});
        //test move
        test::shared_ptr<whatsoever> another(new whatsoever{1,2,3});
        emp = std::move(another);
        std::cout<<*emp<<std::endl;
        another.reset(new whatsoever{4,5,6});
        //test swap
        emp.swap(another);
        std::cout<<*another<<" "<<*emp<<std::endl;
    }
    std::cin.get();
    return 0;
}
