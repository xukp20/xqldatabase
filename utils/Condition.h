/*
 * Conditions for WHERE
 */

#ifndef CONDITION_H
#define CONDITION_H
#include <string>
#include <vector>
#include "StringCompare.h"


class Condition {
public:
    enum cond {
        // alu
        EQ, NE, LE, LT, GE, GT,
        // string equal, string not equal to value
        EQS, NES,
        // string equal to string
        EQS2, NES2,
        // string
        LIKE,
        // list
        IN,
        // null
        IS_NULL, IS_NOT_NULL,
        // two column alu
        EQ2, NE2, LE2, LT2, GE2, GT2
    };
    cond kind;                  // condition kind
    unsigned int column;        // the column to compare
    Condition(cond kind, unsigned int column): kind(kind),column(column){}
    virtual ~Condition() = default;

    bool isSingleCompare(){
        // cannot be NE 
        return kind == EQ || kind == LE || kind == LT || kind == GE || kind == GT;
    }
};

class CompareCondition : public Condition {
public:
    double bound;
    CompareCondition(cond kind, unsigned int column, double bound): Condition(kind, column), bound(bound){}
    bool compare(double value) const{
        switch (kind) {
            case EQ:
                return value == bound;
            case NE:
                return value != bound;
            case LE:
                return value <= bound;
            case LT:
                return value < bound;
            case GE:
                return value >= bound;
            case GT:
                return value > bound;
            default:
                return false;
        }
    }
};

class StringCompareCondition : public Condition {
public:
    string bound;
    StringCompareCondition(cond kind, unsigned int column, string bound): Condition(kind, column), bound(bound){}
    bool compare(string value) const{
        switch (kind) {
            case EQS:
                return value == bound;
            case NES:
                return value != bound;
            default:
                return false;
        }
    }
};

class CompareTwoColumnCondition : public Condition {
public:
    unsigned int column2;
    CompareTwoColumnCondition(cond kind, unsigned int column, unsigned int column2): Condition(kind, column), column2(column2){}
    bool compare(double value1, double value2) const{
        switch (kind) {
            case EQ2:
                return value1 == value2;
            case NE2:
                return value1 != value2;
            case LE2:
                return value1 <= value2;
            case LT2:
                return value1 < value2;
            case GE2:
                return value1 >= value2;
            case GT2:
                return value1 > value2;
            default:
                return false;
        }
    }
    bool compare(string value1, string value2) const{
        switch (kind) {
            case EQ2:
                return value1 == value2;
            case NE2:
                return value1 != value2;
            default:
                return false;
        }
    }
};

class CompareTwoStringCondition : public Condition {
public:
    unsigned int column2;
    CompareTwoStringCondition(cond kind, unsigned int column, unsigned int column2): Condition(kind, column), column2(column2){}
    bool compare(string value1, string value2) const{
        switch (kind) {
            case EQS2:
                return value1 == value2;
            case NES2:
                return value1 != value2;
            default:
                return false;
        }
    }
};

class StringCondition : public Condition {
public:
    string pattern;
    StringCondition(cond kind, unsigned int column, string pattern): Condition(kind, column), pattern(pattern){}
    bool compare(string value) const{
        switch (kind) {
            case LIKE:
                return StringCompare::like(value, pattern);
            default:
                return false;
        }
    }
};

class NumListCondition : public Condition {
public:
    vector<double> list;
    NumListCondition(cond kind, unsigned int column): Condition(kind, column), list(vector<double>()){}
    void add(double value){
        list.push_back(value);
    }
    bool compare(double value) const{
        switch (kind) {
            case IN:
                for (int i = 0; i < list.size(); i++) {
                    if (value == list[i]) return true;
                }
                return false;
            default:
                return false;
        }
    }
};

class StrListCondition : public Condition {
public:
    vector<string> list;
    StrListCondition(cond kind, unsigned int column): Condition(kind, column), list(vector<string>()){}
    void add(string value){
        list.push_back(value);
    }
    bool compare(string value) const{
        switch (kind) {
            case IN:
                for (int i = 0; i < list.size(); i++) {
                    if (value == list[i]) return true;
                }
                return false;
            default:
                return false;
        }
    }
};


class NullCondition : public Condition {
public:
    NullCondition(cond kind, unsigned int column): Condition(kind, column){}
    bool compare(const vector<bool>& bitmap) const{
        if(bitmap.size() <= column) return false;
        switch (kind) {
            case IS_NULL:
                return !bitmap[column];
            case IS_NOT_NULL:
                return bitmap[column];
            default:
                return false;
        }
    }
};
#endif