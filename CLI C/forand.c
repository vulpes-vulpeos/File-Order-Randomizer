#include <ctype.h>
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>   // for opendir(), readdir(), dirent struct
#include <time.h>     // for rand()
#include <sys/stat.h> // for mkdir()

#ifdef __APPLE__
#include <libproc.h> // for proc_pidpath()
#endif

void unrand_filename(char **file_name);

// for qsort()
int comp_str(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void ser_filename(char **file_name, unsigned int * const num_str_len, unsigned int * const num){
    // Remove numbers from previous operations if needed
    if (isdigit(*file_name[0])){unrand_filename(file_name);};
    
    char *ser_num_str = calloc((*num_str_len)+3, sizeof(char));
    // Padding number string
    snprintf(ser_num_str, (*num_str_len)+3, "%0*u. ", *num_str_len, *num+1);
    // Moving memory forward
    memmove(*file_name + (*num_str_len)+2, *file_name, strlen(*file_name)+1);
    // Inserting random number string
    memcpy(*file_name, ser_num_str, (*num_str_len)+2);

    free(ser_num_str);
}

void rand_filename(char **file_name, int * const rand_num_max, int * const rand_num_len){
    // Remove numbers from previous operations if needed
    if (isdigit(*file_name[0])){unrand_filename(file_name);};
    // Getting random number
    unsigned long rand_num = random() % (*rand_num_max + 1);
    int rnum_str_len = *rand_num_len + 3;
    char *rand_num_str = calloc(rnum_str_len, sizeof(char));
    // Padding random number string
    snprintf(rand_num_str, rnum_str_len, "%0*lu. ", *rand_num_len, rand_num);
    // Moving memory forward
    memmove(*file_name + rnum_str_len-1, *file_name, strlen(*file_name)+1);
    // Inserting random number string
    memcpy(*file_name, rand_num_str, rnum_str_len-1);

    free(rand_num_str);
}

void unrand_filename(char **file_name){
    int index = 0;
    char *sep_ptr = NULL;

    do {
        ++index;
    } while (isdigit((*file_name)[index]));

    // '_' separator
    if ((*file_name)[index] == '_'){
        sep_ptr = *file_name + index + 1;
    };
    // '. ' separator
    if ((*file_name)[index] == '.' && (*file_name)[index+1] == ' '){
        sep_ptr = *file_name + index + 2;
    };
    // ' - ' separator
    if ((*file_name)[index] == ' ' && (*file_name)[index+1] == '-' && (*file_name)[index+2] == ' '){
        sep_ptr = *file_name + index + 3;
    };
    // Forming filename without numbers if needed
    if (sep_ptr){
        memmove(*file_name, *file_name + (sep_ptr - (*file_name)), strlen(*file_name + (sep_ptr - (*file_name)))+1);
    };
}

void form_full_path (char **formed_path, char * const folder, char *const file_name){
    *formed_path = calloc(((strlen(folder) + strlen(file_name)) + 2),sizeof(char));
    sprintf(*formed_path, "%s/%s", folder, file_name);
}

int filter_func(char **file_name, char ***filter_list_arr, int *tokens_num){
    // Iterating through filter triggers and returning true if found
    for (int i = 0; i < *tokens_num; ++i){
        if (strstr(*file_name, (*filter_list_arr)[i]) != NULL){
            return 1;
        };
    };

    return 0;
}

void draw_progressbar(const unsigned int *index, const unsigned int *total){
    int cur_per = ((*index) * 100) / (*total);
    int bar_len = 60;
    int fill_len = (cur_per * bar_len)/100;

    char *filler = malloc(fill_len+1);
    memset(filler, '#', fill_len);
    filler[fill_len] = '\0';

    printf("Working: [%-*s] %d%%\r", bar_len, filler, cur_per);
    free(filler);
    
    if (*index == *total){putc('\n', stdout);};
}

int ignore_file(struct dirent *entry){
    // Ignore itself and parent folder
    if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
        return 1;
    };
    // Check if path is folder
    if(entry->d_type != DT_REG){
        return 1;
    };
    // Check for macOS junk
    const char *macOS_junk_names[] = {".DS_Store", ".localized", "$RECYCLE.BIN", ".fseventsd", ".Spotlight-V100", "System Volume Information"};
    const int array_size = sizeof(macOS_junk_names)/sizeof(const char*);
    //Checking for usual junk folders
    for (int i = 0; i < array_size; ++i){
        // strcmp returns 0 if same
        if (!strcmp(entry->d_name, macOS_junk_names[i])){
            return 1;
        };
    };
    //Checking for ._AppleDouble
    if (entry->d_name[0] == '.' && entry->d_name[1] == '_') {
        return 1;
    };

    return 0;
}

