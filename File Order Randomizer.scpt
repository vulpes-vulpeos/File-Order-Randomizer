on run
	
	display dialog "What do you want to do?" with title "File order randomizer" buttons {"Remove symbols", "Randomize order", "Exit"} cancel button "Exit"
	if the button returned of the result is "Remove symbols" then
		set SymbolsNumberChoisesRS to {"2", "3", "4", "5", "6", "7"}
		set SymbolNumberChoiseRS to choose from list SymbolsNumberChoisesRS with prompt "Select how many symbols you want to remove:" default items {"5"} cancel button name "Exit"
		if SymbolNumberChoiseRS is false then
			stop
		else
			set SymbolNumberChoiseReal to SymbolNumberChoiseRS + 1
			set userFolder to path to home folder from user domain
			set whichFile to choose file default location userFolder with prompt "Choose files to remove " & (SymbolNumberChoiseRS as text) & " first symbols:" with multiple selections allowed
			repeat with aFile in whichFile
				tell application "Finder"
					set filename to name of aFile
					set name of aFile to ((characters SymbolNumberChoiseReal thru -1 of filename) as string)
				end tell
			end repeat
			display dialog "Finished" with title "File order randomizer" buttons {"Return", "Exit"} cancel button "Exit"
			if the button returned of the result is "Return" then
				return on run
			end if
		end if
	else
		set SymbolsNumberChoisesRO to {"2", "3", "4", "5", "6", "7"}
		set SymbolNumberChoiseRO to choose from list SymbolsNumberChoisesRO with prompt "Select how many digits you want to add:" default items {"4"} cancel button name "Exit"
		if (SymbolNumberChoiseRO = {"2"}) then
			set userFolder to path to home folder from user domain
			set whichFile to choose file default location userFolder with prompt "Files to add " & SymbolNumberChoiseRO & " random numbers:" with multiple selections allowed
			repeat with aFile in whichFile
				tell application "Finder"
					set filename to name of aFile
					set name of aFile to ((random number from 10 to 99) as text) & ("_" as text) & aFile's name
				end tell
			end repeat
			display dialog "Finished" with title "File order randomizer" buttons {"Return", "Exit"} cancel button "Exit"
			if the button returned of the result is "Return" then
				return on run
			end if
		else if (SymbolNumberChoiseRO = {"3"}) then
			set userFolder to path to home folder from user domain
			set whichFile to choose file default location userFolder with prompt "Files to add " & SymbolNumberChoiseRO & " random numbers:" with multiple selections allowed
			repeat with aFile in whichFile
				tell application "Finder"
					set filename to name of aFile
					set name of aFile to ((random number from 100 to 999) as text) & ("_" as text) & aFile's name
				end tell
			end repeat
			display dialog "Finished" with title "File order randomizer" buttons {"Return", "Exit"} cancel button "Exit"
			if the button returned of the result is "Return" then
				return on run
			end if
		else if (SymbolNumberChoiseRO = {"4"}) then
			set userFolder to path to home folder from user domain
			set whichFile to choose file default location userFolder with prompt "Files to add " & SymbolNumberChoiseRO & " random numbers:" with multiple selections allowed
			repeat with aFile in whichFile
				tell application "Finder"
					set filename to name of aFile
					set name of aFile to ((random number from 1000 to 9999) as text) & ("_" as text) & aFile's name
				end tell
			end repeat
			display dialog "Finished" with title "File order randomizer" buttons {"Return", "Exit"} cancel button "Exit"
			if the button returned of the result is "Return" then
				return on run
			end if
		else if (SymbolNumberChoiseRO = {"5"}) then
			set userFolder to path to home folder from user domain
			set whichFile to choose file default location userFolder with prompt "Files to add " & SymbolNumberChoiseRO & " random numbers:" with multiple selections allowed
			repeat with aFile in whichFile
				tell application "Finder"
					set filename to name of aFile
					set name of aFile to ((random number from 10000 to 99999) as text) & ("_" as text) & aFile's name
				end tell
			end repeat
			display dialog "Finished" with title "File order randomizer" buttons {"Return", "Exit"} cancel button "Exit"
			if the button returned of the result is "Return" then
				return on run
			end if
		else if (SymbolNumberChoiseRO = {"6"}) then
			set userFolder to path to home folder from user domain
			set whichFile to choose file default location userFolder with prompt "Files to add " & SymbolNumberChoiseRO & " random numbers:" with multiple selections allowed
			repeat with aFile in whichFile
				tell application "Finder"
					set filename to name of aFile
					set name of aFile to ((random number from 100000 to 999999) as text) & ("_" as text) & aFile's name
				end tell
			end repeat
			display dialog "Finished" with title "File order randomizer" buttons {"Return", "Exit"} cancel button "Exit"
			if the button returned of the result is "Return" then
				return on run
			end if
		else if (SymbolNumberChoiseRO = {"7"}) then
			set userFolder to path to home folder from user domain
			set whichFile to choose file default location userFolder with prompt "Files to add " & SymbolNumberChoiseRO & " random numbers:" with multiple selections allowed
			repeat with aFile in whichFile
				tell application "Finder"
					set filename to name of aFile
					set name of aFile to ((random number from 1000000 to 9999999) as text) & ("_" as text) & aFile's name
				end tell
			end repeat
			display dialog "Finished" with title "File order randomizer" buttons {"Return", "Exit"} cancel button "Exit"
			if the button returned of the result is "Return" then
				return on run
			end if
		else if SymbolNumberChoiseRO is false then
			stop
		end if
	end if
	return input
end run