# File Order Randomizer (macOS)
A simple script that can randomize files order by adding random numbers at the beginning of files names.
It’s useful if your car is too dumb to shuffle a lot of music properly.
Also this script can remove symbols from the beginning of the file name.

You can create an "app" with automator. Create new application, drag "run apple script" action, paste text from File Order Randomizer.scpt file and save. There is simple icon in repository if you don't like automator icon.

Tested on macOS Big Sur.

File order randomizer+.scpt also contains an option to automaticaly move to trash files which names contain this russian letters or artist names: {"а", "е", "ё", "и", "о", "у", "э", "ю", "я", "MORGENSHTERN", "V $ X V Prince"}.
