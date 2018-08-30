#include "git.h"

int initGitRepo(git_repository **repo, char *dir) {
  git_libgit2_init();
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

void diffMaster(git_repository *repo, char **out, const char *branch_name, int size) {
  git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
  opts.checkout_strategy = GIT_CHECKOUT_SAFE;

  git_object *commit_master = NULL;
  git_revparse_single(&commit_master, repo, "master");

  git_tree *master_tree;
  git_commit_tree(&master_tree, (git_commit *)commit_master);

  git_object *commit_tree = NULL;
  git_revparse_single(&commit_tree, repo, branch_name);

  git_tree *tree;
  git_commit_tree(&tree, (git_commit *)commit_tree);

  git_diff *diff;
  git_diff_tree_to_tree(&diff, repo, master_tree, tree, NULL);

  git_diff_stats *stats;
  git_diff_get_stats(&stats, diff);

  git_buf buf = GIT_BUF_INIT_CONST(NULL, 0);
  git_diff_stats_to_buf(&buf, stats, GIT_DIFF_STATS_FULL, size - 30);

  (*out) = buf.ptr;
}
