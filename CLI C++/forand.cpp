#include <iostream>
#include <filesystem> //for std::filesystem
#include <vector>     //for std::vector
#include <random>     //for mersenne()
#include <fstream>    //for ifstream
#include <unistd.h>   //for getlogin()

void draw_progress_bar(int current_percent) {
	int filler_total_lenght = 60;
	int filler_lenght = (current_percent * filler_total_lenght)/100;
	int space_lenght = filler_total_lenght - filler_lenght;
	std::string filler(filler_lenght, '#'), spacer(space_lenght, ' ');

	std::cout << "Working: [" << filler << spacer << "] " << current_percent << "%\r";
}

void get_filter_list(std::vector<std::string> &filter_triggers, std::filesystem::path &launch_path){
	std::string path_to_filter_file, launch_path_string, path_to_PATH = "/private/etc/paths";

	//checking for forand_filterlist.txt file in app launch folder
	launch_path_string = launch_path.parent_path();
	if (launch_path_string[0] == '/') {
		path_to_filter_file =  launch_path_string + "/forand_filterlist.txt";

		if (!std::filesystem::exists(path_to_filter_file)) {
			path_to_filter_file = "";
		};
	};

	//checking for forand_filterlist.txt in PATH locations
	if (path_to_filter_file.empty() && std::filesystem::exists(path_to_PATH)) {
		std::string line;
		std::ifstream file (path_to_PATH);

		if (file.is_open()){
			while (getline(file, line)){
				if(std::filesystem::exists(line+"/forand_filterlist.txt")) {
					path_to_filter_file = line+"/forand_filterlist.txt";
					break;
				};
			};
		};
		file.close();
	};

	//Loading built-in filters list if forand_filterlist.txt not found
	if (path_to_filter_file.empty()){
		std::string built_in_triggers[] {"а", "А", "е", "Е", "ё", "Ё", "и", "И", 
		"о", "О", "у", "У", "э", "Э", "ю", "Ю", "я", "Я"};

		for (std::string trigger : built_in_triggers){
			filter_triggers.push_back(trigger);
		};
		std::cout << "Loaded built-in filters." << std::endl;
	} else { //Loading filters list from forand_filterlist.txt
		std::string line;
		int i = 0;
		std::ifstream file (path_to_filter_file);

		if (file.is_open()) {
			while (getline(file, line, ',')) {
				filter_triggers.push_back(line);
				++i;
			};
		};
		file.close();
		std::cout << "Loaded filters list from: " << path_to_filter_file << std::endl;
	};
}

std::string get_rand_num(int &random_number_max, int &random_number_len){
	std::random_device rd; 
	std::mt19937 mersenne(rd());
	std::uniform_int_distribution dist{0, random_number_max};
	
	//Adding zero paddig to random number
	std::string random_number = std::to_string(dist(mersenne));
	if (random_number.size() < random_number_len) {
		random_number.insert(0,random_number_len - random_number.size(),'0');
	};

	return random_number;
}

std::string unrand_file_name(std::string file_name){
	int index = 0;
	std::vector<std::string> separators = {"_",". "};
	//Search for separators.
	for (std::string separator : separators){
		index = file_name.find(separator);
		if (index != std::string::npos) {
			index += separator.size();
			break;
		} else {
			index = 0;
		};
	};
	//Trim file name if any of separators found
	if(index >= 0){
		file_name.erase(0,index);
	};

	return file_name;
}

bool macOS_junk(std::string &file_name) {
	std::vector<std::string> macOS_junk_names
	{".DS_Store", ".localized", "$RECYCLE.BIN", ".fseventsd", 
	 ".Spotlight-V100", "System Volume Information"};
	//Checking for usual junk folders
	for (std::string junk_entry : macOS_junk_names) {
		if (file_name.find(junk_entry) != std::string::npos) {
			return true;
		};
	};
	//Checking for ._AppleDouble
	if (file_name[0] == '.' && file_name[1] == '_') {
		return true;
	};

	return false;
}

void print_help(){
	std::cout << "usage: for [-fur] [\"/path/to/folder\"] ...\n"
			  << "-r  randomize files in folder.\n"
			  << "-u  unrandomize files in folder.\n"
			  << "-f  filter files in folder." << std::endl;
}

