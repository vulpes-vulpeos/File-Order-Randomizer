#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// for qsort()
int comp_str(const void *a, const void *b) { return strcmp(*(const char **)a, *(const char **)b); }

void unrand_filename(char **file_name) {
    int index = 0;
    char *sep_ptr = NULL;

    do { ++index; } while (isdigit((*file_name)[index]));
    // '_' separator
    if ((*file_name)[index] == '_') { sep_ptr = *file_name + index + 1; };
    // '. ' separator
    if ((*file_name)[index] == '.' && (*file_name)[index+1] == ' ') { sep_ptr = *file_name + index + 2; };
    // ' - ' separator
    if ((*file_name)[index] == ' ' && (*file_name)[index+1] == '-' && (*file_name)[index+2] == ' ') { sep_ptr = *file_name + index + 3; };
    // Forming filename without numbers if needed
    if (sep_ptr)
        memmove(*file_name, *file_name + (sep_ptr - (*file_name)), strlen(*file_name + (sep_ptr - (*file_name)))+1);
}
void ser_filename(char **file_name, const uint num_str_len, const uint index) {
    // Remove numbers from previous operations if needed
    if (isdigit((*file_name)[0])) { unrand_filename(file_name); };

    char *tmp_ptr = *file_name;
    asprintf(file_name, "%0*u. %s", num_str_len, index+1, tmp_ptr);
    free(tmp_ptr);
}

void rand_filename(char **file_name, const int rand_num_max, const int rand_num_len) {
    // Remove numbers from previous operations if needed
    if (isdigit((*file_name)[0])) { unrand_filename(file_name); };

    ulong rand_num = random() % (rand_num_max + 1);
    char *tmp_ptr = *file_name;
    asprintf(file_name, "%0*lu. %s", rand_num_len, rand_num, tmp_ptr);
    free(tmp_ptr);
}

void draw_progressbar(const unsigned int *index, const unsigned int *total) {
    uint cur_per = ((*index) * 100) / (*total);
    int bar_len = 60;
    uint fill_len = (cur_per * bar_len)/100;

    char *filler = malloc(fill_len+1);
    memset(filler, '#', fill_len);
    filler[fill_len] = '\0';

    printf("Working: [%-*s] %d%%\r", bar_len, filler, cur_per);
    free(filler);

    if (*index == *total) { putc('\n', stdout); };
}

int ignore_file(struct dirent *entry) {
    // Ignore itself and parent folder
    if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) { return 1; };
    // Check if path is folder
    if(entry->d_type != DT_REG) { return 1; };
    // Check for macOS junk
    const char *macOS_junk_names[] = { ".DS_Store", ".localized", "$RECYCLE.BIN", ".fseventsd", ".Spotlight-V100", "System Volume Information" };
    const int array_size = sizeof(macOS_junk_names)/sizeof(const char*);
    //Checking for usual junk folders
    for (int i = 0; i < array_size; ++i) {
        if (strcmp(entry->d_name, macOS_junk_names[i]) == 0) { return 1; };
    };
    //Checking for ._AppleDouble
    if (entry->d_name[0] == '.' && entry->d_name[1] == '_') { return 1; };

    return 0;
}

void fill_filenames_arr (char *folder_path, char **filenames_arr) {
    struct dirent *entry;
    DIR *dir_ptr = opendir(folder_path);
    if(dir_ptr == NULL) { perror("ERROR: "); return; };

    int index = 0;
    while ((entry = readdir(dir_ptr)) != NULL) {
        if (ignore_file(entry)) { continue; };
        filenames_arr[index] = malloc(strlen(entry->d_name) * sizeof(char) +1);
        strcpy(filenames_arr[index], entry->d_name);
        ++index;
    };
    closedir(dir_ptr);
}

int get_num_of_files (char *folder_path) {
    int num_of_files = 0;
    struct dirent *entry;
    DIR *dir_ptr = opendir(folder_path);
    if(dir_ptr == NULL) { perror("ERROR: "); return 1; };

    while ((entry = readdir(dir_ptr)) != NULL){
        if (ignore_file(entry)) { continue; };
        ++num_of_files;
    };
    closedir(dir_ptr);

    return num_of_files;
}

void print_usage_and_exit() {
    printf("Usage: forand [-furs] path/to/folder ...\n");
    printf("-r  randomize files in folder.\n");
    printf("-u  unrandomize files in folder.\n");
    printf("-s  serialize files in folder.\n");
    printf("-d  dry run with debug prints.\n");
    printf("-h  show this message.\n");
    exit(0);
}

