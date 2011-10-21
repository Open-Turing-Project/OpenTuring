/*
** RGB module - colour routines
**
** NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
**      IT WILL BE IMPLICITLY IMPORTED.
*/

unit
module pervasive RGB
    export GetColor, GetColour, SetColor, SetColour, AddColor, AddColour,
           ~.*maxcolour, ~.*maxcolor,

           ~.*black, ~.*blue, ~.*green, ~.*cyan, ~.*red, ~.*magenta,
           ~.*purple, ~.*brown, ~.*white, ~.*gray, ~.*grey, ~.*yellow,
           ~.*brightblue, ~.*brightgreen, ~.*brightcyan, ~.*brightred,
           ~.*brightmagenta, ~.*brightpurple, ~.*brightwhite,
           ~.*darkgrey, ~.*darkgray,
           ~.*colorfg, ~.*colourfg, ~.*colorbg, ~.*colourbg

    external "rgb_getcolour"
        procedure GetColor (colourNum: int, var red, blue, green: real)

    external "rgb_getcolour"
        procedure GetColour (colourNum: int, var red, blue, green: real)

    external "rgb_setcolour"
        procedure SetColor (colourNum: int, red, blue, green: real)

    external "rgb_setcolour"
        procedure SetColour (colourNum: int, red, blue, green: real)

    external "rgb_addcolour"
        function AddColor (red, blue, green: real): int

    external "rgb_addcolour"
        function AddColour (red, blue, green: real): int

/*
    external "rgb_matchcolour"
        function FindColor (red, blue, green: real): int

    external "rgb_matchcolour"
        function FindColour (red, blue, green: real): int
*/

    external "rgb_maxcolour" function maxcolour: int

    external "rgb_maxcolour" function maxcolor: int

    %
    % Colour constants
    %

    const *white            := 0
    const *blue             := 1
    const *green            := 2
    const *cyan             := 3
    const *red              := 4
    const *magenta          := 5
    const *purple           := magenta
    const *brown            := 6
    const *black            := 7
    const *gray             := 8
    const *grey             := gray
    const *brightblue       := 9
    const *brightgreen      := 10
    const *brightcyan       := 11
    const *brightred        := 12
    const *brightmagenta    := 13
    const *brightpurple     := brightmagenta
    const *yellow           := 14
    const *darkgray         := 15

    const *brightwhite      := gray
    const *darkgrey         := darkgray

    const *colorbg          := white
    const *colourbg         := colorbg
    const *colorfg          := black
    const *colourfg         := colorfg

end RGB

