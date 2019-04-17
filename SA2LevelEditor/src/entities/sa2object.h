#ifndef SA2OBJECT_H
#define SA2OBJECT_H

#include "entity.h"
#include "collideableobject.h"
#include <string>

class SA2Object : public CollideableObject
{
public:
    char rawData[32];
    int ID;

    enum TypeInterpretation {
        IS_INT,
        IS_FLOAT,
        NOT_USED
    };

    TypeInterpretation typeRotations;
    TypeInterpretation typePositions;
    TypeInterpretation typeVars;

	SA2Object();
	SA2Object(char data[32]);

    //call this whenever a new object is selected via a user click.
    // this will reach in to the editor window and update the values
    // with this objects properties (like position, id, etc.)
    virtual void updateEditorWindows();

    virtual int getIntRotationX();
    virtual int getIntRotationY();
    virtual int getIntRotationZ();
    virtual int getIntPositionX();
    virtual int getIntPositionY();
    virtual int getIntPositionZ();
    virtual int getIntVar1();
    virtual int getIntVar2();
    virtual int getIntVar3();

    virtual float getFloatRotationX();
    virtual float getFloatRotationY();
    virtual float getFloatRotationZ();
    virtual float getFloatPositionX();
    virtual float getFloatPositionY();
    virtual float getFloatPositionZ();
    virtual float getFloatVar1();
    virtual float getFloatVar2();
    virtual float getFloatVar3();

    virtual std::string getDescriptionRotationX();
    virtual std::string getDescriptionRotationY();
    virtual std::string getDescriptionRotationZ();
    virtual std::string getDescriptionPositionX();
    virtual std::string getDescriptionPositionY();
    virtual std::string getDescriptionVar1();
    virtual std::string getDescriptionVar2();
    virtual std::string getDescriptionVar3();
};
#endif
