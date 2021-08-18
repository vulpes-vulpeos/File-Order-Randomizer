on run
	main_menu()
end run

on main_menu()
	display dialog "What do you want to do?" with title "File order randomizer" buttons {"Remove symbols", "Randomize order", "Exit"} cancel button "Exit"
	if the button returned of the result is "Remove symbols" then
		remove_symbols_handler()
	else if the button returned of the result is "Randomize order" then
		randomize_order_handler()
	end if
end main_menu

on remove_symbols_handler()
	set remove_symbols_array to {"2", "3", "4", "5", "6", "7", "8"}
	set remove_symbols_user_choice to choose from list remove_symbols_array with prompt "Select how many symbols you want to remove:" with title "Toolbox" default items {"5"} cancel button name "Exit"
	if remove_symbols_user_choice is false then
		stop
	else
		set user_home_folder to path to home folder from user domain
		set selected_files to choose file default location user_home_folder with prompt "Choose files to remove " & (remove_symbols_user_choice as text) & " first symbols:" with multiple selections allowed
		-- remove symbols code source: https://apple.stackexchange.com/a/73023
		repeat with aFile in selected_files
			tell application "Finder"
				set name of aFile to text (remove_symbols_user_choice + 1) thru -1 of (get name of aFile)
			end tell
		end repeat
		finish_dialog()
	end if
end remove_symbols_handler

on randomize_order_handler()
	set randomize_order_array to {"2", "3", "4", "5", "6", "7"}
	set randomize_order_user_choice to choose from list randomize_order_array with prompt "Select how many digits you want to add:" with title "Toolbox" default items {"4"} cancel button name "Exit"
	if randomize_order_user_choice is false then
		stop
	else if (randomize_order_user_choice = {"2"}) then
		set range_start to 10
		set range_end to 99
	else if (randomize_order_user_choice = {"3"}) then
		set range_start to 100
		set range_end to 999
	else if (randomize_order_user_choice = {"4"}) then
		set range_start to 1000
		set range_end to 9999
	else if (randomize_order_user_choice = {"5"}) then
		set range_start to 10000
		set range_end to 99999
	else if (randomize_order_user_choice = {"6"}) then
		set range_start to 100000
		set range_end to 999999
	else if (randomize_order_user_choice = {"7"}) then
		set range_start to 1000000
		set range_end to 9999999
	end if
	
	set user_home_folder to path to home folder from user domain
	set selected_files to choose file default location user_home_folder with prompt "Files to add " & randomize_order_user_choice & " random numbers:" with multiple selections allowed
	repeat with aFile in selected_files
		tell application "Finder"
			set name of aFile to ((random number from range_start to range_end) as text) & ("_" as text) & (name of aFile as text)
		end tell
	end repeat
	finish_dialog()
end randomize_order_handler

on finish_dialog()
	display dialog "Finished" with title "Toolbox" buttons {"To main menu", "Exit"} cancel button "Exit"
	if the button returned of the result is "To main menu" then
		main_menu()
	end if
end finish_dialog
