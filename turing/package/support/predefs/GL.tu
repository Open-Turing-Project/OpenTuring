unit
module pervasive GL
    export all

    external "glgraph_newwin" fcn NewWin (width, height : int) : boolean
    external "glgraph_closewin" proc CloseWin ()

    external "glgraph_cls" proc Cls ()
    external "glgraph_update" proc Update ()
    
    % 1 is enabled 0 is disabled. Disabled by default
    external "glgraph_setlight" proc SetLight (enabled : int)

    % pushmatrix and popmatrix manipulate the matrix stack (you don't need to know how to use this.)
    % clearmatrix resets all transforms
    external "glgraph_pushmatrix" proc PushMatrix ()
    external "glgraph_popmatrix" proc PopMatrix ()
    external "glgraph_clearmatrix" proc ClearMatrix ()

    % transforms. These apply to anything drawn after they are called. These can be changed with the matrix functions.
    external "glgraph_scale" proc Scale (x, y, z : real)
    external "glgraph_translate" proc Translate (x, y, z : real)
    external "glgraph_rotate" proc Rotate (angle, x, y, z : real)

    % coordinates go from -1 to 1 on both axes for the default viewport.
    % this means square windows will have distorted coordinates.
    % you can fix this with a scale transform.
    external "glgraph_drawline" proc DrawLine (x1, y1, z1, x2, y2, z2 : real, r, g, b : int)
    external "glgraph_drawfilltriangle" proc DrawFillTriangle (x1, y1, z1, x2, y2, z2, x3, y3, z3 : real, r, g, b : int)
    external "glgraph_drawtriangle" proc DrawTriangle (x1, y1, z1, x2, y2, z2, x3, y3, z3 : real, r, g, b : int)
end GL
