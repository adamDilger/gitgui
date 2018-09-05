#include <stdlib.h>
#include <unistd.h>

#include <curses.h>
#include <menu.h>

#include "git.h"

void printDiff();

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
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  refresh();

  //setup windows
  WINDOW *w_branch = newwin(LINES, COLS, 0, 0);
  mvwprintw(w_branch, LINES - 1, 5, "q to quit, C to Checkout, D to delete...");
  set_menu_win(my_menu, w_branch);

  int stat_h = LINES - branch_count - 3;
  int stat_y = branch_count + 2;

  WINDOW *w_stat_cont = newwin(stat_h, COLS, stat_y, 0);
  box(w_stat_cont, '|', '-');
  wprintw(w_stat_cont, "** DIFF **");
  wbkgd(w_stat_cont, COLOR_PAIR(1));

  WINDOW *w_stat = newwin(stat_h - 2, COLS - 2, stat_y + 1, 1);
  wbkgd(w_stat, COLOR_PAIR(1));

  post_menu(my_menu);
  wrefresh(w_branch); 
  wrefresh(w_stat_cont);
  wrefresh(w_stat);
        
  //wait for user unput
  int stop = 0;
  int c;

  while(!stop)
  {   

    c = getch();

    if (c == 'j') {
      if (menu_driver(my_menu, REQ_DOWN_ITEM) != 0) continue;
      wrefresh(w_branch);

      const char *branch_name = item_name(current_item(my_menu));

      char *diff = NULL;
      diffMaster(repo, &diff, branch_name, COLS);

      wclear(w_stat);
      mvwprintw(w_stat, 0, 0, diff);
      wrefresh(w_stat);

    } else if (c == 'k') {
      if (menu_driver(my_menu, REQ_UP_ITEM) != 0) continue;
      wrefresh(w_branch);

      const char *branch_name = item_name(current_item(my_menu));

      char *diff = NULL;
      diffMaster(repo, &diff, branch_name, COLS);

      wclear(w_stat);
      mvwprintw(w_stat, 0, 0, diff);
      wrefresh(w_stat);

    } else if (c == 'q') {
      stop = 1;
    } else if (c == 'C') {
      const char *branch_name = item_name(current_item(my_menu));
      checkoutBranch(repo, branch_name); 
      stop = 1;
    } else if (c == 'D') {
      const char *branch_name = item_name(current_item(my_menu));
      deleteBranch(&list, branch_name); 
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
