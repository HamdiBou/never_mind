#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include "utils.h" 
#include <sys/stat.h>

#define RESET_STYLE "\033[0m"
#define PURPLE "\033[95m"
#define GREEN "\033[92m"
#define WORKTREE_SIZE 10

// Fancy header for the project output console
void header() {
    printf("%sZahrane@Zahry %sGit ~\n%s",GREEN, PURPLE, RESET_STYLE);
}

// Just some code formatting
void footer() {
    printf("\n\n");
    getchar();
}

// Terminal Methods
int hashFile(char* source, char* dest) {
    char command[1024];

    // Create a temporary file name
    char tmpFileName[] = "/tmp/hashXXXXXX";
    int fd = mkstemp(tmpFileName);
    if (fd == -1) {
        perror("Error creating temporary file");
        return -1;
    }
    close(fd);

    // Construct the command to calculate hash using sha256sum
    snprintf(command, sizeof(command), "sha256sum %s > %s", source, tmpFileName);

    // Execute the command
    if (system(command) != 0) {
        perror("Error executing sha256sum command");
        remove(tmpFileName); // Clean up in case of error
        return -1;
    }

    // Copy the hash from temporary file to destination file
    if (rename(tmpFileName, dest) != 0) {
        perror("Error renaming temporary file to destination file");
        remove(tmpFileName); // Clean up in case of error
        return -1;
    }

    return 0;
}
char* sha256file(char* file) {
    char command[1024];
    char tmpFileName[] = "/tmp/hashXXXXXX";
    int fd = mkstemp(tmpFileName);
    if (fd == -1) return NULL;
    close(fd);

    snprintf(command, sizeof(command), "sha256sum %s > %s", file, tmpFileName);
    if (system(command) != 0) {
        remove(tmpFileName);
        return NULL;
    }

    FILE *tmpFile = fopen(tmpFileName, "r");
    if (!tmpFile) {
        remove(tmpFileName);
        return NULL;
    }

    char *hash = malloc(65);
    if (hash && fscanf(tmpFile, "%64s", hash) != 1) {
        free(hash);
        hash = NULL;
    }

    fclose(tmpFile);
    remove(tmpFileName);
    return hash;
}

///////////////////////////////////////////////////////
//Implémentation d’une liste de chaînes de caractèes///
///////////////////////////////////////////////////////

// Fonction pour initialiser une liste vide
List* initList() {
    List* list = (List*)malloc(sizeof(List));
    if (list == NULL) {
        perror("Error allocating memory for the list");
        exit(EXIT_FAILURE);
    }
    *list = NULL;
    return list;
}

Cell* buildCell(char* ch) {
    Cell* newCell = (Cell*)malloc(sizeof(Cell));
    if (newCell == NULL) {
        perror("Error allocating memory for new cell");
        exit(EXIT_FAILURE);
    }
    newCell->data = strdup(ch);
    newCell->next = NULL;
    return newCell;
}

void insertFirst(List *L, Cell* C) {
    C->next = *L;
    *L = C;
}

char* ctos(Cell* c) {
    return strdup(c->data);
}

char* ltos(List* L) {
    if (*L == NULL) return strdup("");
    
    size_t length = 0;
    Cell* current = *L;
    while (current) {
        length += strlen(current->data) + 1; // +1 for the '|' separator
        current = current->next;
    }

    char* result = (char*)malloc(length);
    if (!result) {
        perror("Error allocating memory for list string");
        exit(EXIT_FAILURE);
    }
    result[0] = '\0';

    current = *L;
    while (current) {
        strcat(result, current->data);
        if (current->next) strcat(result, "|");
        current = current->next;
    }
    return result;
}

Cell* listGet(List* L, int i) {
    Cell* current = *L;
    int index = 0;
    while (current && index < i) {
        current = current->next;
        index++;
    }
    return current;
}

