% The "RandomNotes" program
% Plays sixteenth notes in C scale at random
const CScale := "CDEFGAB"
play ("6") % Set to play sixteenth notes
var note : int
loop
    randint (note, 1, 7)     % Pick a note at random
    play (CScale (note))
end loop
