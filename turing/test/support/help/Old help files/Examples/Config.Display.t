% The "Config.Display" program.
const width : int := Config.Display (cdScreenWidth)
const height : int := Config.Display (cdScreenHeight)
put "The screen width is ", width, " and the screen height is ", height
put "(note that the height does not include the task bar, if applicable)"
