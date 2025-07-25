#pragma once

#include <stdexcept>
#include <string>
#include <windows.h> 

// 定义一个自定义的 C++ 异常类来表示 SEH 异常
class SEH_Exception : public std::runtime_error {
public:
    // 构造函数，接收异常代码和异常指针
    SEH_Exception(unsigned int nCode, _EXCEPTION_POINTERS* pExcPointers);

    // 获取 SEH 异常代码 (如 EXCEPTION_ACCESS_VIOLATION)
    unsigned int getSehCode() const;

    // 获取发生异常的内存地址
    void* getExceptionAddress() const;

    // 静态成员函数，用于在 main 函数或插件入口点注册转换器
    static void setup();

private:
    unsigned int m_nSehCode;
    void*        m_pExceptionAddress;
};