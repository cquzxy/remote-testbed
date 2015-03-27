﻿#summary How to setup and synchronize between git and SVN
#labels Section-Project

# Synchronizing the SVN Repository #

As of around 2007-11-01, the SVN synchronization process tries to mirror the history as good as possible. Before this date, synchronization was batched into merges.

## Setting up the SVN transport ##

Assumes `$module` is set to either of `remote-{db,gui,mci,ws}`
and `$username` is set to the googlecode username.

```
prompt> cd $module
prompt> git svn init --username=$username https://remote-testbed.googlecode.com/svn/trunk/$module
```

After this you can query the configuration using `git config`. For the `remote-mci` module it should say something like the following:

```
prompt> git config --get-regexp svn-remote
svn-remote.svn.url https://remote-testbed.googlecode.com/svn/trunk/remote-mci
svn-remote.svn.fetch :refs/remotes/git-svn
```

Now, fetch the SVN changes, which will create the `git-svn` remote reference, which git-svn uses to keep track of the upstream SVN repository state.

```
prompt> git svn fetch
r11 = 37275f86ebaa13616554dc5ddbeb1112d6e897f3 (git-svn)
```

Next, create a local branch for tracking what in `master` has been synchronized to SVN:

```
prompt> git branch svn/master master
prompt> git branch -v
* master            e594a0c Ignore tag and *.tar.gz files
  svn/master        e594a0c Ignore tag and *.tar.gz files
```

This concludes the setup.

## Updating the SVN repository with the newest changes ##

The following script can be used to update SVN with new changes in the `master` branch. It will create a temporary branch for rebasing all new changes on top of the SVN trunk (remote branch `git-svn`) and commit them to the SVN repository:

```
#!/bin/sh

HEAD="$(git symbolic-ref HEAD)" || exit 1

die()
{
        echo "git-svn-update: $*"
        exit 1
}

git svn fetch || die "failed to fetch SVN changes"
git branch -f svn/tmp master || die "failed to update/create temporary working branch"
git rebase --onto git-svn svn/master svn/tmp || die "failed to rebase on top of git svn branch"
git svn dcommit || die "failed to update SVN repository"
git branch -f svn/master master
git checkout "${HEAD#refs/heads/}"
git branch -D svn/tmp
```

Note, only run this script in a "clean" working directory to avoid problems with local changes that will cause checkout or rebase to error out.