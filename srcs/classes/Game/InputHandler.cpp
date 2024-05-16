#include <classes/Game/InputHandler.hpp>

InputHandler::InputHandler(GLFWwindow *window) : window(window) {
    MapKeys();
    glfwGetCursorPos(window, &posX, &posY);
}

void InputHandler::AddCallback(I_Input *newCallback) {
    callbackVector.push_back(newCallback);
}

void InputHandler::HandleInput()
{
	double newPosX, newPosY, mouseMoveX, mouseMoveY;
    glfwGetCursorPos(window, &newPosX, &newPosY);
	mouseMoveX = newPosX - posX;
	mouseMoveY = newPosY - posY;
	posX = newPosX;
	posY = newPosY;

	for (u_int i = 0; i < GLFW_KEY_LAST; i++) {
		if (keyMap[i]) {
			u_int key = keyMap[i];
			int type = glfwGetKey(window, i);
			if (type == GLFW_PRESS) {
				if (!(keyState[key] & KEY_HOLD)) {
					keyState[key] = KEY_HOLD | KEY_PRESS;
				}
				else {
					keyState[key] &= ~KEY_PRESS;
				}
			}
			else if(type == GLFW_RELEASE) {
				keyState[key] = 0;
			}
		}
	}

	for (std::vector<I_Input*>::iterator iterator = callbackVector.begin(); iterator != callbackVector.end(); iterator++) {
		(*iterator)->SendKeys(keyState, mouseMoveX, mouseMoveY);
    }
}

void InputHandler::MapKeys() {
    keyMap[GLFW_KEY_W] = KEY_MOVE_FORWARD;
    keyMap[GLFW_KEY_S] = KEY_MOVE_BACKWARD;
    keyMap[GLFW_KEY_D] = KEY_MOVE_RIGHTWARD;
    keyMap[GLFW_KEY_A] = KEY_MOVE_LEFTWARD;
    keyMap[GLFW_KEY_LEFT_SHIFT] = KEY_MOVE_UPWARD;
    keyMap[GLFW_KEY_LEFT_CONTROL] = KEY_MOVE_DOWNWARD;

	

    keyMap[GLFW_KEY_ENTER] = KEY_ENTER;
    keyMap[GLFW_KEY_ESCAPE] = KEY_ESCAPE;
	keyMap[GLFW_KEY_SPACE] = KEY_SPACE;

    keyMap[GLFW_KEY_KP_8] = KEY_ROTATE_X_NEGATIVE;
    keyMap[GLFW_KEY_KP_5] = KEY_ROTATE_X_POSITIVE;
    keyMap[GLFW_KEY_KP_4] = KEY_ROTATE_Y_NEGATIVE;
    keyMap[GLFW_KEY_KP_6] = KEY_ROTATE_Y_POSITIVE;
    keyMap[GLFW_KEY_KP_7] = KEY_ROTATE_Z_NEGATIVE;
    keyMap[GLFW_KEY_KP_9] = KEY_ROTATE_Z_POSITIVE;
	keyMap[GLFW_KEY_Q] = KEY_DELETE_ONE_BLOCK;
	keyMap[GLFW_KEY_E] = KEY_DELETE_MORE_BLOCK;
	keyMap[GLFW_KEY_P] = KEY_DISPLAY_INFO;


}

InputHandler::~InputHandler() {}