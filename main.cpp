#include <iostream>
#include <unordered_map>

#include "my_dictionary.h"

#include <gtest/gtest.h>

using namespace std;
TEST(simple_testing, int_work) {

    MyDictionary<int, int> int_dict;

    int_dict.Set(1, 10);
    int_dict.Set(2, 20);
    EXPECT_EQ(int_dict.Get(2), 20);
    EXPECT_FALSE(int_dict.IsSet(3));
    EXPECT_THROW(int_dict.Get(3), MyNotFoundException<int>);
}

TEST(simple_testing, string_work) {

    MyDictionary<string, string> int_dict;

    int_dict.Set("1", "10");
    int_dict.Set("2", "20");
    EXPECT_EQ(int_dict.Get("2"), "20");
    EXPECT_FALSE(int_dict.IsSet("3"));
    EXPECT_THROW(int_dict.Get("3"), MyNotFoundException<string>);
}

TEST(simple_testing, multiple_elements) {

    MyDictionary<int, int> int_dict;
    std::vector<int> values(5000);
    for (int i = 0; i < 5000; i++)
        values[i] = i;

    std::random_shuffle(values.begin(), values.end());

    for (int i = 0; i<5000; i++)
        int_dict.Set(values[i], values[i]);

    std::random_shuffle(values.begin(), values.end());

    for (int i = 0; i < 5000; i++)
        EXPECT_EQ(values[i], int_dict.Get(values[i]));
}

TEST(simple_testing, except_check) {

    MyDictionary<int, int> int_dict;
    try{
        int_dict.Get(1);
    } catch (MyNotFoundException<int> e){
        EXPECT_EQ(e.GetKey(),1);
    }
}
struct A{
    int val1, val2;
    explicit A(int v1, int v2) : val1(v1), val2(v2) {}
    friend bool operator==(const A& a1, const A& a2);
};
bool operator==(const A& a1, const A& a2){return a1.val1 == a2.val1 && a1.val2 == a2.val2;}

TEST(cases_testing, eq_only_struct){
    MyDictionary<A, A> A_dict;
    A a1(10, 10), a2(20, -1), a3(20, -1), a4(10, -1);
    A_dict.Set(a1, a4);
    A_dict.Set(a2, a3);
    EXPECT_EQ(A_dict.Get(a1),a4);
    EXPECT_EQ(A_dict.Get(a3),a3);

    A_dict.Set(a1, a1);
    EXPECT_EQ(A_dict.Get(a1), a1);
    EXPECT_THROW(A_dict.Get(a4),MyNotFoundException<A>);
    EXPECT_FALSE(A_dict.IsSet(a4));
}

class B{
    int val1, val2;
    friend bool operator==(const B& b1, const B& b2);
    friend bool operator<(const B& b1, const B& b2);

public:
    explicit B(int v1, int v2) : val1(v1), val2(v2) {}
};
bool operator==(const B& b1, const B& b2){return b1.val1 == b2.val1 && b1.val2 == b2.val2;}
bool operator<(const B& b1, const B& b2){return (b1.val1 == b2.val1 ? b1.val2 < b2.val2 : b1.val1 < b2.val2);}

TEST(cases_testing, eq_comp_struct){
    MyDictionary<B, B> B_dict;
    B b();
    B b1(10, 10), b2(20, -1), b3(20, -1), b4(10, -1);
    B_dict.Set(b1, b4);
    B_dict.Set(b2, b3);
    EXPECT_EQ(B_dict.Get(b1),b4);
    EXPECT_EQ(B_dict.Get(b3),b3);

    B_dict.Set(b1, b1);
    EXPECT_EQ(B_dict.Get(b1), b1);
    EXPECT_THROW(B_dict.Get(b4),MyNotFoundException<B>);
    EXPECT_FALSE(B_dict.IsSet(b4));
}

class C{
    int val1, val2;
    friend bool operator==(const C& c1, const C& c2);
    friend std::hash<C>;
public:
    explicit C(int v1, int v2) : val1(v1), val2(v2) {}
};
bool operator==(const C& c1, const C& c2){return c1.val1 == c2.val1 && c1.val2 == c2.val2;}

namespace std {
    template<>
    struct hash<C>{
        std::size_t operator()(C const &c) const noexcept {
            std::size_t h1 = std::hash<int>{}(c.val1);
            std::size_t h2 = std::hash<int>{}(c.val2);
            return h1 ^ (h2 << 1);
        }
    };
}

TEST(cases_testing, eq_hash_struct){
    MyDictionary<C, C> C_dict;
    C c1(10, 10), c2(20, -1), c3(20, -1), c4(10, -1);
    C_dict.Set(c1, c4);
    C_dict.Set(c2, c3);
    EXPECT_EQ(C_dict.Get(c1),c4);
    EXPECT_EQ(C_dict.Get(c3),c3);

    C_dict.Set(c1, c1);
    EXPECT_EQ(C_dict.Get(c1), c1);
    EXPECT_THROW(C_dict.Get(c4),MyNotFoundException<C>);
    EXPECT_FALSE(C_dict.IsSet(c4));
}

TEST(cases_testing, touch_diff_types){
    MyDictionary<char, int> char_dict;
    char_dict.Set('0', 1);
    EXPECT_EQ(char_dict.Get('0'), 1);
    EXPECT_FALSE(char_dict.IsSet('1'));

    MyDictionary<bool, int> bool_dict;
    bool_dict.Set(false,1);
    EXPECT_EQ(bool_dict.Get(false), 1);
    EXPECT_THROW(bool_dict.Get(true), MyNotFoundException<bool>);

    MyDictionary<long double, int> double_dict;
    double_dict.Set(123456789.0, 1);
    EXPECT_EQ(double_dict.Get(123456789.0), 1);
    EXPECT_THROW(double_dict.Get(0.0), MyNotFoundException<long double>);
}
