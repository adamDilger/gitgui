#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/git2.h"

typedef struct node {
  git_reference *ref;
  struct node *next;
} node;

int main()
{	
  initscr();

  git_repository *repo;
  int error = git_repository_open(&repo, "/Users/adamdilger/rxp/payg/");

  git_branch_iterator *out;
  git_reference *ref;
  git_branch_t out_type;
  const char *name;

  node *list = NULL; 

  git_branch_iterator_new(&out, repo, GIT_BRANCH_LOCAL);

  while (git_branch_next(&ref, &out_type, out) == 0) {
    node *next = (node *)malloc(sizeof(struct node));
    next->next = list;
    next->ref = ref;

    list = next;
  }

  node *tmp = list;

  while (tmp != NULL) {
    git_branch_name(&name, tmp->ref);
    tmp = tmp->next;

    /* printf("%s\n", name); */
    printw("%s\n", name);
  }

  //refresh();
  getch();
  endwin();

  git_repository_free(repo);

  return 0;
}
