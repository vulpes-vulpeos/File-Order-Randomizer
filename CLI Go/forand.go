package main

import (
	"fmt"
	"regexp"    //for matching provided arguments
	"os"        //for dir eterator, deleting and renaming files etc.
	"os/user"   //for username
	"strings"   //for string manipulations
	"unicode"   //for detecting cyrillic characters
	"math/rand" //for random number generation
	"strconv"   //for int to string convertion
)

func draw_progress_bar(current_percent int){
	filler_total_lenght := 60
	var filler_lenght int = (current_percent*filler_total_lenght)/100
	space_lenght := filler_total_lenght-filler_lenght
	bar := strings.Repeat("#", filler_lenght) + strings.Repeat(" ", space_lenght)

	fmt.Print("Working: [", bar, "] ", current_percent,"%\r")
}

func unrand_file_name(file_name string)string{
	//Search for separators. ". ", "_" are supported
	index := strings.Index(file_name, ". ")
	separator_size := 2
	if index == -1 {
		index = strings.IndexByte(file_name, '_')
		separator_size = 1
	}

	//Trim file name if any of separators found
	if index >= 0 {
		file_name = file_name[index+separator_size:]
	}

	return file_name
}

func print_help(){
	fmt.Print("usage: forand [-fur] [\"/path/to/folder\"] ...\n",
						"-r  randomize files in folder.\n", 
						"-u  unrandomize files in folder.\n",
						"-f  filter files in folder.")
}

func main() {
	if len(os.Args) <= 1{
		fmt.Println("Error: Total number of arguments is <=1.")
		print_help()
		return
	}

	//Getting args passed to the app
	var input_arguments[] string
	for _, argument := range os.Args[1:] {
		input_arguments = append(input_arguments, argument)
	}

	//Checking if arguments contain only -fur
	input_arguments[0] = strings.ToLower(string(input_arguments[0]))
	//Matches any character that is not in the set
	matched, _ := regexp.Match(`[^-fur]+`, []byte(input_arguments[0]))
	if matched {
		fmt.Println("Error: Invalid argument detected.")
		print_help()
		return
	}
	//number_of_arg := len(input_arguments[0])-1 //Number of arguments for progress bar

	//Checking if path exists and is a folder
	for _, path_to_file := range input_arguments[1:] {
		file_info, err := os.Stat(path_to_file)
		if os.IsNotExist(err) || !file_info.IsDir(){
			fmt.Println("Error: Path doesn't exist or is not a directory.", path_to_file)
			print_help()
			return
		}
	}

	//Looping through paths in arguments array
	for _, path_to_folder := range input_arguments[1:]{

		//Getting list of files in folder
		folder_contents, _ := os.ReadDir(path_to_folder)
		//Getting number of files in folder
		files_in_folder := len(folder_contents)
		//Getting random number lenght based on number of files
		random_number_len := len(strconv.Itoa(files_in_folder))

		draw_progress_bar(0)
		file_counter := 0

		//Looping through files in folder
		for _, entry := range folder_contents {

			//Checking if entry is macOS trash or folder
			if entry.Name()[0] == '.'{
				file_counter++
				continue
			}
			if entry.IsDir(){
				file_counter++
				continue
			}
			entry_full_path := path_to_folder + "/" + entry.Name()

			//Looping through provided arguments
			argument_loop: //label for easier loop breaking
			for _, argument := range input_arguments[0][1:]{
				switch argument {
				case 'f':
					//Checking for cyrillic letters in file name
					for _, char := range entry.Name(){
						if unicode.Is(unicode.Cyrillic, char){
							//Move file to trash
							username, _ := user.Current() //Getting username
							err := os.Rename(entry_full_path, 
										 "/Users/" + username.Username + "/.Trash/" + entry.Name())

							//Complitely delete file
							//err := os.Remove(entry_full_path)

  						if err != nil {
    						fmt.Println(err)
  						}
							break argument_loop //Break argument for loop
						}
					}
					
				case 'u':
					//Check if first character in file name is digit
					if !unicode.IsDigit(rune(entry.Name()[0])){
						break //Break switch iteration if not
					}

					//Get unrandomized file name
					file_name := unrand_file_name(entry.Name())

					//Rename file if any of separators found
					if len(file_name) < len(entry.Name()) {
						err := os.Rename(entry_full_path, 
									path_to_folder + "/" + file_name)
						
						if err != nil {
    					fmt.Println(err)
  					}
					}

				case 'r':
					//Getting maximum value of random number
					var random_number_max int = 1
					for i := 0; i < random_number_len; i++{
						random_number_max*=10
					}
					random_number_max-=1

					//Getting random number and adding leading zeros
					random_number := strconv.Itoa(rand.Intn(random_number_max))
					for len(random_number) < random_number_len{
						random_number = "0" + random_number
					}

					//Check if first character in file name is digit
					file_name := entry.Name()
					if unicode.IsDigit(rune(file_name[0])){
						file_name = unrand_file_name(entry.Name())
						entry_full_path = path_to_folder + "/" + file_name
					}

					//Renaming file
					err := os.Rename(entry_full_path, 
								 path_to_folder + "/" + string(random_number) + ". " + file_name)
						
					if err != nil {
    				fmt.Println(err)
  				}

				default:
					fmt.Println("Unexpected argument O_o")
					panic(1)
				}
			}//for >> input_arguments
			//Calculating percent of work done
			file_counter++
			var current_percent int = (file_counter*100)/(files_in_folder)
			draw_progress_bar(current_percent)
		}//for >> folder_contents
	}//for >> paths in arguments

	fmt.Print("\n")
}
