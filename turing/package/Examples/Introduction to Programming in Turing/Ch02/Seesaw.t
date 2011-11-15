% The "Seesaw" program
% Makes saw tooth patterns
loop
    put "How many teeth do you want? (1-12) "
    var count : int
    get count
    put repeat ("*     ", count)
    put repeat (" *   *", count)
    put repeat ("  * * ", count)
    put repeat ("   *  ", count)
end loop
