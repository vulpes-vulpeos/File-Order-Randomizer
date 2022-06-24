--Copyright 2022 Vulpes-Vulpeos

--Licensed under the Apache License, Version 2.0 (the "License");
--you may not use this file except in compliance with the License.
--You may obtain a copy of the License at

--    http://www.apache.org/licenses/LICENSE-2.0

--Unless required by applicable law or agreed to in writing, software
--distributed under the License is distributed on an "AS IS" BASIS,
--WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
--See the License for the specific language governing permissions and
--limitations under the License.

on run {}
	set user_home_folder to path to home folder from user domain
	set selected_folder to choose folder default location user_home_folder with prompt "Select folder:"
	
	main_menu(selected_folder)
end run

on main_menu(selected_folder)
	set main_menu_array to {"Unrandomize order", "Randomize order", "Remove files with cyrillics characters in name"}
	set main_menu_user_choice to choose from list main_menu_array with prompt "What you want me to do?" with title "File Order Randomizer" default items {"Unrandomize order"} cancel button name "Exit"
	
	if main_menu_user_choice is false then
		stop
	else if (main_menu_user_choice = {"Unrandomize order"}) then
		unrandomize_order_handler(selected_folder)
	else if (main_menu_user_choice = {"Randomize order"}) then
		randomize_order_handler(selected_folder)
	else if (main_menu_user_choice = {"Remove files with cyrillics characters in name"}) then
		remove_cyrillic_handler(selected_folder)
	end if
	
end main_menu

on is_number(first_char)
	try
		set first_char to first_char as number
		return true
	on error
		return false
	end try
end is_number

on unrandomize_order_handler(selected_folder)
	--getin path to every file in folder into array
	tell application "Finder"
		set selected_files to (every item of folder selected_folder whose name does not start with ".")
	end tell
	
	--thru -1 means to the end of file
	repeat with aFile in selected_files
		tell application "Finder"
			set file_name to name of aFile as string
			--checking if first character of file name is number
			set first_char to text 1 thru 1 of file_name
			if my is_number(first_char) then
				--deleting numbers with separators in the begining of file name
				set trigger_index to (offset of "_" in file_name)
				if trigger_index is greater than 0 then
					set file_name to text (trigger_index + 1) thru -1 of file_name
				end if
				
				set trigger_index to (offset of ". " in file_name)
				if trigger_index is greater than 0 then
					set file_name to text (trigger_index + 2) thru -1 of file_name
				end if
			end if
			
			set name of aFile to file_name
		end tell
	end repeat
	
	display dialog "Finished. Continue with same files?" with title "File Order Randomizer" buttons {"Filter", "Randomize", "Exit"} cancel button "Exit"
	if the button returned of the result is "Filter" then
		remove_cyrillic_handler(selected_folder)
	else if the button returned of the result is "Randomize" then
		randomize_order_handler(selected_folder)
	end if
	
end unrandomize_order_handler

on randomize_order_handler(selected_folder)
	--getin path to every file in folder into array
	tell application "Finder"
		set selected_files to (every item of folder selected_folder whose name does not start with ".")
	end tell
	
	set number_of_selected_files to count of selected_files
	
	repeat with aFile in selected_files
		tell application "Finder"
			--generating random number
			set random_number to random number from 1 to number_of_selected_files
			--adding 0's to random number if it has less digits then in number of files number
			set random_number_digits to (get length of (random_number as string))
			set number_of_files_digits to (get length of (number_of_selected_files as string))
			set zeros_needed to number_of_files_digits - random_number_digits
			
			if zeros_needed is greater than 0 then
				repeat zeros_needed times
					set random_number to ((0 as string) & (random_number as string))
				end repeat
			end if
			
			set file_name to name of aFile as string
			
			--checking if first character of file name is number
			set first_char to text 1 thru 1 of file_name
			if my is_number(first_char) then
				--deleting numbers with separators in the begining of file name
				set trigger_index to (offset of "_" in file_name)
				if trigger_index is greater than 0 then
					set file_name to text (trigger_index + 1) thru -1 of file_name
				end if
				
				set trigger_index to (offset of ". " in file_name)
				if trigger_index is greater than 0 then
					set file_name to text (trigger_index + 2) thru -1 of file_name
				end if
			end if
			
			set name of aFile to (random_number as text) & ("_" as text) & (file_name as text)
		end tell
	end repeat
	
	display dialog "Finished. Continue with same files?" with title "File Order Randomizer" buttons {"Filter", "Unrandomize", "Exit"} cancel button "Exit"
	if the button returned of the result is "Filter" then
		remove_cyrillic_handler(selected_folder)
	else if the button returned of the result is "Unrandomize" then
		unrandomize_order_handler(selected_folder)
	end if
	
end randomize_order_handler

on remove_cyrillic_handler(selected_folder)
	set remove_cyrillic_names_array to {"а", "е", "ё", "и", "о", "у", "э", "ю", "я"}
	
	tell application "Finder"
		set number_of_cyrrilic_names to 0
		--getin path to every file in folder into array
		tell application "Finder"
			set selected_files to (every item of folder selected_folder whose name does not start with ".")
		end tell
		
		--moving file to trash if it has cyrillic characters in file name
		repeat with aFile in selected_files
			repeat with i from 1 to count of items of remove_cyrillic_names_array
				if name of aFile contains item i of remove_cyrillic_names_array then
					set number_of_cyrrilic_names to number_of_cyrrilic_names + 1
					move aFile to trash
					exit repeat
				end if
			end repeat
		end repeat
	end tell
	
	display dialog "Finished. Total number of files moved to trash is " & number_of_cyrrilic_names & ". Continue with same files?" with title "File Order Randomizer" buttons {"Randomize", "Unrandomize", "Exit"} cancel button "Exit"
	if the button returned of the result is "Randomize" then
		randomize_order_handler(selected_folder)
	else if the button returned of the result is "Unrandomize" then
		unrandomize_order_handler(selected_folder)
	end if
end remove_cyrillic_handler
