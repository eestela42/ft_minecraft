#ifndef INTERFACE_INPUT_H
#define INTERFACE_INPUT_H

#define KEY_HOLD				(1 << 0)
#define KEY_PRESS				(1 << 1)

#define KEY_MOVE_FORWARD		1
#define KEY_MOVE_BACKWARD		2
#define KEY_MOVE_RIGHTWARD		3
#define KEY_MOVE_LEFTWARD		4
#define KEY_MOVE_UPWARD			5
#define KEY_MOVE_DOWNWARD		6

#define KEY_ENTER				7
#define KEY_ESCAPE				8

#define KEY_ROTATE_X_POSITIVE	9
#define KEY_ROTATE_X_NEGATIVE	10
#define KEY_ROTATE_Y_POSITIVE	11
#define KEY_ROTATE_Y_NEGATIVE	12
#define KEY_ROTATE_Z_POSITIVE	13
#define KEY_ROTATE_Z_NEGATIVE	14

#define KEY_SPACE				15

#define KEY_DELETE_ONE_BLOCK	16
#define KEY_DELETE_MORE_BLOCK	18


#define KEY_DISPLAY_INFO        17



#define KEY_LAST				18




class I_Input {
public:
    virtual void SendKeys(unsigned char *keyState, double mouseMoveX, double mouseMoveY) = 0;
};

#endif