package main

import (
	"fmt"
	"io/fs"
	"math"
	"math/rand"
	"os"
	"path/filepath"
	"sort"
	"strconv"
	"strings"
	"unicode"
)

func draw_progress_bar(index *int, total *int) {
	cur_per, bar_len := ((*index) * 100) / (*total), 60
	fill_len := (cur_per * bar_len)/100
	sp_len := bar_len - fill_len
	bar := strings.Repeat("#", fill_len) + strings.Repeat(" ", sp_len)

	fmt.Printf("Working: [%s] %d%%\r", bar, cur_per)
	if (*index == *total){
		fmt.Print("\n")
	}
}

func unrand_file_name(file_name *string) {
	index := 0
	for ; unicode.IsDigit(rune((*file_name)[index])); {
		index++
	}
	// '_' separator
	if ((*file_name)[index] == '_'){
		*file_name = (*file_name)[index+1:]
		return
	}
	// '. ' separator
	if ((*file_name)[index] == '.' && (*file_name)[index+1] == ' '){
		*file_name = (*file_name)[index+2:]
		return
	}
	// ' - ' separator
	if ((*file_name)[index] == ' ' && (*file_name)[index+1] == '-' && (*file_name)[index+2] == ' '){
		*file_name = (*file_name)[index+3:]
		return
	}
}

func rand_file_name(file_name *string, rand_num_max *int, rand_num_len *int) {
	//Getting random number and adding leading zeros
	random_number := strconv.Itoa(rand.Intn(*rand_num_max))
	random_number = fmt.Sprintf("%0*s",*rand_num_len, random_number)
	// Remove numbers from previous operations if needed
	if unicode.IsDigit(rune((*file_name)[0])) {
		unrand_file_name(file_name)
	}
	*file_name =  random_number + ". " + (*file_name)

}

func ser_file_name(file_name *string, num_str_len *int, ser_num *int){
	// Remove numbers from previous operations if needed
	if unicode.IsDigit(rune((*file_name)[0])) {
		unrand_file_name(file_name)
	}

	ser_num_str := strconv.Itoa(*ser_num)
	// Padding number string
	ser_num_str = fmt.Sprintf("%0*s", *num_str_len, ser_num_str)
	// Inserting serial number string
	*file_name = ser_num_str + ". " + (*file_name);
}

func ignore_entry(entry *fs.DirEntry) bool{
	// Check if path is folder
	if ((*entry).IsDir()){
		return true
	}
	// Check if macOS junk
	macOS_junk_names := []string{".DS_Store", ".localized", "$RECYCLE.BIN", ".fseventsd", ".Spotlight-V100", "System Volume Information"}
	//Checking for usual macOS junk names
	for index := range macOS_junk_names {
		if (strings.Contains((*entry).Name(), macOS_junk_names[index])){
			return true
		}
	}
	//Checking for ._AppleDouble
	if ((*entry).Name()[0] == '.' && (*entry).Name()[1] == '_'){
		return true
	}
	
	return false
}

func filter_func(file_name *string, filters_arr *[]string) bool{
	// Iterating through filter triggers and returning true if found
	for index := range *filters_arr {
		if (strings.Contains(*file_name, (*filters_arr)[index])){
			return true
		}
	}

	return false
}

func form_filter_list(filters_arr *[]string, exec_path string){
		path_to_filterlist := exec_path + "/forand_filterlist.txt"
		_, err := os.Stat(path_to_filterlist)
		if (os.IsNotExist(err)){
			*filters_arr = append(*filters_arr, "а", "А", "е", "Е", "ё", "Ё", "и", "И", "о", "О", "у", "У",
                               					"э", "Э", "ю", "Ю", "я", "Я", "ы", "Ы", "т", "Т", "с", "С")
            fmt.Println("Loaded built-in filter list.")
		} else {
			var file_content []byte
			file_content, err = os.ReadFile(path_to_filterlist)
			if (err != nil){
				fmt.Println("Error: Couldn't open forand_filterlist.txt.")
				os.Exit(0)
			}
			// Split content by commas or new lines
		    triggers := strings.FieldsFunc(string(file_content), func(r rune) bool {
		        return r == ',' || r == '\n'
		    })

		    *filters_arr = append(*filters_arr, triggers...)

		    fmt.Printf("Loaded filter list from: %s\n", path_to_filterlist)
		}
}

func print_help_and_exit() {
	fmt.Println("usage: forand [-furs] [\"/path/to/folder\"] ...\n",
		"-f  filter files in folder.\n",
		"-u  unrandomize files in folder.\n",
		"-r  randomize files in folder.\n",
		"-s  serialize files in folder(s).")
	os.Exit(0)
}

