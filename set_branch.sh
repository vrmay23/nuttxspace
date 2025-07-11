#!/bin/bash

# ==================================================
# ==            NuttX Submodule Syncer            ==
# ==    Syncs nuttx and apps submodules to a     ==
# ==    selected branch version from upstream     ==
# ==================================================

NUTTX_REMOTE="https://github.com/apache/nuttx.git"
APPS_REMOTE="https://github.com/apache/nuttx-apps.git"

# -----------------------------------------
# Fetch remote branches from a given repo
# Args:
#   $1 - remote git URL
# Returns:
#   prints list of branch names (one per line)
# Why:
#   Uses git ls-remote --heads to get remote branches without cloning
fetch_remote_branches() {
  git ls-remote --heads "$1" | awk '{print $2}' | sed 's|refs/heads/||'
}

# -----------------------------------------
# Get common branches between nuttx and apps
# Args:
#   $1 - array of nuttx branches
#   $2 - array of apps branches
# Returns:
#   prints common branch names (one per line)
# Why:
#   Only versions with both repos having the branch are valid to sync
get_common_branches() {
  local -n branches1=$1
  local -n branches2=$2
  for b1 in "${branches1[@]}"; do
    for b2 in "${branches2[@]}"; do
      if [[ "$b1" == "$b2" ]]; then
        echo "$b1"
        break
      fi
    done
  done
}

# -----------------------------------------
# Convert branch name to numeric version for sorting
# Args:
#   $1 - branch name (e.g. master, releases/12.10)
# Returns:
#   numeric string for sorting (master = 9999)
# Why:
#   Sort by version number descending, master always on top
version_number() {
  local v=$1
  if [[ "$v" == "master" ]]; then
    echo 9999
  else
    v="${v#releases/}"
    if [[ $v =~ ^[0-9]+(\.[0-9]+)*$ ]]; then
      echo "$v"
    else
      echo 0
    fi
  fi
}

# -----------------------------------------
# Sort branches by version descending, master on top
# Args:
#   $1 - array of branch names
# Returns:
#   prints sorted branch names (one per line)
# Why:
#   Clean sorted menu for user selection
sort_branches() {
  local -n branches=$1
  declare -A map=()
  for v in "${branches[@]}"; do
    map["$v"]="$(version_number "$v")"
  done
  for k in "${!map[@]}"; do
    echo "${map[$k]} $k"
  done | sort -t' ' -k1,1nr -k2,2 | awk '{print $2}'
}

# -----------------------------------------
# Print interactive menu and get user's branch choice
# Args:
#   $1 - array of branches (full names)
# Returns:
#   selected full branch name via global var $version
# Why:
#   Simplifies user input and validates choice
select_version() {
  local -n full_branches=$1
  local menu=()
  for b in "${full_branches[@]}"; do
    if [[ "$b" == "master" ]]; then
      menu+=("master")
    else
      menu+=("${b#releases/}")
    fi
  done

  echo "Choose version to sync both nuttx and apps:"
  select choice in "${menu[@]}"; do
    if [[ -n "$choice" ]]; then
      if [[ "$choice" == "master" ]]; then
        version="master"
      else
        version="releases/$choice"
      fi
      echo "You selected version: $version"
      break
    else
      echo "Invalid selection. Try again."
    fi
  done
}

# -----------------------------------------
# Update a submodule directory to specific branch
# Args:
#   $1 - submodule directory (nuttx or apps)
#   $2 - branch name
# Returns:
#   prints commit hash of updated HEAD
# Why:
#   Fetches remote, forcibly resets local branch to remote branch ensuring fresh update
update_submodule() {
  local dir=$1
  local ver=$2
  echo "Updating $dir to version $ver..."
  cd "$dir" || { echo "Failed to enter $dir"; exit 1; }

  # Fetch all refs from origin
  git fetch origin

  # Check if remote branch exists
  if git show-ref --verify --quiet "refs/remotes/origin/$ver"; then
    # Checkout local branch tracking remote, forcibly reset to remote state
    git checkout -B "$ver" "origin/$ver"
    # Force reset hard to match exactly remote branch
    git reset --hard "origin/$ver"
    # Clean untracked files to guarantee exact state
    git clean -fdx
  else
    echo "Branch $ver not found in remote for $dir"
    exit 1
  fi

  # Pull latest changes to local branch (optional, safe)
  git pull origin "$ver"

  local commit_hash
  commit_hash=$(git rev-parse HEAD)
  cd ..
  echo "$dir commit hash: $commit_hash"
  echo "$commit_hash"
}


# -----------------------------------------
# Configure NuttX sim: clean previous build and run configure for sim:nsh
# Why:
#  - ensure a clean environment for consistent build
#  - configure build for sim:nsh (NuttX simulator with NSH shell)
configure_nuttx_sim() {
  echo "Configuring NuttX sim..."

  cd nuttx || { echo "Failed to enter nuttx directory"; exit 1; }
  make distclean
  ./tools/configure.sh sim:nsh

  cd ..
}

# -----------------------------------------
# Compile NuttX sim
# Why:
#  - build the updated binary according to the synced code and config
compile_nuttx_sim() {
  echo "Compiling NuttX sim..."

  cd nuttx || { echo "Failed to enter nuttx directory"; exit 1; }
  make -j$(nproc)

  cd ..
}

# -----------------------------------------
# Run NuttX simulator
# Why:
#  - execute the simulator for testing the system
run_nuttx_sim() {
  echo "Running NuttX sim..."

  cd nuttx || { echo "Failed to enter nuttx directory"; exit 1; }
  ./nuttx

  cd ..
}




# -----------------------------------------
# Main script logic starts here

main() {
  # Fetch branches from both repos
  nuttx_branches=($(fetch_remote_branches "$NUTTX_REMOTE"))
  apps_branches=($(fetch_remote_branches "$APPS_REMOTE"))

  # Get common branches
  common_branches=($(get_common_branches nuttx_branches apps_branches))

  if [ ${#common_branches[@]} -eq 0 ]; then
    echo "No common branches found between nuttx and apps."
    exit 1
  fi

  # Sort branches
  sorted_branches=($(sort_branches common_branches))

  # Select branch version
  select_version sorted_branches

  # Update submodules
  NUTTX_COMMIT=$(update_submodule nuttx "$version")
  APPS_COMMIT=$(update_submodule apps "$version")

  # Commit updates in main repo
  git add nuttx apps

  echo "[nuttx]" > .nuttxsync
  echo "last_synced = $NUTTX_COMMIT" >> .nuttxsync
  echo >> .nuttxsync
  echo "[apps]" >> .nuttxsync
  echo "last_synced = $APPS_COMMIT" >> .nuttxsync

  git add .nuttxsync
  git commit -m "Sync NuttX and Apps to branch $version ($NUTTX_COMMIT, $APPS_COMMIT)"

  # Show confirmation
  echo
  echo "Submodules synced and commit hashes tracked in .nuttxsync"
  echo
  echo "===== Confirming submodule status ====="
  git submodule status
  echo
  echo "===== nuttx submodule details ====="
  (cd nuttx && git branch --show-current && git log -1 --oneline)
  echo
  echo "===== apps submodule details ====="
  (cd apps && git branch --show-current && git log -1 --oneline)
  echo
  echo "===== Contents of .nuttxsync ====="
  cat .nuttxsync

  # Now call build + run functions if you want:
  #configure_nuttx_sim
  #compile_nuttx_sim
  #run_nuttx_sim
}



main



