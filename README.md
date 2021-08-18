# File Order Randomizer (macOS)
A simple script and app that can randomize files order by adding random numbers at the beginning of files names.
It’s useful if your car is too dumb to shuffle a lot of music properly.
Also they can remove symbols from the beginning of the file name.

**AppleScript:**

You can create an "app" with Script Editor. Open .scpt file, in menu bar go File -> Export... . For File Format select Application, for Code Sign select Sign to Run Locally. 
Keyboard shortcuts (cmd+a, cmd+v etc.) stop working if the .app is created in Automator.

Toolbox.scpt also contains an option to automaticaly move to trash files which names contain this russian letters or artist names and to download a video with youtube-dl.

**C++/gtkmm4:**

You need gtkmm4 installed for it to run. If you have homebrew installed just type into terminal: ```brew install gtkmm4```

You can compile it with g++: ```g++ -std=c++2a -Wall \"${file}\" -o \"${file_path}/${file_base_name}\"  `pkg-config gtkmm-4.0 --cflags --libs` ```

To hide terminal window you need to pack main file into minimal .app bundle:
File Order Randomizer.app
- Contents
   - MacOS/main
   - Resources/main.icns
   - Info.plist

There is simple icon in repository if you need one.

There are commented strings for Linux Mint if you want to compile this app for it. Don't forget to // macOS strings.

Tested on macOS Big Sur.