func main() {
	if len(os.Args) <= 2 {
		fmt.Println("Error: Total number of arguments is <=1.")
		print_help_and_exit()
	}

	filter, unrand, random, serial := false, false, false, false
	// Processing options passed to the app
	for index := range os.Args[1] {
		switch os.Args[1][index] {
		case 'f':
			filter = true
		case 'u':
			unrand = true
		case 'r':
			random = true
		case 's':
			serial = true
		case '-':
			// Do nothing		
		default:
			fmt.Printf("Error: Invalid option detected: \"%c\"\n", os.Args[1][index])
			print_help_and_exit()
		}
	}

	// Checking provided paths
	var paths_arr []string
	for _, cur_path := range os.Args[2:] {
		file_info, err := os.Stat(cur_path)
		if (os.IsNotExist(err) || !file_info.IsDir()) {
			fmt.Printf("Error: Path \"%s\" doesn't exist or is not a directory.", cur_path)
			print_help_and_exit()
		}
		// Removing trailing '/' if present
		cur_path = strings.TrimSuffix(cur_path, "/")

		paths_arr = append(paths_arr, cur_path)
	}

	// Forming filter list
	var exec_path string
	var filters_arr []string
	var err error
	if (filter){
		exec_path, err = os.Executable()
		if err != nil {
			fmt.Println("Error: Couldn't get executable launch path.")
			os.Exit(0)
		}
		exec_path = filepath.Dir(exec_path)
		form_filter_list(&filters_arr, exec_path)
	}

	//Looping through provided paths
	for _, cur_path := range paths_arr {
		//Getting list of files in folder
		folder_contents, _ := os.ReadDir(cur_path)
		
		//Getting number of files in folder
		files_in_folder := len(folder_contents)
		
		// Forming path for filtered files
		filter_path := cur_path + "/filtered"
		os.Mkdir(filter_path, 0755)
		if err != nil && !os.IsExist(err) {
			fmt.Println("Error: Couldn't create \"filtered\" folder.")
			os.Exit(0)
		}
		
		//Calculating random number properties
		rand_num_len, rand_num_max := 0, 1;
		if (random){
			rand_num_len = len(strconv.Itoa(files_in_folder))
			rand_num_max = int(math.Pow10(rand_num_len)) - 1
		}

		// Prepairing serialization array
		var serial_arr []string

		// Counters
		file_count, filter_ctr, unrand_ctr, random_ctr := 0,0,0,0

		draw_progress_bar(&file_count, &files_in_folder)

		//Looping through files in folder
		var old_full_path, new_full_path, file_name string

		for _, entry := range folder_contents {
			if (ignore_entry(&entry)){
				file_count++
				draw_progress_bar(&file_count, &files_in_folder)
				continue
			}

			file_name = entry.Name()
			// Filtering
			if (filter && filter_func(&file_name, &filters_arr)){
				old_full_path = cur_path + "/" + file_name
				new_full_path = filter_path + "/" + file_name
				
				err := os.Rename(old_full_path, new_full_path)
				if (err != nil) {fmt.Println(err)}
				
				file_count++
				filter_ctr++
				draw_progress_bar(&file_count, &files_in_folder)
				continue
			}
			// Unrandomizing
			if (unrand && unicode.IsDigit(rune(file_name[0]))){
				old_full_path = cur_path + "/" + file_name
				unrand_file_name(&file_name)
				new_full_path = cur_path + "/" + file_name

				if (old_full_path != new_full_path){
					err := os.Rename(old_full_path, new_full_path)
					if (err != nil) {fmt.Println(err)}
					
					unrand_ctr++
				}
			}
			// Randomizing
			if (random){
				old_full_path = cur_path + "/" + file_name
				rand_file_name(&file_name, &rand_num_max, &rand_num_len)
				new_full_path = cur_path + "/" + file_name

				err := os.Rename(old_full_path, new_full_path)
				if (err != nil) {fmt.Println(err)}
				
				random_ctr++
			}
			// Serializing
			if (serial){
				serial_arr = append(serial_arr, file_name)
			}
			
			// Update progressbar
			file_count++
			draw_progress_bar(&file_count, &files_in_folder)
		}

		if (filter) {
			fmt.Print("Files filtered: ", filter_ctr, " ")
			os.Remove(filter_path)
			if err != nil {
				fmt.Println("Error: Couldn't create \"filtered\" folder.")
			}
		}
		if (unrand) {
			fmt.Print("Files unrandomized: ", unrand_ctr, " ")
		}
		if (random) {
			fmt.Print("Files randomized: ", random_ctr, " ")
		}
		fmt.Println()

		if (!serial || len(serial_arr) == 0) {continue;}
		
		// Serializing
		sort.Strings(serial_arr)

		file_count = 0
		ser_arr_size := len(serial_arr)
		num_str_len := len(strconv.Itoa(ser_arr_size))
		// Draw empty progressbar
		draw_progress_bar(&file_count, &ser_arr_size)

		for index, cur_entry := range serial_arr {
			old_full_path = cur_path + "/" + cur_entry
			ser_file_name(&cur_entry, &num_str_len, &index)
			new_full_path = cur_path + "/" + cur_entry
			
			
			err := os.Rename(old_full_path, new_full_path)
			if (err != nil) {fmt.Println(err)}
			
			file_count++
			// Draw progressbar
			draw_progress_bar(&file_count, &ser_arr_size)
		}

		fmt.Println("Files serialized:", file_count)
	}
}
