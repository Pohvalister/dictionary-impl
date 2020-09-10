#include <iostream>
#include <unordered_map>
#include "my_dictionary.h"

using namespace std;

struct A{
    int val;

    explicit A(int v)
    : val(v)
    {}

    friend bool operator==(const A& a1, const A& a2);
    //friend bool operator<(const A& a1, const A& a2);
};

bool operator==(const A& a1, const A& a2){
    return a1.val == a2.val;
}

namespace std {
    template<>
    struct hash<A>{
        std::size_t operator()(A const &a) const noexcept {
            std::size_t h1 = std::hash<int>{}(a.val);
            return h1;
        }
    };
}


struct B{
    int val;

    explicit B(int v)
            : val(v)
    {}

    friend bool operator==(const B& b1, const B& b2);
    friend bool operator<(const B& b1, const B& b2);
};

bool operator==(const B& b1, const B& b2){
    return b1.val == b2.val;
}
bool operator<(const B& b1, const B& b2){
    return b1.val < b2.val;
}

struct C{
    int val;

    explicit C(int v)
            : val(v)
    {}

    friend bool operator==(const C& c1, const C& c2);
};

bool operator==(const C& c1, const C& c2){
    return c1.val == c2.val;
}


int main() {


    MyDictionary<int, int> Int_dic;

    Int_dic.Set(1, 10);
    Int_dic.Set(2, 20);
    std::cout<<Int_dic.Get(2);
    std::cout<<' '<< (Int_dic.IsSet(3) ? Int_dic.Get(3) : Int_dic.Get(1))<<'\n';

   MyDictionary<A, A> A_dic;
    A a1(20);
    a1.val = 20;
    A a2(10);
    a2.val = 10;
    A_dic.Set(a1, a2);
    A_dic.Set(a2, a1);
    A a3(30);
    a3.val = 20;
    std::cout<<A_dic.Get(a2).val<<' '<<(A_dic.IsSet(a3) ? "Yes" : "No")<<'\n';

    MyDictionary<B, B> B_dic;
    B b1(20);
    b1.val = 20;
    B b2(10);
    b2.val = 10;
    B_dic.Set(b1, b2);
    B_dic.Set(b2, b1);
    B b3(30);
    b3.val = 20;
    std::cout<<B_dic.Get(b2).val<<' '<<(B_dic.IsSet(b3) ? "Yes" : "No")<<'\n';

    MyDictionary<C, C> C_dic;
    C c1(20);
    c1.val = 20;
    C c2(10);
    c2.val = 10;
    C_dic.Set(c1, c2);
    C_dic.Set(c2, c1);
    C c3(30);
    c3.val = 20;
    std::cout<<C_dic.Get(c2).val<<' '<<(C_dic.IsSet(c3) ? "Yes" : "No")<<'\n';

    MyDictionary<std::string, std::string> Str_dic;

    Str_dic.Set("1", "2");
    Str_dic.Set("2", "1");
    std::cout<<Str_dic.Get("2")<<Str_dic.Get("1")<<'\n';

    MyDictionary<int, int> Int_dic_stress;
    std::vector<int> values;
    for (int i = 0; i < 3000; i++)
        values.push_back(i);

    std::random_shuffle(values.begin(), values.end());

    for (int i = 0; i<3000; i++)
        Int_dic_stress.Set(i, i);
    std::random_shuffle(values.begin(), values.end());

    for (int i = 0; i < 3000; i++){
        if (values[i] == Int_dic_stress.Get(values[i]))
            std::cout<<values[i]<<' ';
        else
            throw 32;
    }


    return 0;
}
