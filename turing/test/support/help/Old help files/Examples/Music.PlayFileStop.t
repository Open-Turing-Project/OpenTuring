% The "Music.PlayFileStop" program.
% Loop playing background music until 'finished' is true.
var finished : boolean := false

process BackgroundMusic
    loop
	exit when finished
	Music.PlayFile ("%oot/support/help/Examples/Data Files/music.mid")
    end loop
end BackgroundMusic

fork BackgroundMusic        % Start the background music
Time.Delay (30000)          % Wait for 30 seconds

% Stop the background music.
finished := true            % The flag must be set first
Music.PlayFileStop          % Music.PlayFile will return immediately
