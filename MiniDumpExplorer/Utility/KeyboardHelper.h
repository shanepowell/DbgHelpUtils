#pragma once

class KeyboardHelper
{
public:
    struct MetaKeyState1
    {
        bool ctrl;
        bool shift;
    };

    static MetaKeyState1 GetMetaKeyState1();

    struct MetaKeyState2
    {
        bool ctrl;
        bool shift;
        bool alt;
    };

    static MetaKeyState2 GetMetaKeyState2();
};
