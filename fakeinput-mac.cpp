#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include "fakeinput.h"

//note: compile with: gcc -framework ApplicationServices

namespace FakeInput {

void initFakeInput() {
}

void freeFakeInput() {
}

void doMouseEvent(CGEventType type, int addX, int addY, CGMouseButton button) {
    CGEventRef getPos = CGEventCreate(NULL);
    CGPoint cursor = CGEventGetLocation(getPos);
    CFRelease(getPos);
    int curX = cursor.x;
    int curY = cursor.y;

    CGPoint absPos = CGPointMake(curX + addX, curY + addY);
    if(absPos.x < 0) absPos.x = 0;
    if(absPos.y < 0) absPos.y = 0;
    // Todo: support multiple monitors if this doesn't.
    // Will fix when i have a usable VM. fuck.
    CGRect mainMonitor = CGDisplayBounds(CGMainDisplayID());
    CGFloat monitorHeight = CGRectGetHeight(mainMonitor);
    CGFloat monitorWidth = CGRectGetWidth(mainMonitor);
    if(absPos.x > monitorWidth) absPos.x = monitorWidth;
    if(absPos.y > monitorHeight) absPos.y = monitorHeight;

    CGEventRef event = CGEventCreateMouseEvent(NULL, type, absPos, button);
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
}

void doKeyboardEvent(CGKeyCode key, bool down) {
    CGEventRef event = CGEventCreateKeyboardEvent(NULL, key, down);
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
}

void typeUniChar(wchar_t c) {
	CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
	CGEventRef down_evt = CGEventCreateKeyboardEvent(src, (CGKeyCode) 0, true);
	CGEventRef up_evt = CGEventCreateKeyboardEvent(src, (CGKeyCode) 0, false);

	UniChar str[] = {(UniChar)c, '\0'};
	CGEventKeyboardSetUnicodeString(down_evt, 1, str);
	CGEventKeyboardSetUnicodeString(up_evt, 1, str);

	CGEventPost (kCGHIDEventTap, down_evt);
	CGEventPost (kCGHIDEventTap, up_evt);

    CFRelease (down_evt);
    CFRelease (up_evt);
    CFRelease (src);
}

void typeChar(wchar_t c) {
    if(c == '\n')
        keyTap("Return");
    else
        typeUniChar(c);
}

void typeString(wchar_t *string) {
	int i = 0;
	while(string[i] != '\0') {
		typeChar(string[i++]);
	}
}

#define EQ(K) ( strcmp(keyName, K) == 0 )
CGKeyCode getSpecialKey(char *keyName)
{
    if( EQ("Return") )
        return kVK_Return;
    else if( EQ("Ctrl") )
        return kVK_Control;
    else if( EQ("Tab") )
        return kVK_Tab;
    else if( EQ("BackSpace") )
        return kVK_Delete;
    else if( EQ("Esc") )
        return kVK_Escape;
    else if( EQ("VolumeUp") )
        return kVK_VolumeUp;
    else if( EQ("VolumeDown") )
        return kVK_VolumeDown;
    else if( EQ("Left") )
        return kVK_LeftArrow;
    else if( EQ("Right") )
        return kVK_RightArrow;
    else if( EQ("Up") )
        return kVK_UpArrow;
    else if( EQ("Down") )
        return kVK_DownArrow;
    else
        return (CGKeyCode)-1;
}

void keyTap(char *key) {
    keyDown(key);
    keyUp(key);
}

void keyDown(char *key) {
    doKeyboardEvent(getSpecialKey(key), true);
}

void keyUp(char *key) {
    doKeyboardEvent(getSpecialKey(key), false);
}

// need to keep track of buttons on mac for drag events.
bool buttonDown[] = {false, false, false};
void mouseMove(int addX, int addY) {
    CGEventType moveType = kCGEventMouseMoved;
    if(buttonDown[0]) moveType = kCGEventLeftMouseDragged;
    if(buttonDown[1]) moveType = kCGEventOtherMouseDragged;
    if(buttonDown[2]) moveType = kCGEventRightMouseDragged;
    doMouseEvent(moveType, addX, addY, (CGMouseButton)0);
}

CGMouseButton getCGButton(int button) {
    if(button == 1)
        return kCGMouseButtonLeft;
    else if(button == 2)
        return kCGMouseButtonCenter;
    else//if(button == 3)
        return kCGMouseButtonRight;
}

CGEventType getMouseEventType(int button, bool down) {
    if(button == 1)
        return down? kCGEventLeftMouseDown:kCGEventLeftMouseUp;
    else if(button == 2)
        return down? kCGEventOtherMouseDown:kCGEventOtherMouseUp;
    else//if(button == 3)
        return down? kCGEventRightMouseDown:kCGEventRightMouseUp;
}

void mouseDown(int button) {
    if(button >= 0 && button <= 3)
        buttonDown[button-1] = true;
    doMouseEvent(getMouseEventType(button, true), 0, 0, getCGButton(button));
}

void mouseUp(int button) {
    if(button >= 0 && button <= 3)
        buttonDown[button-1] = false;
    doMouseEvent(getMouseEventType(button, false), 0, 0, getCGButton(button));
}

void mouseScroll(int amount) {
    CGEventRef scroll = CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitLine, 1, -amount);
    CGEventPost(kCGHIDEventTap, scroll);
    CFRelease(scroll);
}

//end of namespace
}
