% The "Voting" program
% Read votes and count totals
const sentinel := - 1
put "Vote 1 for Left, 2 for Middle, 3 for Right, end with ", sentinel
var vote : int
var left, middle, right : int := 0  % initialize all three to 0
const leftVote := 1
const midVote := 2
const rightVote := 3
loop
    put "Enter vote " ..
    get vote
    case vote of
	label leftVote :
	    left := left + 1
	label midVote :
	    middle := middle + 1
	label rightVote :
	    right := right + 1
	label sentinel :
	    exit
	label :
	    put "Invalid vote"
    end case
end loop
put "Left" : 8, "Middle" : 8, "Right" : 8
put left : 4, middle : 10, right : 7
