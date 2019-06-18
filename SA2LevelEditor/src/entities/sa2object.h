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
	SA2Object(char data[32], bool useDefaultValues);

    //call this whenever a new object is selected via a user click.
    // this will reach in to the editor window and update the values
    // with this objects properties (like position, id, etc.)
    virtual void updateEditorWindows();

    //when the user manually enters a new text value and clicks
    // the update button, this function gets called. it is the duty of
    // this function to parse the text from the corresponding button
    // and determine if it is an ok value, then update the button text
    // again and also update itself and redraw the screen.
    virtual void updateValue(int btnIndex);

    //fills the given data buffer with the data for this object.
    // this is used when exporting the objects back out to a setfile,
    // the exporter can just ask each SA2Object to fill out its
    // 32 byte buffer and write those to the set file
    virtual void fillData(char data[32]);

	//returns ID of switch or knudai (MM shrine) linked to the object, or -1 otherwise
	virtual int getSwitchID();
	virtual int getShrineID();

    bool isSA2Object();

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
