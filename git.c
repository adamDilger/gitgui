#include "git.h"

int initGitRepo(git_repository **repo, char *dir) {
  return git_repository_open(repo, dir);
}

int getAllBranches(git_repository *repo, node **list) {
  git_branch_iterator *out;
  git_reference *ref;
  git_branch_t out_type;
  int count = 0;

  git_branch_iterator_new(&out, repo, GIT_BRANCH_LOCAL);

  while (git_branch_next(&ref, &out_type, out) == 0) {
    count++;
    node *next = (node *)malloc(sizeof(struct node));
    next->next = *list;
    next->ref = ref;

    *list = next;
  }

  return count;
}

int checkoutBranch(git_repository *repo, const char *branch_name) {
  git_object *tree = NULL;
  git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
  opts.checkout_strategy = GIT_CHECKOUT_SAFE;

  char *head_name = "refs/heads/";

  git_revparse_single(&tree, repo, branch_name);
  git_checkout_tree(repo, tree, &opts);

  char* strA = calloc(strlen(branch_name) + strlen(head_name) + 1, sizeof(char));
  sprintf(strA,"%s%s",head_name, branch_name);

  git_repository_set_head(repo, strA);
  git_object_free(tree);

  return 0;
}













