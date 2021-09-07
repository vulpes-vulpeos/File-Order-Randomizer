# File Order Randomizer (macOS)
A simple script and app that can randomize files order by adding random numbers at the beginning of files names.
It’s useful if your car is too dumb to shuffle a lot of music properly.
Also they can remove symbols from the beginning of the file name.

**AppleScript:**

You can create an "app" with Script Editor. Open .scpt file, in menu bar go File -> Export... . For File Format select Application, for Code Sign select Sign to Run Locally. 
Keyboard shortcuts (cmd+a, cmd+v etc.) stop working if the .app is created in Automator.

Toolbox.scpt also contains an option to automaticaly move to trash files which names contain this russian letters or artist names and to download a video with youtube-dl.

**C++/AppleScript:**

This version is fully automatic. All you need to do is to select folder and choose what you want to do with files in the folder. The app will automatically count how many digits random number needs, it will detect if file was already randomized and rerandomize it. While unrandomizing it ignores files which do not start with number and automatically counts how many numbers it need to delete (supported separators: "_", ". ").

You can compile it with g++: ```g++ -std=c++2a -O2 -Wall \"${file}\"  -o \"${file_path}/${file_base_name}\" ```
Or download .app bundle from release section. You'll need to allow it's launch in Settings -> Security &Privacy -> General tab.

Tested on macOS Big Sur.
