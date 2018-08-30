#include "include/git2.h"

#include <string.h>
#include <stdio.h>

typedef struct node {
  git_reference *ref;
  struct node *next;
} node;

int initGitRepo(git_repository **repo, char *dir);
int getAllBranches(git_repository *repo, node **list);
int checkoutBranch(git_repository *repo, const char *branch_name); 
void diffMaster(git_repository *repo, char **out, const char *branch_name, int size);