int main(int argc, char const *argv[]) {

	if (argc <= 2){
		print_help();
		return 1;
	};

	//Checking options passed to the app
	std::string options_string = argv[1];
	for (char option : options_string){
		if (option != '-' && option != 'f' && option != 'u' && option != 'r'){
			std::cout << "Error: Invalid option: " << option << std::endl;
			print_help();
			return 1;
		};
	};
	options_string.erase(0,1);

	//Checking paths passed to the app
	std::vector<std::string> paths_array;
	for (int arg_number = 2; arg_number < argc; ++arg_number){
		if (!std::filesystem::exists(argv[arg_number]) ||
			!std::filesystem::is_directory(argv[arg_number])){
			std::cout << "Error: Path doesn't exist or isn't a directory." << std::endl;
			print_help();
			return 1;
		};

		paths_array.push_back(argv[arg_number]);
	};

	//Loading filter list
	std::filesystem::path launch_path = argv[0];
	std::vector<std::string> filter_triggers;
	get_filter_list(filter_triggers, launch_path);
	std::string trash_path = "/Users/" + (std::string)(getlogin()) + "/.Trash";

	//Looping through paths in path array
	for (std::string path_to_folder : paths_array){

		//Getting list of files in folder and their total number
		std::vector<std::filesystem::path> folder_contents;
		int files_in_folder = 0;
		for (const auto &entry : std::filesystem::directory_iterator(path_to_folder)) {
			folder_contents.push_back(entry.path());
			++files_in_folder;
		};

		//Getting lenght of random number based on number of files
		int random_number_len = std::to_string(files_in_folder).size();
		//Getting maximum value of random number
		int random_number_max = 1;
		for (int i = 0; i < random_number_len; ++i) {
			random_number_max *= 10;
		};
		random_number_max -= 1;

		draw_progress_bar(0);
		int file_counter = 0, filt_counter = 0, rand_counter = 0, unrand_counter = 0;

		//Looping through paths in folder_contents
		int current_position = 0;
		std::string file_name, file_path, old_path, random_number;

		for (std::filesystem::path &path_to_file : folder_contents) {

			//Checking if entry is macOS trash or folder
			file_name = path_to_file.filename();
			if(macOS_junk(file_name)){
				++current_position;
				continue;
			};
			if(std::filesystem::is_directory(path_to_file)){
				++current_position;
				continue;
			};
			file_path = path_to_file.parent_path();

			//Looping through provided arguments
			for (char argument : options_string){
				switch(argument){
				case 'f':{
					bool trigger_found = false;
					for (int i = 0; i < filter_triggers.size(); ++i){
						if (file_name.find(filter_triggers[i]) != std::string::npos){
							trigger_found = true;
							break;
						};
					};

					if (!trigger_found){break;};

					//Moving file to trash
					old_path = file_path + "/" + file_name;
					const char *char_old_path = old_path.c_str();
					path_to_file = trash_path + "/" + file_name;
					const char *char_new_path = path_to_file.c_str();

					int result = rename(char_old_path, char_new_path);
					if (result != 0) {
						perror("Couldn't rename file (ʘдʘ╬)");
					};

					++filt_counter;
					goto exit_options_cycle;
				}
				case 'u':{
					//Check if first character in file name is digit
					if(!isdigit(file_name[0])){break;};

					//Renaming file
					old_path = file_path + "/" + file_name;
					const char *char_old_path = old_path.c_str();
					file_name = unrand_file_name(file_name);
					path_to_file = file_path + "/" + file_name;
					const char *char_new_path = path_to_file.c_str();

					int result = rename(char_old_path, char_new_path);
					if (result != 0) {
						perror("Couldn't rename file (ʘдʘ╬)");
					};
					++unrand_counter;
					break;
				}
				case 'r':{
					//Renaming file
					old_path = file_path + "/" + file_name;
					const char *char_old_path = old_path.c_str();

					//Unrandomizing file before continuing
					if(isdigit(file_name[0])) {
						file_name = unrand_file_name(file_name);
					};

					file_name = get_rand_num(random_number_max, random_number_len) + "_" + file_name;
					path_to_file = file_path + "/"  + file_name;
					const char *char_new_path = path_to_file.c_str();

					int result = rename(char_old_path, char_new_path);
					if (result != 0) {
						perror("Couldn't rename file (ʘдʘ╬)");
					};
					++rand_counter;
					break;
				}
				default:{
					break;
				}
				};//switch(argument)
			};// for >> options_string

			//Label to get out of options_string cycle
			//after moving file to trash
			exit_options_cycle:

			//Calculating percent of work done
			++file_counter;
			int current_percent = (file_counter * 100) / (files_in_folder);
			draw_progress_bar(current_percent);
		} // for >> folder_contents

		// Draw 100% progress bar when finished with folder
		draw_progress_bar(100);
		std::cout << std::endl;

		//Ouptut additional data
		if (filt_counter > 0){
			std::cout << "Files filtered: " << filt_counter << " ";
		};
		if (unrand_counter > 0){
			std::cout << "Files unrandomized: " << unrand_counter << " ";
		};
		if (rand_counter > 0){
			std::cout << "Files randomized: " << rand_counter << " ";
		};
		std::cout<< std::endl;
	} //for >> paths array
			
	return 0;
}
