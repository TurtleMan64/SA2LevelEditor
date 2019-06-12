#include <Windows.h>
#include <vector>

#include "emeraldf.h"
#include "emerald.h"
#include "../../main/main.h"

EMERALD_F::EMERALD_F()
{

}

EMERALD_F::EMERALD_F(char data[32], bool useDefaultValues) : EMERALD(data, useDefaultValues)
{

}

void EMERALD_F::updateEditorWindows()
{
    EMERALD::updateEditorWindows();
    SetWindowTextA(Global::windowValues[1], "EMERALD_F");
    SetWindowTextA(Global::windowDescriptions[1], "Like a normal treasure hunting item, but has a further draw distance.");
}
