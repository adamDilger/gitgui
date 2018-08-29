#include <stdlib.h>
#include <unistd.h>

#include <curses.h>
#include <menu.h>

#include "git.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

int main()
{	

  //get git branches
  char dir[1024];
  getcwd(dir, sizeof(dir));

  git_repository *repo;

  if (initGitRepo(&repo, dir) < 0) return -1;

  node *list = NULL; 
  int branch_count = getAllBranches(repo, &list);

  //setup menu
  ITEM **my_items;
  MENU *my_menu;
  ITEM *cur_item;

  my_items = (ITEM **)calloc(branch_count + 1, sizeof(ITEM *));

  const char *name;
  node *tmp = list; 

  for(int i = 0; i < branch_count; i++) {
    git_branch_name(&name, tmp->ref);
    my_items[i] = new_item(name, "");
    tmp = tmp->next;
  }

  my_items[branch_count] = (ITEM *)NULL;
  my_menu = new_menu((ITEM **)my_items);

  //setup/draw to screen
  initscr();
  cbreak();
  noecho();
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_RED);
  init_pair(2, COLOR_GREEN, COLOR_BLUE);
  refresh();

  //setup windows
  WINDOW *w_branch = newwin(LINES / 2, COLS, 0, 0);
  wbkgd(w_branch, COLOR_PAIR(1));
  set_menu_win(my_menu, w_branch);

  WINDOW *w_stat_cont = newwin(LINES / 2, COLS, LINES / 2, 0);
  box(w_stat_cont, '*', '*');
  wbkgd(w_stat_cont, COLOR_PAIR(2));

  WINDOW *w_stat = newwin((LINES / 2) - 2, COLS - 2, LINES / 2 + 1, 1);
  wbkgd(w_stat, COLOR_PAIR(2));

  post_menu(my_menu);
  wrefresh(w_branch); 
  wrefresh(w_stat_cont);
  wrefresh(w_stat);
        
  //wait for user unput
  int stop = 0;
  int c;

  git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
  opts.checkout_strategy = GIT_CHECKOUT_SAFE;

  git_object *commit_master = NULL;
  git_revparse_single(&commit_master, repo, "master");

  git_tree *master_tree;
  git_commit_tree(&master_tree, (git_commit *)commit_master);

  while(!stop)
  {   

    c = getch();

    if (c == 'j') {
      menu_driver(my_menu, REQ_DOWN_ITEM);
      wrefresh(w_branch);
    } else if (c == 'k') {
      menu_driver(my_menu, REQ_UP_ITEM);
      wrefresh(w_branch);

      const char *branch_name = item_name(current_item(my_menu));

      git_object *commit_tree = NULL;
      git_revparse_single(&commit_tree, repo, branch_name);

      git_tree *tree;
      git_commit_tree(&tree, (git_commit *)commit_tree);

      git_diff *diff;
      git_diff_tree_to_tree(&diff, repo, master_tree, tree, NULL);

      git_diff_stats *stats;
      git_diff_get_stats(&stats, diff);

      git_buf buf = GIT_BUF_INIT_CONST(NULL, 0);
      git_diff_stats_to_buf(&buf, stats, GIT_DIFF_STATS_FULL, COLS - 30);

      if (buf.ptr != NULL) {
        mvwprintw(w_stat, 0, 0, buf.ptr);
        wrefresh(w_stat);
      }

    } else if (c == 'q') {
      stop = 1;
    } else if (c == 'C') {
      const char *branch_name = item_name(current_item(my_menu));
      checkoutBranch(repo, branch_name); 
      stop = 1;
    }
  }

  //free memory
  unpost_menu(my_menu);
  for (int i = 0; i < branch_count; ++i) free_item(my_items[i]);

  for (node *tmp = list; tmp != NULL; tmp = tmp->next) 
    git_reference_free(tmp->ref);

  free_menu(my_menu);
  endwin();

  git_repository_free(repo);

  return 0;
}
