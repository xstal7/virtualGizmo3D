# virtualGizmo3D
**virtualGizmo3D** is an 3D GIZMO manipulator: it provides a way to rotate, move and scale a model, with mouse.
You can also define a way to rotate the model around any single axis.
It use mouse movement on screen, mouse buttons and (eventually) key modifiers, like Shift/Ctrl/Alt/Super

![alt text](https://raw.githubusercontent.com/BrutPitt/virtualGizmo3D/master/screenshots/oglGizmo.gif)

**virtualGizmo3D** internally uses quaternions to rotate the model, but You can also only pass your model matrix and gat back a transormation matrix with rotation, translazion and scale, inside.

**virtualGizmo3D** is an *header only* tool. 
It uses the [**glm** mathematics library](https://github.com/g-truc/glm) (0.9.9 or higher), also it an *header only* tool.


To use **virtualGizmo3D** need to include `virtualGizmo.h` file in your code and declare an object of type vfGizmo3DClass, global or as member of your class 

```cpp
#include "virtualGizmo.h"

// Global or member class declaration
vfGizmo3DClass gizmo; 
vfGizmo3DClass &getGizmo() { return gizmo; }  //optional helper
```

In your 3D engine initalization declare (overriding default ones) your preferred controls
In this example I use GLFW, but it is simple to change it if you use SDL, other tools, or native OS access.

```cpp
void onInit()
{

    //If do not using GLFW, simply use your ID defines 
    //You can freely define any button or key modifier you prefer

    //For main manipulator/rotation
    getGizmo().setGizmoRotControl( (vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) 0 /* evNoModifier */ );

    //for pan and zoom/dolly
    getGizmo().setDollyControl((vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_CONTROL|GLFW_MOD_SHIFT);
    getGizmo().setPanControl(  (vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) 0);
    
    //If you need, for rotations around a single axis
    getGizmo().setGizmoRotXControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_SHIFT);
    getGizmo().setGizmoRotYControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_CONTROL);
    getGizmo().setGizmoRotZControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_ALT | GLFW_MOD_SUPER);


    // Now call viewportSize with the dimension of window/screen
    // It is need to set mouse sensitivity for rotation
    // You need to call it also in your "reshape" function: when resize the window (look below)

    getGizmo().viewportSize(GetWidth(), GetHeight());

}    
```

In your Mouse_Button_Callback function call:
```cpp
void onMouseButton(int button, int upOrDown, int x, int y)
{
    //  Call on mouse button event
    //      button:  your mouse button
    //      mod:     your modifier key -> CTRL, SHIFT, ALT, SUPER
    //      pressed: if button is pressed (TRUE) or released (FALSE)
    //      x, y:    mouse coordinates

    getGizmo().mouse((vgButtons) (button), (vgModifiers) theApp->getModifier(), 
                     upOrDown==GLFW_PRESS, x, y);

}
```

In your Mouse_Move_Callback function call:
```cpp
void onMotion(int x, int y)
{
    //  Call on motion event to communicate the position
    getGizmo().motion(x, y);
}
```

And in your Resize_Window_Callback function 
```cpp
void onReshape(GLint w, GLint h)
{
    // call it on resize window to re-align mouse sensitivity
    getGizmo().viewportSize(w, h);
}
```

And finally, in your render function (or where you prefer) you can get the transformations
```cpp
void onRender() //or when you prefer
{
    glm::mat4 model(1.0f);                          // Identity matrix

    // virtualGizmo transformations
    getGizmo().applyTransform(model);           // apply transform to matrix model

    //Now you can build MV and MVP matrix
}
```

 
### Live WebGL2 example
You can run/test an emscripten WebGL 2 example of **virtualGismo3D** from following link:
- [virtualGizmo3D WebGL2](https://michelemorrone.eu/www.michelemorrone.eu/emsExamples/oglGizmo.html)

It works only on browsers with WebGl 2 support (FireFox/Chrome/Opera)
Test here if your browser support WebGL 2: [WebGL 2 Report](http://webglreport.com/?v=2)


### Other useful stuff

If you need to more feeling with the mouse use:
`getGizmo().setGizmoFeeling(1.0); // 1.0 default,  > 1.0 more sensible, < 1.0 less`

Same thing for Dolly and Pan:

```cpp
 getGizmo().setDollyScale(1.0f);
 getGizmo().setPanScale(1.0f);
```
You probably will need to set center of rotation (default: origin), Dolly position (default: 1.0), and Pan position (default: vec2(0.0, 0.0)

```cpp
 getGizmo().setDollyPosition(1.0f); 
 getGizmo().setPanPosition(vec3(0.0f);
 getGizmo().setRotationCenter(vec3(0.0));
```

If you want a *continuous rotation*, that you can stop with a click, like in example, need to add the below call in your Idle function, or inside of the main render loop

```cpp
void onIdle()
{
    // call it every rendering if want an continue rotation until you do not click on screen
    // look at glApp.cpp : "mainLoop" ("newFrame") functions

    getGizmo().idle();
}
```

The include file `virtualGizmo.h` contains two classes:
- `virtualGizmoClass<T>` simple rotation manipulator, used mainly for imGuIZMO (a GIZMO widget developed for ImGui user intefrace)
- `virtualGizmo3DClass<T>` manipulator with dolly/zoom and pan/translatior

Helper `typedef` are also defined:
```cpp
typedef virtualGizmoClass<float>   vfGizmoClass;
typedef virtualGizmo3DClass<float> vfGizmo3DClass;

typedef virtualGizmoClass<double>   vdGizmoClass;
typedef virtualGizmo3DClass<double> vdGizmo3DClass;
```