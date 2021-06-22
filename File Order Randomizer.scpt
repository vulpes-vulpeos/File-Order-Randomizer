on run
	
	display dialog "What do you want to do?" with title "File order randomizer" buttons {"Remove symbols", "Randomize order", "Exit"} cancel button "Exit"
	if the button returned of the result is "Remove symbols" then
		set SymbolsNumberChoisesRS to {"2", "3", "4", "5", "6", "7", "8"}
		set SymbolNumberChoiseRS to choose from list SymbolsNumberChoisesRS with prompt "Select how many symbols you want to remove:" default items {"5"} cancel button name "Exit"
		if SymbolNumberChoiseRS is false then
			stop
		else
			set userFolder to path to home folder from user domain
			set whichFile to choose file default location userFolder with prompt "Choose files to remove " & (SymbolNumberChoiseRS as text) & " first symbols:" with multiple selections allowed
			repeat with aFile in whichFile
				tell application "Finder"
					set name of aFile to text (SymbolNumberChoiseRS + 1) thru -1 of (get name of aFile)
				end tell
			end repeat
			display dialog "Finished" with title "File order randomizer" buttons {"Return", "Exit"} cancel button "Exit"
			if the button returned of the result is "Return" then
				return on run
			end if
		end if
		
	else if the button returned of the result is "Randomize order" then
		set SymbolsNumberChoisesRO to {"2", "3", "4", "5", "6", "7"}
		set SymbolNumberChoiseRO to choose from list SymbolsNumberChoisesRO with prompt "Select how many digits you want to add:" default items {"4"} cancel button name "Exit"
		if (SymbolNumberChoiseRO = {"2"}) then
			set RangeStart to 10
			set RangeEnd to 99
		else if (SymbolNumberChoiseRO = {"3"}) then
			set RangeStart to 100
			set RangeEnd to 999
		else if (SymbolNumberChoiseRO = {"4"}) then
			set RangeStart to 1000
			set RangeEnd to 9999
		else if (SymbolNumberChoiseRO = {"5"}) then
			set RangeStart to 10000
			set RangeEnd to 99999
		else if (SymbolNumberChoiseRO = {"6"}) then
			set RangeStart to 100000
			set RangeEnd to 999999
		else if (SymbolNumberChoiseRO = {"7"}) then
			set RangeStart to 1000000
			set RangeEnd to 9999999
		end if
		
		if SymbolNumberChoiseRO is false then
			stop
		else
			set userFolder to path to home folder from user domain
			set whichFile to choose file default location userFolder with prompt "Files to add " & SymbolNumberChoiseRO & " random numbers:" with multiple selections allowed
			repeat with aFile in whichFile
				tell application "Finder"
					set name of aFile to ((random number from RangeStart to RangeEnd) as text) & ("_" as text) & (name of aFile as text)
				end tell
			end repeat
			display dialog "Finished" with title "File order randomizer" buttons {"Return", "Exit"} cancel button "Exit"
			if the button returned of the result is "Return" then
				return on run
			end if
		end if
	end if
end run