int get_num_of_files (char *folder_path){
    int num_of_files = 0;
    struct dirent *entry;
    DIR *dir_ptr = opendir(folder_path);
    if(dir_ptr == NULL) {perror("ERROR: "); return 1;};
    
    while ((entry = readdir(dir_ptr)) != NULL){
        if (ignore_file(entry)) {continue;};
        ++num_of_files;
    };
    closedir(dir_ptr);

    return num_of_files;
}

void form_filterlist(char ***filter_list_arr, char *launch_path, int *tokens_num){
    //Forming filter file path
    char *filter_file_path = calloc(strlen(launch_path) + 23, sizeof(char));
    sprintf(filter_file_path,"%s/forand_filterlist.txt", launch_path);
    //printf("Filter file path: %s\n", filter_file_path);

    if(access(filter_file_path, F_OK) != 0) {
        // Loading default list if forand_filterlist.txt not found
        char *default_list[] = {"а", "А", "е", "Е", "ё", "Ё", "и", "И", "о", "О", "у", "У",
                                "э", "Э", "ю", "Ю", "я", "Я", "ы", "Ы", "т", "Т", "с", "С"};
        *filter_list_arr = (char **)malloc(24 * sizeof(char *));

        for (int i = 0; i < 24; ++i) {
            (*filter_list_arr)[i] = (char *)malloc((strlen(default_list[i]) + 1) * sizeof(char));
            strcpy((*filter_list_arr)[i], default_list[i]);
        };

        *tokens_num = 24;
    } else {
        // Reading forand_filterlist.txt
        *tokens_num = 0;
        FILE *file_ptr;
        file_ptr = fopen(filter_file_path, "r");
        if(file_ptr == NULL) {perror("ERROR: ");exit(1);};

        char buffer[1024];
        // Getting number of tokens
        while(fgets(buffer, sizeof(buffer), file_ptr)) {
            char * pch = strtok (buffer,",\n");
            while (pch != NULL) {
                if (strlen(pch) > 0 && *pch != '\n'){
                    ++(*tokens_num);
                };
                pch = strtok (NULL, ",\n");
            };
        };

        fseek(file_ptr, 0, SEEK_SET);
        // Filling filter list array with tokens
        *filter_list_arr = (char **)malloc((*tokens_num) * sizeof(char *));
        int index = 0;
        while(fgets(buffer, sizeof(buffer), file_ptr)) {
            char * pch = strtok (buffer,",\n");
            while (pch != NULL) {
                if (strlen(pch) > 0 && *pch != '\n'){
                    (*filter_list_arr)[index] = (char *)malloc((strlen(pch) + 1) * sizeof(char));
                    strcpy((*filter_list_arr)[index], pch);
                    ++index;
                };
                pch = strtok (NULL, ",\n");
            };
        };

        fclose(file_ptr);
    };

    free(filter_file_path);
}

void print_usage_and_exit(){
    printf("Usage: forand [-furs] path/to/folder ...\n");
    printf("-f  filter files in folder.\n");
    printf("-u  unrandomize files in folder.\n");
    printf("-r  randomize files in folder.\n");
    printf("-s  serialize files in folder.\n");
    exit(0);
}
  
