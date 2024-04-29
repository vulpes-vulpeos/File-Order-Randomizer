# File Order Randomizer (macOS)
A simple script and app that can randomize files order by adding random numbers at the beginning of files names.
It’s useful if your car is too dumb to shuffle a lot of music properly.

It automatically count how many digits in random number is needed, it will detect if file was already randomized and rerandomize it. While unrandomizing it ignores files which do not start with number and automatically counts how many numbers it needs to delete (supported separators: "_", ". ").

**AppleScript:**

You can create an "app" with Script Editor. Open .scpt file, in menu bar go File -> Export... . For File Format select Application, for Code Sign select Sign to Run Locally.

**Bash:**

use ```chmod u+x forand.sh``` to make it executable.
Probably it will also work on Linux.

Usage:
```
["/path/to/forand.sh"] [-fur] ["/path/to/folder"]
-r  randomize files in folder.
-u  unrandomize files in folder.
-f  remove files which contain cyrillic letters in their file names.
```


**AppleScript and C++:**

You can compile it with g++: ```g++ -std=c++2a -O2 -Wall \"${file}\"  -o \"${file_path}/${file_base_name}\" ```
Or download .app bundle from release section. You'll need to allow it's launch in Settings -> Security & Privacy -> General tab.

This version uses applescript dialogs as its UI.


**CLI C++: (macOS & Linux)**

You can compile it with g++: ```g++ -std=c++2a -O2 -Wall \"${file}\"  -o \"${file_path}/${file_base_name}\" ```.

This version has command-line interface and supports external filter list. Put "forand_filterlist.txt" near executable.\
Supported separators: "_", ". ", " - ".

Usage:
```
["/path/to/forand/executable"] [-furs] ["/path/to/folder"]
-r  randomize files in folder.
-u  unrandomize files in folder.
-f  remove files which contain strings/characters from filter list in their file names.
-s  serialize files in folder.
```


**CLI Go: (macOS & Linux)**

You can compile it with: ```go build -ldflags \"-w -s\" '${file}' ```.

This version has command-line interface and supports external filter list. Put "forand_filterlist.txt" near executable.\
Supported separators: "_", ". ", " - ".

Usage:
```
["/path/to/forand/executable"] [-furs] ["/path/to/folder"]
-f  move files which contain strings/characters from filter list in their file names in "filtered" folder.
-u  unrandomize files in folder.
-r  randomize files in folder.
-s  serialize files in folder.
```


**CLI C: (macOS & Linux)**

You can compile it with gcc: ```gcc "path/to/forand.c" -o "output/path" ```.

This version has command-line interface and supports external filter list. Put "forand_filterlist.txt" near executable.\
Supported separators: "_", ". ", " - ".

Usage:
```
["/path/to/forand/executable"] [-furs] ["/path/to/folder"]
-f  move files which contain strings/characters from filter list in their file names in "filtered" folder.
-u  unrandomize files in folder.
-r  randomize files in folder.
-s  serialize files in folder.
```


Tested on m1 Macbook Air with macOS Ventura and HP EliteDesk 800 G3 DM 35W with Debian Sid.

Idea was taken from this windows app: <https://sourceforge.net/projects/filerandomizer/>
