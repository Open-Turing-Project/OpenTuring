% The "ExpressionEvaluator" program.
var token : string

% The following procedures are declared forward because expn calls
% term, term calls primary, and primary calls expn.
forward procedure expn (var eValue : real)
forward procedure term (var tValue : real)
forward procedure primary (var pValue : real)

% Each of expn, term, and primary are defined "body" (and with no
% parameters) because they have been previously declared in the
% forward statement. 
body procedure expn
    var nextValue : real
    term (eValue)                   % Evaluate t
    loop                            % Evaluate { + t}
	exit when token not= "+"
	get token
	term (nextValue)
	eValue := eValue + nextValue
    end loop
end expn

body procedure term
    var nextValue : real
    primary (tValue)                % Evaluate p
    loop                            % Evaluate { * p}
	exit when token not= "*"
	get token
	primary (nextValue)
	tValue := tValue * nextValue
    end loop
end term

body procedure primary
    if token = "(" then
	get token
	expn (pValue)               % Evaluate (e)
	assert token = ")"
    else                            % Evaluate "explicit real"
	pValue := strreal (token)
    end if
    get token
end primary

put "Enter an expression ending with '=' containing:"
put "       real numbers, +, *, (, and )"
put "Note that elements must have spaces between them.", skip
put "Legal example: '1.5 + 3.0 * ( 0.5 + 1.5 ) ='"
put "Illegal example: '( 3 * 2.5) + 4 ='   [no space between the '2.5' and the ')']"
put skip, "Expression: " ..
get token                       % Start by reading first token
var answer : real
expn (answer)                   % Scan and evaluate input expression
put "Answer is ", answer
