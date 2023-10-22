#include "Condition.h"

#include <string>
#include <vector>
#include <iostream>
using namespace std;

class ConditionTester {
public:
    void static test_num();
    void static test_str();
    void static test_list();
    void static test_null();
};


void ConditionTester::test_num(){
    Condition* c1 = new CompareCondition(EQ, 0, 1);
    Condition* c2 = new CompareCondition(NE, 0, 1);
    Condition* c3 = new CompareCondition(LE, 0, 1);
    Condition* c4 = new CompareCondition(LT, 0, 1);
    Condition* c5 = new CompareCondition(GE, 0, 1);
    Condition* c6 = new CompareCondition(GT, 0, 1);

    cout<<"1 EQ 1: "<<((CompareCondition*)c1)->compare(1)<<endl;
    cout<<"1 NE 1: "<<((CompareCondition*)c2)->compare(1)<<endl;
    cout<<"1 LE 1: "<<((CompareCondition*)c3)->compare(1)<<endl;
    cout<<"1 LT 1: "<<((CompareCondition*)c4)->compare(1)<<endl;
    cout<<"1 GE 1: "<<((CompareCondition*)c5)->compare(1)<<endl;
    cout<<"1 GT 1: "<<((CompareCondition*)c6)->compare(1)<<endl;

    cout<<"1.2 EQ 1: "<<((CompareCondition*)c1)->compare(1.2)<<endl;
    cout<<"1.2 NE 1: "<<((CompareCondition*)c2)->compare(1.2)<<endl;
    cout<<"1.2 LE 1: "<<((CompareCondition*)c3)->compare(1.2)<<endl;
    cout<<"1.2 LT 1: "<<((CompareCondition*)c4)->compare(1.2)<<endl;
    cout<<"1.2 GE 1: "<<((CompareCondition*)c5)->compare(1.2)<<endl;
    cout<<"1.2 GT 1: "<<((CompareCondition*)c6)->compare(1.2)<<endl;

    cout<<"0.5 EQ 1: "<<((CompareCondition*)c1)->compare(0.5)<<endl;
    cout<<"0.5 NE 1: "<<((CompareCondition*)c2)->compare(0.5)<<endl;
    cout<<"0.5 LE 1: "<<((CompareCondition*)c3)->compare(0.5)<<endl;
    cout<<"0.5 LT 1: "<<((CompareCondition*)c4)->compare(0.5)<<endl;
    cout<<"0.5 GE 1: "<<((CompareCondition*)c5)->compare(0.5)<<endl;
    cout<<"0.5 GT 1: "<<((CompareCondition*)c6)->compare(0.5)<<endl;

    return;
}

void ConditionTester::test_str(){
    const char* str1 = "abc";
    const char* str2 = "1";
    const char* str3 = "a-";
    const char* str4 = "1b3";

    const char* pattern1 = "a%";
    const char* pattern2 = "%b%";
    const char* pattern3 = "[0-9]";
    const char* pattern4 = "[0-9][a-z][0-9]";
    const char* pattern5 = "[a-]";
    const char* pattern6 = "a[a-]%";
    const char* pattern7 = "a_";

    Condition* c1 = new StringCondition(LIKE, 0, pattern1);
    Condition* c2 = new StringCondition(LIKE, 0, pattern2);
    Condition* c3 = new StringCondition(LIKE, 0, pattern3);
    Condition* c4 = new StringCondition(LIKE, 0, pattern4);
    Condition* c5 = new StringCondition(LIKE, 0, pattern5);
    Condition* c6 = new StringCondition(LIKE, 0, pattern6);
    Condition* c7 = new StringCondition(LIKE, 0, pattern7);

    cout<<"abc LIKE a%: "<<((StringCondition*)c1)->compare(str1)<<endl;
    cout<<"1 LIKE a%: "<<((StringCondition*)c1)->compare(str2)<<endl;
    cout<<"a- LIKE a%: "<<((StringCondition*)c1)->compare(str3)<<endl;
    cout<<"1b3 LIKE a%: "<<((StringCondition*)c1)->compare(str4)<<endl;

    cout<<"abc LIKE %b%: "<<((StringCondition*)c2)->compare(str1)<<endl;
    cout<<"1 LIKE %b%: "<<((StringCondition*)c2)->compare(str2)<<endl;
    cout<<"a- LIKE %b%: "<<((StringCondition*)c2)->compare(str3)<<endl;
    cout<<"1b3 LIKE %b%: "<<((StringCondition*)c2)->compare(str4)<<endl;

    cout<<"abc LIKE [0-9]: "<<((StringCondition*)c3)->compare(str1)<<endl;
    cout<<"1 LIKE [0-9]: "<<((StringCondition*)c3)->compare(str2)<<endl;
    cout<<"a- LIKE [0-9]: "<<((StringCondition*)c3)->compare(str3)<<endl;
    cout<<"1b3 LIKE [0-9]: "<<((StringCondition*)c3)->compare(str4)<<endl;

    cout<<"abc LIKE [0-9][a-z][0-9]: "<<((StringCondition*)c4)->compare(str1)<<endl;
    cout<<"1 LIKE [0-9][a-z][0-9]: "<<((StringCondition*)c4)->compare(str2)<<endl;
    cout<<"a- LIKE [0-9][a-z][0-9]: "<<((StringCondition*)c4)->compare(str3)<<endl;
    cout<<"1b3 LIKE [0-9][a-z][0-9]: "<<((StringCondition*)c4)->compare(str4)<<endl;

    cout<<"abc LIKE [a-]: "<<((StringCondition*)c5)->compare(str1)<<endl;
    cout<<"1 LIKE [a-]: "<<((StringCondition*)c5)->compare(str2)<<endl;
    cout<<"a- LIKE [a-]: "<<((StringCondition*)c5)->compare(str3)<<endl;
    cout<<"1b3 LIKE [a-]: "<<((StringCondition*)c5)->compare(str4)<<endl;

    cout<<"abc LIKE a[a-]%: "<<((StringCondition*)c6)->compare(str1)<<endl;
    cout<<"1 LIKE a[a-]%: "<<((StringCondition*)c6)->compare(str2)<<endl;
    cout<<"a- LIKE a[a-]%: "<<((StringCondition*)c6)->compare(str3)<<endl;
    cout<<"1b3 LIKE a[a-]%: "<<((StringCondition*)c6)->compare(str4)<<endl;

    cout<<"abc LIKE a_: "<<((StringCondition*)c7)->compare(str1)<<endl;
    cout<<"1 LIKE a_: "<<((StringCondition*)c7)->compare(str2)<<endl;
    cout<<"a- LIKE a_: "<<((StringCondition*)c7)->compare(str3)<<endl;
    cout<<"1b3 LIKE a_: "<<((StringCondition*)c7)->compare(str4)<<endl;

    return;
}