int main(int argc, char *argv[]){
    if (argc <= 2){print_usage_and_exit();};

    int opt,
        filter = 0,
        unrand = 0,
        random = 0,
        serial = 0;

    // Parse options
    while((opt = getopt(argc, argv, "furs")) != -1) {
        switch(opt) {
            case 'f':
                filter = 1;
                break;
            case 'u':
                unrand = 1;
                break;
            case 'r':
                random = 1;
                break;
            case 's':
                serial = 1;
                break;
            default:
                print_usage_and_exit();
                break;
        };
    };
    
    // Getting and validating paths
    if (optind >= argc) {
        printf("ERROR: No path provided.\n");
        print_usage_and_exit();
    };

    int paths_num = argc - optind,
        index     = 0;
    char **folders_to_parse_arr = (char **)malloc(paths_num * sizeof(char *));

    do {
        if(access(argv[optind], F_OK) != 0) {
            printf("ERROR: Path %s doesn't exist.\n", argv[optind]);
            return 1;
        };

        folders_to_parse_arr[index] = argv[optind];
        // removing trailing '/' if present
        if (folders_to_parse_arr[index][strlen(argv[optind])-1] == '/'){
            folders_to_parse_arr[index][strlen(argv[optind])-1] = '\0';
        };

        ++optind;
        ++index;
    } while (optind < argc);

    // Forming filtering list
    char *username;
    char **filter_list_arr;
    int tokens_num = 0;
    
    if (filter){
        // Getting forand_filterlist.txt (should be placed near executable)
        char path[4096];
        #ifdef __linux__
        ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);

        if (len != -1) {
            // Removing app name from path
            long app_name_len = strlen(path) - (strrchr(path, '/') - path);
            path[len-app_name_len] = '\0';

            form_filterlist(&filter_list_arr, path, &tokens_num);
        } else {
            printf("ERROR: Failed to get app launce path");
            return 1;
        };

        #elif __APPLE__
        pid_t pid = getpid();

        if (proc_pidpath(pid, path, sizeof(path)) > 0) {
            form_filterlist(&filter_list_arr, path, &tokens_num);
        } else {
            perror("proc_pidpath: ");
            return 1;
        }
        #endif
    };

    // Looping through provided paths
    for (int i = 0; i < paths_num; ++i){
        // Getting number of files in folder
        unsigned int num_of_files = get_num_of_files(folders_to_parse_arr[i]);
        //printf("Number of files in %s: %d\n", folders_to_parse_arr[i], num_of_files);
        if (num_of_files == 0) { continue;};

        // Forming path for filtered files
        char *filter_path;
        if (filter){
            filter_path = calloc(strlen(folders_to_parse_arr[i]) + 10, sizeof(char));
            sprintf(filter_path, "%s/filtered", folders_to_parse_arr[i]);
            mkdir(filter_path, 0755);
        };

        // Calculating random number properties
        int rand_num_len = 0,
            rand_num_max = 1;
        if (random){
            srandom(time(NULL));
            rand_num_len = snprintf(NULL, 0, "%d", num_of_files);
            for (int i = 0; i < rand_num_len; ++i){
                rand_num_max *= 10;
            };
            --rand_num_max;
        };

        // Prepairing array for serialization
        char **fold_cont_arr;
        if (serial){
            fold_cont_arr = (char **)malloc(num_of_files * sizeof(char *));
        };
        // Counters
        unsigned int file_count   = 0, 
             filter_count = 0, 
             random_count = 0, 
             unrand_count = 0,
             index = 0,
             ser_arr_size = 0;

        // Draw empty progressbar
        draw_progressbar(&file_count, &num_of_files);

        // Looping through folder contents
        char *fold_path = folders_to_parse_arr[i],
             *random_num;

        struct dirent *entry;
        DIR *dir_ptr = opendir(fold_path);
        if(dir_ptr == NULL) {perror("ERROR: "); return 1;};
        
        while ((entry = readdir(dir_ptr)) != NULL){
            if (ignore_file(entry)) {continue;};
            
            char *old_full_path;
            char *new_full_path;
            char *file_name = entry->d_name;
            //printf("File name: %s\n", file_name);

            // Filtering
            if (filter && filter_func(&file_name, &filter_list_arr, &tokens_num)){
                // Forming old full path
                form_full_path(&old_full_path, fold_path, file_name);
                //printf("\033[1;31mFilter old path\033[0m: %s\n", old_full_path);
                // Forming new full path
                form_full_path(&new_full_path, filter_path, file_name);
                //printf("\033[1;31mFilter new path\033[0m: %s\n", new_full_path);
                // Move file to filtered older
                if(rename(old_full_path, new_full_path)) {perror("ERROR: "); exit(1);};

                ++file_count;
                ++filter_count;

                free(old_full_path);
                free(new_full_path);

                continue;
            };
            // Unrandomizing
            if (unrand && isdigit(file_name[0])){
                // Forming old full path
                form_full_path(&old_full_path, fold_path, file_name);
                //printf("\033[1;32mUnrand old path\033[0m: %s\n", old_full_path);
                // Forming new full path
                unrand_filename(&file_name);
                form_full_path(&new_full_path, fold_path, file_name);
                //printf("\033[1;32mUnrand new path\033[0m: %s\n", new_full_path);

                // Rename file
                if(strlen(old_full_path) != strlen(new_full_path)){
                    if(rename(old_full_path, new_full_path)) {perror("ERROR: "); exit(1);};
                   ++unrand_count;
                };

                free(old_full_path);
                free(new_full_path);
            };
            // Randomizing
            if (random){
                // Forming old full path
                form_full_path(&old_full_path, fold_path, file_name);
                //printf("\033[1;35mRand old path\033[0m: %s\n", old_full_path);
                // Forming new full path
                rand_filename(&file_name, &rand_num_max, &rand_num_len);
                form_full_path(&new_full_path, fold_path, file_name);
                //printf("\033[1;35mRand new path\033[0m (%d max): %s\n", rand_num_max, new_full_path);

                // Rename file
                if(rename(old_full_path, new_full_path)) {perror("ERROR: "); exit(1);};

                free(old_full_path);
                free(new_full_path);
                ++random_count;
            };
            // Serialization
            if(serial){
                // Adding last filename to serialize array
                fold_cont_arr[index] = file_name;
                ++ser_arr_size;
            };

            ++index;
            ++file_count;
            // Update progressbar
            draw_progressbar(&file_count, &num_of_files);
        };

        printf("Filtered: %d Unrandomized: %d Randomized: %d\n", filter_count, unrand_count, random_count);
        
        if (filter) {
            //remove filtered folder if empty
            rmdir(filter_path);
            // free allocated memory
            free(filter_path);
        };

        // Continue loop if no need for serialization
        if (!serial){
            closedir(dir_ptr);
            //printf("Completed folder cycle.\n");
            continue;
        };

        // Performing serialization
        qsort(fold_cont_arr, ser_arr_size, sizeof(char *), comp_str);
        unsigned int num_str_len = snprintf(NULL, 0, "%d", ser_arr_size);
        //printf("Serialization array size: %d\n", ser_arr_size);

        // Draw empty progressbar
        file_count = 0;
        draw_progressbar(&file_count, &ser_arr_size);

        char *old_full_path;
        char *new_full_path;
        for (unsigned int i = 0, bar_ind = 1; i < ser_arr_size; ++i, ++bar_ind){
            // Forming old full path
            form_full_path(&old_full_path, fold_path, fold_cont_arr[i]);
            //printf("\033[1;34mSerial old path\033[0m: %s\n", old_full_path);
            // Forming new full path
            ser_filename(&fold_cont_arr[i], &num_str_len, &i);
            form_full_path(&new_full_path, fold_path, fold_cont_arr[i]);
            //printf("\033[1;34mSerial new path\033[0m: %s\n", new_full_path);
            // Rename file
            if(rename(old_full_path, new_full_path)) {perror("ERROR: "); exit(1);};
            
            draw_progressbar(&bar_ind, &ser_arr_size);

            free(old_full_path);
            free(new_full_path);
        };
        printf("Serialized: %d\n", ser_arr_size);

        // free allocated memory;
        free(fold_cont_arr);

        closedir(dir_ptr);
        //printf("Completed folder cycle.\n");
    };

    // free allocated memory;
    free(folders_to_parse_arr);
    if (filter) {
        for (int i = 0; i < tokens_num; ++i) {
            free(filter_list_arr[i]);
        };
        free(filter_list_arr);
    };

    return 0; 
} 