Cell* searchList(List* L, char* str) {
    Cell* current = *L;
    while (current) {
        if (strcmp(current->data, str) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

List* stol(char* s) {
    List* list = initList();
    char* dup = strdup(s);
    if (!dup) {
        perror("Error duplicating string");
        exit(EXIT_FAILURE);
    }
    char* token = strtok(dup, "|");
    while (token != NULL) {
        Cell* newCell = buildCell(token);
        insertFirst(list, newCell);
        token = strtok(NULL, "|");
    }
    free(dup);
    return list;
}

void ltof(List* L, char* path) {
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    Cell* current = *L;
    while (current) {
        fprintf(file, "%s\n", current->data);
        current = current->next;
    }
    fclose(file);
}

List* ftol(char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }
    List* list = initList();
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
        Cell* newCell = buildCell(buffer);
        insertFirst(list, newCell);
    }
    fclose(file);
    return list;
}

void printList(List* L) {
    Cell* current = *L;
    while (current) {
        printf("%s ", current->data);
        current = current->next;
    }
    printf("\n");
}

// Methodes Gestion de fichiers sous git
List* listdir(char* root_dir) {
    DIR *dp;
    struct dirent *ep;
    List* list = initList();

    dp = opendir(root_dir);
    if (dp != NULL) {
        while ((ep = readdir(dp)) != NULL) {
            insertFirst(list, buildCell(ep->d_name));
        }
        closedir(dp);
    } else {
        perror("Couldn't open the directory");
        return NULL;
    }
    return list;
}

int file_exists(char *file) {
    List* list = listdir(".");
    if (list == NULL) return 0;

    Cell* current = *list;
    while (current) {
        if (strcmp(current->data, file) == 0) {
            // Free the list
            while (*list) {
                Cell* temp = *list;
                *list = (*list)->next;
                free(temp->data);
                free(temp);
            }
            free(list);
            return 1;
        }
        current = current->next;
    }
    // Free the list
    while (*list) {
        Cell* temp = *list;
        *list = (*list)->next;
        free(temp->data);
        free(temp);
    }
    free(list);
    return 0;
}

void cp(char *to, char *from) {
    if (!file_exists(from)) {
        fprintf(stderr, "Source file does not exist.\n");
        return;
    }

    FILE *source = fopen(from, "r");
    FILE *destination = fopen(to, "w");
    if (source == NULL || destination == NULL) {
        perror("Error opening file");
        if (source) fclose(source);
        if (destination) fclose(destination);
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), source)) {
        fputs(buffer, destination);
    }

    fclose(source);
    fclose(destination);
}

char* hashToPath(char* hash) {
    if (strlen(hash) < 2) {
        fprintf(stderr, "Invalid hash length.\n");
        return NULL;
    }

    char* path = (char*)malloc(strlen(hash) + 2);
    if (!path) {
        perror("Error allocating memory for path");
        return NULL;
    }

    snprintf(path, strlen(hash) + 2, "%c%c/%s", hash[0], hash[1], hash + 2);
    return path;
}

void blobFile(char* file) {
    if (!file_exists(file)) {
        fprintf(stderr, "Source file does not exist.\n");
        return;
    }

    // Calculate hash of the file content
    char* hash = sha256file(file);
    if (!hash) {
        fprintf(stderr, "Error calculating file hash.\n");
        return;
    }

    // Get the path based on hash
    char* path = hashToPath(hash);
    if (!path) {
        free(hash);
        return;
    }

    // Create the directory if it doesn't exist
    char dir[3];
    snprintf(dir, sizeof(dir), "%c%c", hash[0], hash[1]);
    char mkdirCommand[128];
    snprintf(mkdirCommand, sizeof(mkdirCommand), "mkdir -p %s", dir);
    if (system(mkdirCommand) != 0) {
        perror("Error creating directory");
        free(hash);
        free(path);
        return;
    }

    // Copy the file to the new location
    cp(path, file);

    free(hash);
    free(path);
}


