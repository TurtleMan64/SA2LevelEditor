#ifndef EMERALDF_H
#define EMERALDF_H

#include "emerald.h"

class EMERALD_F : public EMERALD
{
public:
    EMERALD_F();
    EMERALD_F(char data[32], bool useDefaultValues);

    void updateEditorWindows();
};
#endif
