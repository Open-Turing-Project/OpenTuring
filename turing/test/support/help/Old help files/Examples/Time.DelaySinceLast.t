% The "Time.DelaySinceLast" program.
var t : int := Time.Elapsed
put "Starting now, count to 100,000"
for i : 1 .. 100000
    put i ..
    locate (whatrow, 1)
end for
put "Finished counting to 100,000"
put "Elapsed time: ", Time.Elapsed - t, " milliseconds"
put "Now call Time.DelaySinceLast for 10 seconds"
Time.DelaySinceLast (10000)
put "Elapsed time: ", Time.Elapsed - t, " milliseconds"