////////////////////////////////////////////
//Enregistrement de plusieurs instantanés///
////////////////////////////////////////////
int getChmod(const char *path){
    struct stat ret;
    if (stat(path, &ret) == -1) {
        return -1;
    }
    return
        (ret.st_mode & S_IRUSR)|(ret.st_mode & S_IWUSR)|(ret.st_mode & S_IXUSR)|/*owner*/
        (ret.st_mode & S_IRGRP)|(ret.st_mode & S_IWGRP)|(ret.st_mode & S_IXGRP)|/*group*/
        (ret.st_mode & S_IROTH)|(ret.st_mode & S_IWOTH)|(ret.st_mode & S_IXOTH);/*other*/
    }

void setMode(int mode, char* path){
    char buff [100];
    sprintf(buff, "chmod %d %s", mode, path);
    system(buff);
    } 

WorkFile* createWorkFile(char* name) {
    WorkFile* wf = (WorkFile*)malloc(sizeof(WorkFile));
    wf->name = strdup(name);
    wf->hash = NULL;
    wf->mode = 0;
    return wf;
}

char* wfts(WorkFile* wf) {
    char* result = (char*)malloc(1024 * sizeof(char));
    sprintf(result, "%s\t%s\t%d", wf->name, wf->hash ? wf->hash : "", wf->mode);
    return result;
}

WorkFile* stwf(char* ch) {
    WorkFile* wf = (WorkFile*)malloc(sizeof(WorkFile));
    wf->name = strtok(ch, "\t");
    wf->hash = strtok(NULL, "\t");
    wf->mode = atoi(strtok(NULL, "\t"));
    return wf;
}

// Manipulation de WORKTREE

WorkTree* initWorkTree() {
    WorkTree* wt = (WorkTree*)malloc(sizeof(WorkTree));
    wt->tab = (WorkFile*)malloc(WORKTREE_SIZE * sizeof(WorkFile));
    wt->size = WORKTREE_SIZE;
    wt->n = 0;
    return wt;
}

