% File "shape4.t".
% Draw one of four shapes
% depending on input from the user.
import Shape in "shape.tu"
var choice : int
const x := maxx div 2
const y := maxy div 2
const clr := red
const size := maxy div 4
const angle := 45
loop
    put "What shape do you want? "
    put "1. ball"
    put "2. block"
    put "3. moon"
    put "4. tiltSquare"
    put "5. quit"
    get choice
    Draw.Cls % Clears screen.
    case choice of
        label 1 : Shape.ball (x, y, size, clr)
        label 2 : Shape.block (x, y, size, clr)
        label 3 : Shape.moon (x, y, size, clr)
        label 4 : Shape.tiltSquare (x, y, size, angle, clr)
        label 5 : exit
        label : put "Please enter a number between 1 and 5"
    end case
    Shape.hold % Stops execution until any key pressed.
    Draw.Cls % Clears screen.
end loop
