#include "SEHHandler.h"
#include <eh.h>    
#include <sstream> 

// SEH 异常转换函数
static void se_translator_function(unsigned int u, _EXCEPTION_POINTERS* pExp) { throw SEH_Exception(u, pExp); }

// 辅助函数，将异常代码转换为可读的字符串
static std::string get_seh_description(unsigned int code) {
    switch (code) {
    case EXCEPTION_ACCESS_VIOLATION:
        return "Access Violation";
    case EXCEPTION_BREAKPOINT:
        return "Breakpoint";
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        return "Datatype Misalignment";
    case EXCEPTION_SINGLE_STEP:
        return "Single Step";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        return "Array Bounds Exceeded";
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        return "Float Denormal Operand";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        return "Float Divide by Zero";
    case EXCEPTION_FLT_INEXACT_RESULT:
        return "Float Inexact Result";
    case EXCEPTION_FLT_INVALID_OPERATION:
        return "Float Invalid Operation";
    case EXCEPTION_FLT_OVERFLOW:
        return "Float Overflow";
    case EXCEPTION_FLT_STACK_CHECK:
        return "Float Stack Check";
    case EXCEPTION_FLT_UNDERFLOW:
        return "Float Underflow";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        return "Integer Divide by Zero";
    case EXCEPTION_INT_OVERFLOW:
        return "Integer Overflow";
    case EXCEPTION_PRIV_INSTRUCTION:
        return "Privileged Instruction";
    case EXCEPTION_IN_PAGE_ERROR:
        return "In Page Error";
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        return "Illegal Instruction";
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        return "Noncontinuable Exception";
    case EXCEPTION_STACK_OVERFLOW:
        return "Stack Overflow";
    case EXCEPTION_INVALID_DISPOSITION:
        return "Invalid Disposition";
    case EXCEPTION_GUARD_PAGE:
        return "Guard Page";
    case EXCEPTION_INVALID_HANDLE:
        return "Invalid Handle";
    default:
        return "Unknown SEH Exception";
    }
}


// SEH_Exception 类的实现
SEH_Exception::SEH_Exception(unsigned int nCode, _EXCEPTION_POINTERS* pExcPointers)
: std::runtime_error(""), // what() 将在下面被重写
  m_nSehCode(nCode),
  m_pExceptionAddress(pExcPointers->ExceptionRecord->ExceptionAddress) {

    // 创建一个详细的错误信息
    std::stringstream ss;
    ss << "SEH Exception caught!\n"
       << "  Description: " << get_seh_description(m_nSehCode) << "\n"
       << "  Code: 0x" << std::hex << m_nSehCode << "\n"
       << "  Address: 0x" << m_pExceptionAddress;

    // std::runtime_error 的 what() 消息是通过其基类构造函数设置的
    static_cast<std::runtime_error&>(*this) = std::runtime_error(ss.str());
}

unsigned int SEH_Exception::getSehCode() const { return m_nSehCode; }

void* SEH_Exception::getExceptionAddress() const { return m_pExceptionAddress; }

void SEH_Exception::setup() {
    // 设置进程级别的 SEH 转换函数
    _set_se_translator(se_translator_function);
}