void ConditionTester::test_list(){
    Condition* num_cond = new NumListCondition(IN, 0);
    Condition* str_cond = new StrListCondition(IN, 0);

    ((NumListCondition*)num_cond)->add(1);
    ((NumListCondition*)num_cond)->add(2);
    ((NumListCondition*)num_cond)->add(3);

    ((StrListCondition*)str_cond)->add("abc");
    ((StrListCondition*)str_cond)->add("def");
    ((StrListCondition*)str_cond)->add("ghi");

    cout<<"1 IN (1,2,3): "<<((NumListCondition*)num_cond)->compare(1)<<endl;
    cout<<"2 IN (1,2,3): "<<((NumListCondition*)num_cond)->compare(2)<<endl;
    cout<<"3 IN (1,2,3): "<<((NumListCondition*)num_cond)->compare(3)<<endl;
    cout<<"4 IN (1,2,3): "<<((NumListCondition*)num_cond)->compare(4)<<endl;

    cout<<"abc IN (abc,def,ghi): "<<((StrListCondition*)str_cond)->compare("abc")<<endl;
    cout<<"def IN (abc,def,ghi): "<<((StrListCondition*)str_cond)->compare("def")<<endl;
    cout<<"ghi IN (abc,def,ghi): "<<((StrListCondition*)str_cond)->compare("ghi")<<endl;
    cout<<"jkl IN (abc,def,ghi): "<<((StrListCondition*)str_cond)->compare("jkl")<<endl;

    return;
}

void ConditionTester::test_null(){
    NullCondition* c1 = new NullCondition(IS_NULL, 1);
    NullCondition* nc1 = new NullCondition(IS_NOT_NULL, 1);
    NullCondition* c2 = new NullCondition(IS_NULL, 2);
    NullCondition* nc2 = new NullCondition(IS_NOT_NULL, 2);
    NullCondition* c3 = new NullCondition(IS_NULL, 3);
    NullCondition* nc3 = new NullCondition(IS_NOT_NULL, 3);


    vector<bool> bitmap({true, true, false, false, true, false});
    for (int i = 0; i < 6; i++){
        cout<<bitmap[i]<<" ";
    }
    cout<<endl;

    cout<<"1 IS NULL: "<<c1->compare(bitmap)<<endl;
    cout<<"1 IS NOT NULL: "<<nc1->compare(bitmap)<<endl;
    cout<<"2 IS NULL: "<<c2->compare(bitmap)<<endl;
    cout<<"2 IS NOT NULL: "<<nc2->compare(bitmap)<<endl;
    cout<<"3 IS NULL: "<<c3->compare(bitmap)<<endl;
    cout<<"3 IS NOT NULL: "<<nc3->compare(bitmap)<<endl;

    return;
}