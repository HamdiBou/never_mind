#ifndef UTILS_H
#define UTILS_H

typedef struct cell {
char* data;
struct cell* next;
} Cell;
typedef Cell* List;

typedef struct {
    char* name;
    char* hash;
    int mode;
} WorkFile;

typedef struct {
    WorkFile* tab;
    int size;
    int n;
} WorkTree;


// Console UI methods
void header();
void footer();

// Terminal Commands
int hashFile(char* source, char* dest);
char* sha256file(char* file);
#endif

// Methodes liste chainée
List* initList();
Cell* buildCell(char* ch);
void insertFirst(List *L, Cell* C);
char* ctos(Cell* c);
char* ltos(List* L);
Cell* listGet(List* L, int i);
Cell* searchList(List* L, char* str);
//
List* stol(char* s);
void ltof(List* L, char* path);
List* ftol(char* path);
void printList(List* L);

// Methodes Gestion de fichiers sous git
List* listdir(char* root_dir);
int file_exists(char *file);
void cp(char *to, char *from);
char* hashToPath(char* hash);
void blobFile(char* file);

// Enregistrement de plusieurs instantanés
// Manipulation de WORKFILE
WorkFile* createWorkFile(char* name);
char* wfts(WorkFile* wf);
WorkFile* stwf(char* ch);

// Manipulation de WORKTREE
WorkTree* initWorkTree();
int inWorkTree(WorkTree* wt, char* name);
int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode);
char* wtts(WorkTree* wt);
WorkTree* stwt(char* ch);
int wttf(WorkTree* wt, char* file);
WorkTree* ftwt(char* file);

// Enregistrement instantané et restauration d’un WorkTree
char* blobWorkTree(WorkTree* wt);
char* saveWorkTree(WorkTree* wt, char* path);
void restoreWorkTree(WorkTree* wt, char* path);