int main(int argc, char *argv[]){
    if (argc <= 2) { print_usage_and_exit(); };

    // Parse arguments
    int opt, unrand = 0, random = 0, serial = 0, dry = 0;
    while((opt = getopt(argc, argv, "rusdh")) != -1) {
        switch(opt) {
            case 'r': ++random; break;
            case 'u': ++unrand; break;
            case 's': ++serial; break;
            case 'd': ++dry; break;
            case 'h':
            default: print_usage_and_exit(); break;
        };
    };
    if (optind >= argc) { printf("ERROR: No path provided.\n"); print_usage_and_exit(); };
    if (dry && !unrand && !random && !serial) { print_usage_and_exit(); };

    // Getting and validating paths
    ulong path_len = 0;
    for (int i = optind; i < argc; ++i) {
        if(access(argv[i], F_OK) != 0) { printf("ERROR: Path %s doesn't exist.\n", argv[i]); return 1; };
        // removing trailing '/' if needed
        path_len = strlen(argv[i]);
        if (argv[i][path_len-1] == '/') { argv[i][path_len-1] = '\0'; };
    };

    // Looping through provided paths
    for (; optind < argc; ++optind) {
        // Getting number of files in folder
        uint num_of_files = get_num_of_files(argv[optind]);
        if (dry) { printf("Number of files in %s: %d\n", argv[optind], num_of_files); };
        if (num_of_files <= 0) { continue; };

        // Filling list of file names
        char **filenames_arr = malloc(num_of_files * sizeof(char *));
        fill_filenames_arr(argv[optind], filenames_arr);

        // Calculating random number properties
        int rand_num_len = 0, rand_num_max = 1;
        if (random) {
            srandom(time(NULL));
            rand_num_len = snprintf(NULL, 0, "%d", num_of_files);
            for (int i = 0; i < rand_num_len; ++i){ rand_num_max *= 10; };
            --rand_num_max;
        };

        // Prepairing array for serialization
        char **ser_arr;
        if (serial) { ser_arr = malloc(num_of_files * sizeof(char *)); };

        // Looping through folder contents
        char *random_num;
        uint file_ctr = 0, rand_ctr = 0, unrand_ctr = 0, ser_arr_ind = 0, ser_arr_size = 0;

        // Draw empty progressbar
        draw_progressbar(&file_ctr, &num_of_files);

        for (int i = 0; i < num_of_files; ++i){
            char *old_full_path;
            char *new_full_path;
            if (dry) { printf("File name: %s\n", filenames_arr[i]); };

            // Unrandomizing
            if (unrand && isdigit(filenames_arr[i][0])){
                asprintf(&old_full_path, "%s/%s", argv[optind], filenames_arr[i]);
                unrand_filename(&filenames_arr[i]);
                asprintf(&new_full_path, "%s/%s", argv[optind], filenames_arr[i]);
                if (dry) { printf("\033[1;32mUnrand old path\033[0m: %s\n\033[1;32mUnrand new path\033[0m: %s\n", old_full_path, new_full_path); };

                // Rename file
                if(strlen(old_full_path) != strlen(new_full_path)){
                    if(!dry && rename(old_full_path, new_full_path)) { perror("ERROR: "); exit(1); };
                    ++unrand_ctr;
                };

                free(old_full_path);
                free(new_full_path);
            };
            // Randomizing
            if (random){
                asprintf(&old_full_path, "%s/%s", argv[optind], filenames_arr[i]);
                rand_filename(&filenames_arr[i], rand_num_max,rand_num_len);
                asprintf(&new_full_path, "%s/%s", argv[optind], filenames_arr[i]);
                if (dry) { printf("\033[1;35mRand old path\033[0m: %s\n\033[1;35mRand new path\033[0m (%d max): %s\n", old_full_path, rand_num_max, new_full_path); };

                // Rename file
                if(!dry && rename(old_full_path, new_full_path)) { perror("ERROR: "); exit(1); };

                free(old_full_path);
                free(new_full_path);
                ++rand_ctr;
            };
            // Serialization
            if(serial){
                // Adding last filename to serialize array
                ser_arr[ser_arr_ind] = malloc(strlen(filenames_arr[i]) * sizeof(char) +1);
                strcpy(ser_arr[ser_arr_ind], filenames_arr[i]);
                ++ser_arr_size;
                ++ser_arr_ind;
            };

            ++file_ctr;
            // Update progressbar
            draw_progressbar(&file_ctr, &num_of_files);
        };

        printf("Unrandomized: %d Randomized: %d\n", unrand_ctr, rand_ctr);

        // free allocated memory;
        for (int i = 0; i < num_of_files; ++i) { free(filenames_arr[i]); };
        free(filenames_arr);

        // Continue loop if no need for serialization
        if (!serial){
            if (dry) { printf("Completed folder (un)randomization.\n"); };
            continue;
        };

        // Performing serialization
        qsort(ser_arr, ser_arr_size, sizeof(char *), comp_str);
        unsigned int num_str_len = snprintf(NULL, 0, "%d", ser_arr_size);
        if (dry) { printf("Serialization array size: %d\n", ser_arr_size); };

        // Draw empty progressbar
        file_ctr = 0;
        draw_progressbar(&file_ctr, &ser_arr_size);

        char *old_full_path;
        char *new_full_path;
        for (unsigned int i = 0, bar_ind = 1; i < ser_arr_size; ++i, ++bar_ind){
            asprintf(&old_full_path, "%s/%s", argv[optind], ser_arr[i]);
            ser_filename(&ser_arr[i], num_str_len, i);
            asprintf(&new_full_path, "%s/%s", argv[optind], ser_arr[i]);
            if (dry) { printf("\033[1;34mSerial old path\033[0m: %s\n\033[1;34mSerial new path\033[0m: %s\n",old_full_path, new_full_path); };
            // Rename file
            if(!dry && rename(old_full_path, new_full_path)) { perror("ERROR: "); exit(1); };

            draw_progressbar(&bar_ind, &ser_arr_size);

            free(old_full_path);
            free(new_full_path);
        };
        printf("Serialized: %d\n", ser_arr_size);

        // free allocated memory;
        for (int i = 0; i < ser_arr_size; ++i) { free(ser_arr[i]); };
        free(ser_arr);

        if (dry) { printf("Completed full folder cycle.\n"); };
    };

    return 0;
}
