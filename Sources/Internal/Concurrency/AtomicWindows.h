#pragma once

#include "Base/Platform.h"
#ifndef USE_CPP11_CONCURRENCY

#include "Concurrency/Atomic.h"

namespace DAVA
{

#if defined(__DAVAENGINE_WINDOWS__)

namespace Detail
{
template <size_t N>
struct TypeSelector;

template <>
struct TypeSelector<sizeof(CHAR)>
{
    using Type = CHAR;
};
template <>
struct TypeSelector<sizeof(SHORT)>
{
    using Type = SHORT;
};
template <>
struct TypeSelector<sizeof(LONG)>
{
    using Type = LONG;
};
template <>
struct TypeSelector<sizeof(LONGLONG)>
{
    using Type = LONGLONG;
};

void DVMemBarrier();

// 8-bit
inline CHAR AtomicIncrement(volatile CHAR* value)
{
    return (CHAR)__sync_add_and_fetch((volatile char*)value, 1);
}
inline CHAR AtomicDecrement(volatile CHAR* value)
{
    return (CHAR)__sync_sub_and_fetch((volatile char*)value, 1);
}
inline CHAR AtomicSwap(volatile CHAR* target, CHAR desired)
{
    return (CHAR)__sync_lock_test_and_set((volatile char*)target, desired);
}
inline CHAR AtomicCAS(volatile CHAR* target, CHAR expected, CHAR desired)
{
    return (CHAR)__sync_val_compare_and_swap((volatile char*)target, expected, desired);
}

// 16-bit
inline SHORT AtomicIncrement(volatile SHORT* value)
{
    return (SHORT)__sync_add_and_fetch((volatile short*)value, 1);
}
inline SHORT AtomicDecrement(volatile SHORT* value)
{
    return (SHORT)__sync_sub_and_fetch((volatile short*)value, 1);
}
inline SHORT AtomicSwap(volatile SHORT* target, SHORT desired)
{
    return (SHORT)__sync_lock_test_and_set((volatile short*)target, desired);
}
inline SHORT AtomicCAS(volatile SHORT* target, SHORT expected, SHORT desired)
{
    return (SHORT)__sync_val_compare_and_swap((volatile short*)target, expected, desired);
}

// 32-bit
inline LONG AtomicIncrement(volatile LONG* value)
{
    return (LONG)__sync_add_and_fetch((volatile long*)value, 1);
}
inline LONG AtomicDecrement(volatile LONG* value)
{
    return (LONG)__sync_sub_and_fetch((volatile long*)value, 1);
}
inline LONG AtomicSwap(volatile LONG* target, LONG desired)
{
    return (LONG)__sync_lock_test_and_set((volatile long*)target, desired);
}
inline LONG AtomicCAS(volatile LONG* target, LONG expected, LONG desired)
{
    return (LONG)__sync_val_compare_and_swap((volatile long*)target, expected, desired);
}

// 64-bit
inline LONGLONG AtomicIncrement(volatile LONGLONG* value)
{
    return (LONGLONG)__sync_add_and_fetch((volatile long long*)value, 1);
}
inline LONGLONG AtomicDecrement(volatile LONGLONG* value)
{
    return (LONGLONG)__sync_sub_and_fetch((volatile long long*)value, 1);
}
inline LONGLONG AtomicSwap(volatile LONGLONG* target, LONGLONG desired)
{
    return (LONGLONG)__sync_lock_test_and_set((volatile long long*)target, desired);
}
inline LONGLONG AtomicCAS(volatile LONGLONG* target, LONGLONG expected, LONGLONG desired)
{
    return (LONGLONG)__sync_val_compare_and_swap((volatile long long*)target, expected, desired);
}

} //  namespace Detail

template <typename T>
void Atomic<T>::Set(T val) DAVA_NOEXCEPT
{
    Swap(val);
}

template <typename T>
T Atomic<T>::Get() const DAVA_NOEXCEPT
{
    T val = value;
    Detail::DVMemBarrier();
    return val;
}

template <typename T>
T Atomic<T>::GetRelaxed() const DAVA_NOEXCEPT
{
    volatile T val = value;
    return val;
}

template <typename T>
T Atomic<T>::Increment() DAVA_NOEXCEPT
{
    using Type = typename Detail::TypeSelector<sizeof(T)>::Type;
    Type* val = reinterpret_cast<Type*>(&value);
    T result = Cast(Detail::AtomicIncrement(val));
    return result;
}

template <typename T>
T Atomic<T>::Decrement() DAVA_NOEXCEPT
{
    using Type = typename Detail::TypeSelector<sizeof(T)>::Type;
    Type* val = reinterpret_cast<Type*>(&value);
    T result = Cast(Detail::AtomicDecrement(val));
    return result;
}

template <typename T>
T Atomic<T>::Swap(T desired) DAVA_NOEXCEPT
{
    using Type = typename Detail::TypeSelector<sizeof(T)>::Type;
    Type* val = reinterpret_cast<Type*>(&value);
    Type des = static_cast<Type>(desired);
    T result = Cast(Detail::AtomicSwap(val, des));
    return result;
}

template <typename T>
bool Atomic<T>::CompareAndSwap(T expected, T desired) DAVA_NOEXCEPT
{
    using Type = typename Detail::TypeSelector<sizeof(T)>::Type;
    Type* val = reinterpret_cast<Type*>(&value);
    Type exp = static_cast<Type>(expected);
    Type des = static_cast<Type>(desired);
    T result = Cast(Detail::AtomicCAS(val, exp, des));
    return result == expected;
}

template <typename T>
template <typename Y>
T Atomic<T>::Cast(Y val)
{
    return static_cast<T>(val);
}

template <>
template <typename Y>
bool Atomic<bool>::Cast(Y val)
{
    return val != 0;
}

#endif //  __DAVAENGINE_WINDOWS__

} //  namespace DAVA

#endif //  !USE_CPP11_CONCURRENCY