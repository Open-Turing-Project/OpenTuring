% The "Input.Flush" program.
put "Press and hold a key down.  Then comment out the Input.Flush"
put "and run the program a second time.  The second time the keys"
put "keep echoing after the key is no longer pressed.", skip
loop
    if hasch then
	put getchar ..
	Input.Flush
	delay (100)
    end if
end loop
