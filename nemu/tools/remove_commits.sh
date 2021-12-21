#!/bin/sh
git filter-repo --commit-callback '
  if re.match(".*>  [rc].*", commit.message.decode()) is not None:
    commit.skip()
    print(f"skip {commit.message.decode()[:10]}")
  else:
    print(f"use {commit.message.decode()[:10]}")
  return commit
'