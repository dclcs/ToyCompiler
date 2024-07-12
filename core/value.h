//
// Created by cl d on 2024/7/12.
//

#ifndef TOYCOMPILER_VALUE_H
#define TOYCOMPILER_VALUE_H

#include "../common.h"

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
    } as;
} Value;

/// 返回类型
#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)

/// 创建变量类型
#define BOOL_VAL(value) ((Value){ VAL_BOOL, {.boolean = value} })
#define NIL_VAL           ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value) { VAL_NUMBER, { .number = value } })

/// 变量是否是类型
#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)

#endif //TOYCOMPILER_VALUE_H
