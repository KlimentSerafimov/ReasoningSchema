//
// Created by Kliment Serafimov on 8/18/20.
//

#ifndef COMPACTPOSET_POINTER_H
#define COMPACTPOSET_POINTER_H


template<typename T>
class Pointer
{
    T* pointer = nullptr;
public:
    Pointer(T* _pointer)
    {
        pointer = _pointer;
    }
    bool operator < (const Pointer& other) const
    {
        assert(pointer != nullptr);
        assert(other.pointer != nullptr);
        return pointer->operator<(other.pointer);
    }
    T* get_pointer() const
    {
        return pointer;
    }
};


#endif //COMPACTPOSET_POINTER_H
