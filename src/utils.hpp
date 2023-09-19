#ifndef _utils_hpp
#define _utils_hpp
#include <vector>

template<typename T, typename U> constexpr std::pair<ptrdiff_t, size_t> makeMemberPair(U T::*member)
{
    return std::pair((ptrdiff_t)&((T*)nullptr->*member), sizeof(U));
}

template<typename T> std::vector<std::byte> getValue(T* instance, std::pair<ptrdiff_t, size_t> memberPair) {
    std::vector<std::byte> out(memberPair.second);
    std::memcpy((void*)out.data(), (void*)((uintptr_t)instance + memberPair.first), memberPair.second);

    return out;
};

template<typename T> void restoreValue(T* instance, std::pair<ptrdiff_t, size_t> memberPair, std::vector<std::byte> value) {
    std::memcpy((void*)((uintptr_t)instance + memberPair.first), (void*)value.data(), memberPair.second);
};

#endif