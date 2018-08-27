#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/git2.h"

int main()
{	
  /* char command[400]; */
  /* strcpy(command, "git -C ~/rxp/payg status"); */
  /* popen(command,"r"); */
  /* system(command); */
  /* initscr();			/1* Start curses mode 		  *1/ */

  /* printw(command);	/1* Print Hello World		  *1/ */
  /* refresh();			/1* Print it on to the real screen *1/ */
  /* getch();			/1* Wait for user input *1/ */
  /* endwin();			/1* End curses mode		  *1/ */

  /* char buffer[1024]; */

  /* git(buffer, "ads"); */
  git_repository *repo;
  git_branch_iterator *out;
  int error = git_repository_open(&repo, "/Users/adamdilger/rxp/payg/");

  git_reference *ref;
  git_branch_t out_type;
  const char *name;

  git_branch_iterator_new(&out, repo, GIT_BRANCH_LOCAL);
  git_branch_next(&ref, &out_type, out);
  git_branch_name(&name, ref);
  printf("%s", name);

  git_repository_free(repo);

  return 0;
}