int inWorkTree(WorkTree* wt, char* name) {
    for (int i = 0; i < wt->n; i++) {
        if (strcmp(wt->tab[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode) {
    if (inWorkTree(wt, name) != -1) {
        return -1;  // Already exists
    }
    if (wt->n >= wt->size) {
        return -1;  // No space left
    }
    WorkFile* wf = createWorkFile(name);
    wf->hash = strdup(hash);
    wf->mode = mode;
    wt->tab[wt->n++] = *wf;
    return 0;  // Success
}

char* wtts(WorkTree* wt) {
    char* result = (char*)malloc(1024 * sizeof(char));
    result[0] = '\0';
    for (int i = 0; i < wt->n; i++) {
        char* wf_str = wfts(&wt->tab[i]);
        strcat(result, wf_str);
        strcat(result, "\n");
        free(wf_str);
    }
    return result;
}

WorkTree* stwt(char* ch) {
    WorkTree* wt = initWorkTree();
    char* token = strtok(ch, "\n");
    while (token != NULL) {
        WorkFile* wf = stwf(token);
        appendWorkTree(wt, wf->name, wf->hash, wf->mode);
        token = strtok(NULL, "\n");
    }
    return wt;
}

int wttf(WorkTree* wt, char* file) {
    FILE* fp = fopen(file, "w");
    if (!fp) {
        return -1;  // Error opening file
    }
    char* wt_str = wtts(wt);
    fprintf(fp, "%s", wt_str);
    fclose(fp);
    free(wt_str);
    return 0;
}

WorkTree* ftwt(char* file) {
    FILE* fp = fopen(file, "r");
    if (!fp) {
        return NULL;  // Error opening file
    }
    char buffer[1024];
    char* content = (char*)malloc(1024 * sizeof(char));
    content[0] = '\0';
    while (fgets(buffer, sizeof(buffer), fp)) {
        strcat(content, buffer);
    }
    fclose(fp);
    WorkTree* wt = stwt(content);
    free(content);
    return wt;
}

// Enregistrement instantané et restauration d’un WorkTree
char* blobWorkTree(WorkTree* wt) {
    // Create a temporary file
    char temp_filename[] = "temp_file.XXXXXX";
    int temp_fd = mkstemp(temp_filename);
    if (temp_fd == -1) {
        fprintf(stderr, "Error creating temporary file\n");
        exit(1);
    }
    close(temp_fd);

    // Open the temporary file for writing
    FILE* temp_file = fopen(temp_filename, "w");
    if (temp_file == NULL) {
        fprintf(stderr, "Error opening temporary file\n");
        exit(1);
    }

    // Write the WorkTree content to the temporary file
    for (int i = 0; i < wt->n; i++) {
        WorkFile* wf = &(wt->tab[i]);
        fprintf(temp_file, "%s %s %d\n", wf->name, wf->hash, wf->mode);
    }

    // Close the temporary file
    fclose(temp_file);

    // Calculate the hash of the temporary file
    char* hash = sha256file(temp_filename);
    if (hash == NULL) {
        fprintf(stderr, "Error calculating hash of temporary file\n");
        exit(1);
    }

    // Rename the temporary file to include the hash in the filename
    char new_filename[strlen(hash) + 3]; // 2 for ".t" and 1 for null terminator
    sprintf(new_filename, "%s.t", hash);
    rename(temp_filename, new_filename);

    return hash;
}
void restoreWorkTree(WorkTree* wt, char* path) {
    // Read the contents of the saved WorkTree file
    WorkTree* saved_wt = ftwt(path);
    if (saved_wt == NULL) {
        fprintf(stderr, "Error reading saved WorkTree\n");
        return;
    }

    // Process each WorkFile in the saved WorkTree
    for (int i = 0; i < saved_wt->n; i++) {
        WorkFile* saved_wf = &(saved_wt->tab[i]);

        // Check if the file exists in the current WorkTree
        int index = inWorkTree(wt, saved_wf->name);
        if (index != -1) {
            WorkFile* wf = &(wt->tab[index]);

            // Restore the file using its saved hash and mode
            cp(saved_wf->name, hashToPath(saved_wf->hash));
            setMode(saved_wf->mode, saved_wf->name);
        } else {
            // File does not exist in the current WorkTree, so create it
            cp(saved_wf->name, hashToPath(saved_wf->hash));
            setMode(saved_wf->mode, saved_wf->name);
        }
    }

    // Clean up
    free(saved_wt);
}
char* saveWorkTree(WorkTree* wt, char* path) {
    // Create a temporary WorkTree to store the modified WorkFiles
    WorkTree* temp_wt = initWorkTree();

    // Process each WorkFile in the WorkTree
    for (int i = 0; i < wt->n; i++) {
        WorkFile* wf = &(wt->tab[i]);

        // If it's a file, create an instant snapshot and update its hash and mode
        if (file_exists(wf->name)) {
            blobFile(wf->name);
            char* hash = sha256file(wf->name);
            if (hash == NULL) {
                fprintf(stderr, "Error calculating hash of file %s\n", wf->name);
                exit(1);
            }
            wf->hash = hash;
            wf->mode = getChmod(wf->name);
        }

        // If it's a directory, recursively process its contents
        else {
            // Create a new WorkTree for the directory
            WorkTree* subdir_wt = initWorkTree();
            List* dir_list = listdir(wf->name);
            if (dir_list == NULL) {
                fprintf(stderr, "Error listing directory %s\n", wf->name);
                exit(1);
            }

            // Add files and subdirectories to the new WorkTree
            Cell* current = *dir_list;
            while (current) {
                char* dir_entry = current->data;
                appendWorkTree(subdir_wt, dir_entry, NULL, getChmod(dir_entry));
                current = current->next;
            }

            // Recursively process the new WorkTree
            char* subdir_hash = saveWorkTree(subdir_wt, path);
            appendWorkTree(temp_wt, wf->name, subdir_hash, getChmod(wf->name));

            // Clean up
            free(dir_list);
            free(subdir_wt);
            free(subdir_hash);
        }
    }

    // Create an instant snapshot of the modified WorkTree
    char* wt_hash = blobWorkTree(temp_wt);

    // Clean up
    free(temp_wt);

    // Write the modified WorkTree to the specified path
    wttf(wt, path);

    return wt_hash;
}
