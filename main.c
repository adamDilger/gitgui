#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <curses.h>
#include <menu.h>

#include "include/git2.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

typedef struct node {
  git_reference *ref;
  struct node *next;
} node;

void checkout(char *branch) {
}

int main()
{	
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));

  git_repository *repo;
  int error = git_repository_open(&repo, cwd);

  git_branch_iterator *out;
  git_reference *ref;
  git_branch_t out_type;
  const char *name;

  node *list = NULL; 
  int count = 0;

  git_branch_iterator_new(&out, repo, GIT_BRANCH_LOCAL);

  while (git_branch_next(&ref, &out_type, out) == 0) {
    count++;
    node *next = (node *)malloc(sizeof(struct node));
    next->next = list;
    next->ref = ref;

    list = next;
  }

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  ITEM **my_items;
  int c;
  MENU *my_menu;
  int n_choices, i;
  ITEM *cur_item;

  n_choices = count;
  my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));

  node *tmp = list; 

  for(int i = 0; i < n_choices; i++) {
    git_branch_name(&name, tmp->ref);
    my_items[i] = new_item(name, "");
    tmp = tmp->next;
  }

  my_items[n_choices] = (ITEM *)NULL;

  my_menu = new_menu((ITEM **)my_items);
  mvprintw(LINES - 2, 0, "q to Exit");
  post_menu(my_menu);
  refresh();

  /* node *tmp = list; */

  /* while (tmp != NULL) { */
  /*   git_branch_name(&name, tmp->ref); */
  /*   tmp = tmp->next; */

  /*   /1* printf("%s\n", name); *1/ */
  /*   printw("%s\n", name); */
  /* } */

  int stop = 0;

  while(!stop)
  {   
    c = getch();

    if (c == 106) {
      menu_driver(my_menu, REQ_DOWN_ITEM);
    } else if (c == 107) {
      menu_driver(my_menu, REQ_UP_ITEM);
    } else if (c == 'C') {
      git_object *tree = NULL;
      git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
      opts.checkout_strategy = GIT_CHECKOUT_SAFE;

      const char *branch_name = item_name(current_item(my_menu));
      char *head_name = "refs/heads/";

      git_revparse_single(&tree, repo, branch_name);
      git_checkout_tree(repo, tree, &opts);

      char* strA = calloc(strlen(branch_name) + strlen(head_name) + 1, sizeof(char));
      sprintf(strA,"%s%s",head_name, branch_name);

      git_repository_set_head(repo, strA);
      git_object_free(tree);

      /* move(20, 0); */
      /* clrtoeol(); */
      /* mvprintw(20, 0, "Item selected is : %s", item_name(current_item(my_menu))); */
      /* pos_menu_cursor(my_menu); */

      stop = 1;
    }
  }

  unpost_menu(my_menu);
  for (i = 0; i < n_choices; ++i) free_item(my_items[i]);
  free_menu(my_menu);
  endwin();

  git_repository_free(repo);

  return 0;
}
