# File Order Randomizer (macOS)
A simple script that can randomize files order by adding random numbers at the beginning of files names.
It’s useful if your car is too dumb to shuffle a lot of music properly.
Also this script can remove symbols from the beginning of the file name.

You can create an "app" with Script Editor. Open .scpt file, in menu bar go File -> Export... . For File Format select Application, for Code Sign select Sign to Run Locally. 
There is simple icon in repository if you don't like Script Editor icon. Keyboard shortcuts (cmd+a, cmd+v etc.) stop working if the .app is created in Automator.

Tested on macOS Big Sur.

Toolbox.scpt also contains an option to automaticaly move to trash files which names contain this russian letters or artist names and to download a video with youtube-dl.
