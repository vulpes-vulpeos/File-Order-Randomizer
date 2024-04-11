#include <algorithm>
#include <cctype>
#include <iostream>
#include <filesystem> //for std::filesystem
#include <vector>     //for std::vector
#include <random>     //for mersenne()
#include <fstream>    //for ifstream
#include <unistd.h>   //for readlink()

#ifdef __APPLE__
#include <libproc.h> // for proc_pidpath()
#endif

void draw_progress_bar(const int &index, const int &total) {
	int cur_per = (index * 100) / (total);
	int bar_len = 60;
	int fill_len = (cur_per * bar_len)/100;
	int sp_len = bar_len - fill_len;
	std::string filler(fill_len, '#'), spacer(sp_len, ' ');

	std::cout << "Working: [" << filler << spacer << "] " << cur_per << "%\r";
	if (index == total) {std::cout << std::endl;};
}

bool ignore_entry(std::filesystem::path path_to_file){
	// Check if path is folder
	if(std::filesystem::is_directory(path_to_file)){return true;};
	// Check if macOS junk
	std::vector<std::string> macOS_junk_names = {".DS_Store", ".localized", "$RECYCLE.BIN", ".fseventsd", 
												 ".Spotlight-V100", "System Volume Information"};
	//Checking for usual macOS junk names
	std::string file_name = path_to_file.filename();
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

void rename_file(const std::string &old_full_path, const std::string &new_full_path){
	const char *char_old_path = old_full_path.c_str();
	const char *char_new_path = new_full_path.c_str();

	int result = rename(char_old_path, char_new_path);
	if (result != 0) {
		std::cout << "(ʘдʘ╬) Couldn't rename file:\n" << old_full_path
				  << "\nto:\n" << new_full_path << std::endl;
	};
}

void form_filter_list(std::vector<std::string> &filter_triggers, const std::filesystem::path &launch_path){
	std::string path_to_filterlist = (std::string)launch_path + "/forand_filterlist.txt";

	if (std::filesystem::exists(path_to_filterlist)) {
		//Loading filters list from forand_filterlist.txt
		std::string line;
		std::ifstream file (path_to_filterlist);

		if (file.is_open()) {
			while (getline(file, line, ',')) {
				filter_triggers.push_back(line);
			};
		};
		file.close();
		std::cout << "Loaded filter list from: " << path_to_filterlist << std::endl;
	} else {
		//Loading built-in filters list if forand_filterlist.txt not found
		std::string builtin[] {"а", "А", "е", "Е", "ё", "Ё", "и", "И", "о", "О", "у", "У",
                               "э", "Э", "ю", "Ю", "я", "Я", "ы", "Ы", "т", "Т", "с", "С"};

		for (std::string trigger : builtin){
			filter_triggers.push_back(trigger);
		};
		std::cout << "Loaded built-in filter list." << std::endl;
	};
}

void unrand_file_name(std::string &file_name){
	int index = 0;

	do {
		++index;
	} while (isdigit(file_name[index]));
	// '_' separator
	if (file_name[index] == '_'){
		file_name = file_name.erase(0, index+1);
		return;
	};
	// '. ' separator
	if (file_name[index] == '.' && file_name[index+1] == ' '){
		file_name = file_name.erase(0, index+2);
		return;
	};
	// ' - ' separator
	if (file_name[index] == ' ' && file_name[index+1] == '-' && file_name[index+1] == ' '){
		file_name = file_name.erase(0, index+3);
		return;
	};
}

void rand_file_name(std::string &file_name, const int &random_num_max, const int &random_num_len){
	// Remove numbers from previous operations if needed
	if(isdigit(file_name[0])) {unrand_file_name(file_name);};
	// Getting random number
	std::random_device rd; 
	std::mt19937 mersenne(rd());
	std::uniform_int_distribution dist{0, random_num_max};
	std::string rand_num_str = std::to_string(dist(mersenne));
	// Padding random number string
	if ((int)rand_num_str.size() < random_num_len) {
		rand_num_str.insert(0, random_num_len - rand_num_str.size(),'0');
	};
	// Inserting random number string
	file_name = rand_num_str + ". " + file_name;
}

void ser_file_name (std::string &file_name, const int &num_str_len, const int &index){
	// Remove numbers from previous operations if needed
	if(isdigit(file_name[0])) {unrand_file_name(file_name);};
	std::string ser_num_str = std::to_string(index);
	// Padding number string
	if ((int)ser_num_str.size() < num_str_len) {
		ser_num_str.insert(0, num_str_len - ser_num_str.size(),'0');
	};
	// Inserting serial number string
	file_name = ser_num_str + ". " + file_name;
}

bool filter_func (std::string &file_name, std::vector<std::string> &filters_arr){
	// Iterating through filter triggers and returning true if found
	for (unsigned long i = 0; i < filters_arr.size(); ++i){
		if (file_name.find(filters_arr[i]) != std::string::npos){
			return true;
		};
	};

	return false;
}

void print_help_and_exit(){
	std::cout << "usage: for [-furs] [\"/path/to/folder\"] ...\n"
			  << "-f  filter files in folder.\n"
			  << "-u  unrandomize files in folder.\n"
			  << "-r  randomize files in folder.\n"
			  << "-s  serialize files in folder." << std::endl;
			  exit(0);
}

int main(int argc, char const *argv[]) {
	// Exit if not enough arguments
	if (argc <= 2){print_help_and_exit();};

	bool filter = false,
		 unrand = false,
		 random = false,
		 serial = false;
	// Processing options passed to the app
	std::string options_string = argv[1];
	for (unsigned long i = 1; i < options_string.size(); ++i){
		switch (options_string[i]) {
			case 'f':
				filter = true;
				break;
			case 'u':
				unrand = true;
				break;
			case 'r':
				random = true;
				break;
			case 's':
				serial = true;
				break;
			default:
				std::cout << "Error: Invalid option detected." << std::endl;
				print_help_and_exit();
				break;
		};
	};

	// Checking paths
	std::vector<std::string> paths_array;
	for (int arg_number = 2; arg_number < argc; ++arg_number){
		std::string cur_path = argv[arg_number];
		if (!std::filesystem::exists(cur_path) ||
			!std::filesystem::is_directory(cur_path)){
			std::cout << "Error: " << cur_path << " doesn't exist or isn't a directory." << std::endl << std::endl;
			print_help_and_exit();
		};
		// Removing trailing '/' if present
		if (cur_path.back() == '/'){cur_path.pop_back();};

		paths_array.push_back(cur_path);
	};

	// Forming filter list
	std::filesystem::path launch_path;
	std::vector<std::string> filters_arr;
	if (filter) {
		char path[4096];
		// Getting app launch path
		#ifdef __linux__
		int len = readlink("/proc/self/exe", path, 4095);
		if(len != -1){
			launch_path = path;
			launch_path = launch_path.parent_path();
		} else {
			std::cout << "Error: Failed to get app launch path" << std::endl << std::endl;
			return 1;
		};
		#elif __APPLE__
		pid_t pid = getpid();

	    if (proc_pidpath(pid, path, 4095)) {
	        launch_path = path;
			launch_path = launch_path.parent_path();
	    } else {
	        perror("proc_pidpath: ");
	        return 1;
	    }
		#endif

		form_filter_list(filters_arr, launch_path);
	};

	//Looping through provided paths
	for (std::string path_to_folder : paths_array){
		//Getting list of files in folder and their total number
		std::vector<std::filesystem::path> folder_contents;
		int files_in_folder = 0;
		for (const auto &entry : std::filesystem::directory_iterator(path_to_folder)) {
			folder_contents.push_back(entry.path());
			++files_in_folder;
		};

		// Forming path for filtered files
		std::filesystem::path filter_path;
		if (filter){
			filter_path = path_to_folder + "/filtered";
			std::filesystem::create_directory(filter_path);
		};

		//Calculating random number properties
		int random_num_len = 0;
		int random_num_max = 1;
		if (random){
			random_num_len = std::to_string(files_in_folder).size();
			for (int i = 0; i < random_num_len; ++i) {
				random_num_max *= 10;
			};
			random_num_max -= 1;
		};

		// Prepairing serialization array
		std::vector<std::string> serial_arr;

		// Counters
		int file_count = 0,
		    filter_ctr = 0,
		    random_ctr = 0,
		    unrand_ctr = 0;

	    draw_progress_bar(file_count, files_in_folder);

		//Looping through paths in folder_contents
		std::string old_full_path, new_full_path, file_name, random_number;

		for (std::filesystem::path &path_to_file : folder_contents) {
			if (ignore_entry(path_to_file)){
				++file_count;
				// Update progressbar
				draw_progress_bar(file_count, files_in_folder);
				continue;
			};
			file_name = path_to_file.filename();
			// Filtering
			if (filter && filter_func(file_name, filters_arr)){
				old_full_path = path_to_file;
				new_full_path = (std::string)filter_path + "/" + file_name;
				rename_file(old_full_path, new_full_path);

				++filter_ctr;
				++file_count;
				// Update progressbar
				draw_progress_bar(file_count, files_in_folder);
				continue;
			};
			// Unrandomizing
			if (unrand && isdigit(file_name[0])){
				old_full_path = path_to_file;
				unrand_file_name(file_name);
				new_full_path = path_to_folder + "/" + file_name;
				if (old_full_path != new_full_path){
					rename_file(old_full_path, new_full_path);
					++unrand_ctr;
				};
			};
			// Randomizing
			if (random){
				old_full_path = path_to_folder + "/" + file_name;
				rand_file_name(file_name, random_num_max, random_num_len);
				new_full_path = path_to_folder + "/" + file_name;
				rename_file(old_full_path, new_full_path);
				++random_ctr;
			};
			// Serializing
			if (serial){serial_arr.push_back(file_name);};
			
			++file_count;
			// Update progressbar
			draw_progress_bar(file_count, files_in_folder);
		};

		//Ouptut additional data
		if (filter) {
			std::cout << "Files filtered: " << filter_ctr << " ";
		};
		if (unrand) {
			std::cout << "Files unrandomized: " << unrand_ctr << " ";
		};
		if (random) {
			std::cout << "Files randomized: " << random_ctr << " ";
		};
		std::cout << std::endl;

		if (filter){
			// Remove filtered folder if empty
			std::error_code ec;
			std::filesystem::remove(filter_path, ec);
		}

		if (!serial) {continue;};
		// Serializing
		std::sort(serial_arr.begin(), serial_arr.end());
		// Draw empty ptogressbar
		file_count = 0;
		unsigned long ser_arr_size = serial_arr.size();
		int num_str_len = std::to_string(ser_arr_size).size();

		draw_progress_bar(file_count, ser_arr_size);

		for (unsigned long i = 0; i < ser_arr_size; ++i){
			old_full_path = path_to_folder + "/" + serial_arr[i];
			ser_file_name(serial_arr[i], num_str_len, i+1);
			new_full_path = path_to_folder + "/" + serial_arr[i];
			rename_file(old_full_path, new_full_path);
			++file_count;
			// Draw progressbar
			draw_progress_bar(file_count, ser_arr_size);
		};

		std::cout << "Files serialized: " << file_count << std::endl;
	};
	
	return 0;
}
