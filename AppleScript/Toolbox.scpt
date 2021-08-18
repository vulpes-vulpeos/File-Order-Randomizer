on run {}
	main_menu()
end run

on main_menu()
	set main_menu_array to {"Remove symbols", "Randomize order", "Remove RU songs", "Download Youtube video", "Download video"}
	set main_menu_user_choice to choose from list main_menu_array with prompt "What you want me to do?" with title "Toolbox" default items {"Remove symbols"} cancel button name "Exit"
	
	if main_menu_user_choice is false then
		stop
	else if (main_menu_user_choice = {"Remove symbols"}) then
		remove_symbols_handler()
	else if (main_menu_user_choice = {"Randomize order"}) then
		randomize_order_handler()
	else if (main_menu_user_choice = {"Remove RU songs"}) then
		remove_ru_songs_handler()
	else if (main_menu_user_choice = {"Download Youtube video"}) then
		download_yt_video_handler()
	else if (main_menu_user_choice = {"Download video"}) then
		download_video_handler()
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

on remove_ru_songs_handler()
	set remove_ru_songs_array to {"а", "е", "ё", "и", "о", "у", "э", "ю", "я", "MORGENSHTERN", "V $ X V Prince", "GAYAZOV$ BROTHER$", "ONUKA", "Slava Marlow", "Niletto", "Zivert", "Johnyboy", "Nebezao", "ALEXEMELYA"}
	set user_home_folder to path to home folder from user domain
	set selected_files to choose file default location user_home_folder with prompt "Choose files you want to filter" with multiple selections allowed
	
	set list_of_files to {}
	tell application "Finder"
		set number_of_ru_files to 0
		repeat with aFile in selected_files
			repeat with i from 1 to count of items of remove_ru_songs_array
				if name of aFile contains item i of remove_ru_songs_array then
					set number_of_ru_files to number_of_ru_files + 1
					move aFile to trash
					exit repeat
				end if
			end repeat
		end repeat
	end tell
	
	display dialog "Finished! Total number of files moved to trash is " & number_of_ru_files & "." with title "Toolbox" buttons {"To main menu", "Exit"} cancel button "Exit"
	if the button returned of the result is "To main menu" then
		main_menu()
	end if
end remove_ru_songs_handler

on download_yt_video_handler()
	set clipboard_content to the clipboard as text
	set link_to_yt_video to display dialog "Paste video URL:" default answer clipboard_content with title "Toolbox" buttons {"Exit", "Continue"} default button "Continue" cancel button "Exit"
	set user_home_folder to POSIX path of (path to home folder from user domain)
	set download_yt_video_array to {"1080p", "720p", "As .mp3"}
	set download_yt_video_user_choice to choose from list download_yt_video_array with prompt "Select the quality or type of download:" with title "Toolbox" default items {"1080p"} cancel button name "Exit"
	
	if download_yt_video_user_choice is false then
		stop
	else if (download_yt_video_user_choice = {"1080p"}) then
		set video_quality to "-f 137+140"
		set video_destination_folder to user_home_folder & "Movies/"
	else if (download_yt_video_user_choice = {"720p"}) then
		set video_quality to "-f 136+140"
		set video_destination_folder to user_home_folder & "Movies/"
	else if (download_yt_video_user_choice = {"As .mp3"}) then
		set video_quality to "--extract-audio --audio-format mp3"
		set video_destination_folder to user_home_folder & "Downloads/"
	end if
	
	tell application "Terminal"
		activate
		tell application "System Events"
			keystroke "t" using {command down}
			delay 0.2
		end tell
		do script "youtube-dl " & video_quality & " -o \"" & video_destination_folder & "%(title)s.%(ext)s" & "\" '" & (text returned of link_to_yt_video) & "'" in window 1
	end tell
	
	--To make it work in background use this (terminal window still opens):
	--tell application "Terminal"
	--do shell script "PATH=$PATH:/usr/local/bin:/usr/bin:/bin:/usr/sbin" & quoted form of (POSIX path of (path to me)) & "; " & "youtube-dl " & video_quality & " -o \"" & video_destination_folder & "%(title)s.%(ext)s" & "\" '" & (text returned of link_to_yt_video) & "'"
	--end tell
end download_yt_video_handler

on download_video_handler()
	set clipboard_content to the clipboard as text
	set link_to_video to display dialog "Paste video URL:" default answer clipboard_content with title "Toolbox" buttons {"Exit", "Continue"} default button "Continue" cancel button "Exit"
	set user_home_folder to POSIX path of (path to home folder from user domain)
	set video_destination_folder to user_home_folder & "Movies/"
	
	tell application "Terminal"
		activate
		tell application "System Events"
			keystroke "t" using {command down}
			delay 0.2
		end tell
		do script "youtube-dl " & " -o \"" & video_destination_folder & "%(title)s.%(ext)s" & "\" '" & (text returned of link_to_video) & "'" in window 1
	end tell
end download_video_handler

on finish_dialog()
	display dialog "Finished" with title "Toolbox" buttons {"To main menu", "Exit"} cancel button "Exit"
	if the button returned of the result is "To main menu" then
		main_menu()
	end if
end finish_dialog
