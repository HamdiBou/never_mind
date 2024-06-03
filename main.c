#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include "utils/utils.h" 

#define ROOT "/home/zahrane-rabhi/projet-c"
#define SOURCE "Tests/source.txt"
#define DESTINATION "Tests/dest.txt"

// Q-1.3
void hash_to_file() {
    char source[] = SOURCE;;
    char dest[] = DESTINATION;

    int result = hashFile(source, dest);
    if (result == 0) {
        printf("Hash written to %s successfully.\n", dest);
    } else {
        printf("An error occurred.\n");
    }

    printf("%d", result);
}

// Q-1.4
void get_hash_from_file() {
    char file[] = SOURCE;
    char *hash = sha256file(file);

    if (hash) {
        printf("Hash of %s: %s\n", file, hash);
        free(hash);
    } else {
        printf("An error occurred.\n");
    }

}

// 
void liste_chainee() {
List* myList = initList();
    insertFirst(myList, buildCell("Hello"));
    insertFirst(myList, buildCell("World"));
    insertFirst(myList, buildCell("Test"));

    char* listString = ltos(myList);
    printf("List: %s\n", listString);
    free(listString);

    Cell* cell = listGet(myList, 1);
    if (cell) {
        printf("Element at index 1: %s\n", ctos(cell));
    }

    cell = searchList(myList, "World");
    if (cell) {
        printf("Found: %s\n", ctos(cell));
    } else {
        printf("Not found\n");
    }

    // Free the list
    while (*myList) {
        Cell* temp = *myList;
        *myList = (*myList)->next;
        free(temp->data);
        free(temp);
    }
    free(myList);
}

void liste_chainee_file() {
 List* myList = initList();
    insertFirst(myList, buildCell("Hello"));
    insertFirst(myList, buildCell("World"));
    insertFirst(myList, buildCell("Test"));

    char* listString = ltos(myList);
    printf("List: %s\n", listString);
    free(listString);

    Cell* cell = listGet(myList, 1);
    if (cell) {
        printf("Element at index 1: %s\n", ctos(cell));
    }

    cell = searchList(myList, "World");
    if (cell) {
        printf("Found: %s\n", ctos(cell));
    } else {
        printf("Not found\n");
    }

    ltof(myList, "Tests/liste.txt");

    List* newList = ftol("Tests/liste.txt");
    printList(newList);

    // Free the lists
    while (*myList) {
        Cell* temp = *myList;
        *myList = (*myList)->next;
        free(temp->data);
        free(temp);
    }
    free(myList);

    while (*newList) {
        Cell* temp = *newList;
        *newList = (*newList)->next;
        free(temp->data);
        free(temp);
    }
    free(newList);
}

void gestion_fichiers_git() {
    // Test de la fonction listdir
    printf("Testing listdir:\n");
    List* myList = listdir(".");
    if (myList != NULL) {
        Cell* current = *myList;
        while (current) {
            printf("%s\n", current->data);
            current = current->next;
        }
    } else {
        printf("Could not list directory contents.\n");
    }

    // Test de la fonction file_exists
    printf("\nTesting file_exists:\n");
    if (file_exists("Tests/source.txt")) {
        printf("File Tests/source.txt exists.\n");
    } else {
        printf("File Tests/source.txt does not exists.\n");
    }

    // Test de la fonction cp
    printf("\nTesting cp(copy):\n");
    cp("Tests/copy_of_test.txt", "Tests/source.txt");
    if (file_exists("Tests/copy_of_test.txt")) {
        printf("File copy_of_test.txt created successfully.\n");
    } else {
        printf("Failed to create file copy_of_test.txt.\n");
    }

    // Test de la fonction hashToPath
    printf("\nTesting hashToPath:\n");
    char* hash = "e99a18c428cb38d5f260853678922e03";
    char* path = hashToPath(hash);
    if (path != NULL) {
        printf("Hash path: %s\n", path);
        free(path);
    } else {
        printf("Failed to convert hash to path.\n");
    }

    // Test de la fonction blobFile
    printf("\nTesting blobFile:\n");
    blobFile("main.c");

    // Libérer la liste myList
    while (*myList) {
        Cell* temp = *myList;
        *myList = (*myList)->next;
        free(temp->data);
        free(temp);
    }
    free(myList);
}

void enregistrement_de_plusieurs_instantanés() {
// Initialisation du WorkTree
    WorkTree* myWorkTree = initWorkTree();

    // Ajout de fichiers au WorkTree
    appendWorkTree(myWorkTree, "Tests/pythonProject/app.py", "hash1", 777);
    appendWorkTree(myWorkTree, "Tests/pythonProject/README.md", "hash2", 755);

    // Conversion du WorkTree en chaîne de caractères et affichage
    char* wt_str = wtts(myWorkTree);
    printf("WorkTree as string:\n%s", wt_str);
    free(wt_str);

    // Écriture du WorkTree dans un fichier
    wttf(myWorkTree, "Tests/worktree.txt");

    // Lecture du WorkTree à partir du fichier
    WorkTree* readWorkTree = ftwt("Tests/worktree.txt");
    char* read_wt_str = wtts(readWorkTree);
    printf("Read WorkTree from file:\n%s", read_wt_str);
    free(read_wt_str);

    // Nettoyage de la mémoire
    for (int i = 0; i < myWorkTree->n; i++) {
        free(myWorkTree->tab[i].name);
        free(myWorkTree->tab[i].hash);
    }
    free(myWorkTree->tab);
    free(myWorkTree);

    for (int i = 0; i < readWorkTree->n; i++) {
        free(readWorkTree->tab[i].name);
        free(readWorkTree->tab[i].hash);
    }
    free(readWorkTree->tab);
    free(readWorkTree);

}

void test_BlobWorkTree(){
     // Sample WorkTree
    WorkTree* wt = initWorkTree();
    appendWorkTree(wt, "Test/pythonProject/app.py", "hash1", 644);
    appendWorkTree(wt, "Test/pythonProject/README.md", "hash2", 644);
    appendWorkTree(wt, "Test/PythonProject", "hash3", 755);

    // Use blobWorkTree to create an instant snapshot
    char* snapshot_hash = blobWorkTree(wt);
    printf("Snapshot hash: %s\n", snapshot_hash);

    // Cleanup
    free(snapshot_hash);
    free(wt->tab);
    free(wt);
}

void enregistrement_instantané_et_restauration_WorkTree(){
    // Création d'un WorkTree
    WorkTree* wt = initWorkTree();
    appendWorkTree(wt, "test1.txt", "hash1", 644);
    appendWorkTree(wt, "test2.txt", "hash2", 644);

    // Save the WorkTree to a file
    char* saved_hash = saveWorkTree(wt, "Tests/worktree.txt");
    printf("Saved WorkTree hash: %s\n", saved_hash);

    // Restore the WorkTree from the file
    WorkTree* restored_wt = ftwt("Tests/worktree.txt");
    printf("Restored WorkTree:\n");
    printf("%s\n", wtts(restored_wt));

    // Restore the files to their original locations
    restoreWorkTree(restored_wt, "Tests/worktree.txt");

    // Clean up
    free(saved_hash);
    free(wt);
    free(restored_wt);
}

int main() {
    enregistrement_instantané_et_restauration_WorkTree();
    
}
