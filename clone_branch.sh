#!/bin/bash

# ==================================================
# ==          NuttX Submodule Syncer            ==
# ==   Removes and clones nuttx and apps          ==
# ==   submodules to a selected branch version    ==
# ==   from upstream, ensuring clean sync.        ==
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
# Remove and then clone a submodule directory to a specific branch
# Args:
#   $1 - submodule directory (nuttx or apps)
#   $2 - branch name
#   $3 - remote git URL for the submodule
# Returns:
#   prints commit hash of the cloned HEAD
# Why:
#   Guarantees a clean state by removing and re-cloning to the exact branch.
clone_submodule() {
  local dir=$1
  local ver=$2
  local remote_url=$3

  echo "Preparing to clone $dir to version $ver..."

  # 1. Clean up potential old submodule references in .git/modules
  # This targets the 'absorbed' git directory for the submodule
  if [ -d ".git/modules/$dir" ]; then
    echo "Removing old .git/modules/$dir entry..."
    rm -rf ".git/modules/$dir" || { echo "Failed to remove .git/modules/$dir"; exit 1; }
  fi

  # 2. Ensure the submodule is de-initialized from the main repo's perspective
  # This also removes the entry from .git/config
  echo "De-initializing submodule $dir..."
  git submodule deinit -f "$dir" &>/dev/null || true # Suppress error if not initialized or already de-initialized
  git rm --cached "$dir" &>/dev/null || true # Remove from index if it's there as a submodule

  # 3. Remove the existing work tree directory (if any)
  if [ -d "$dir" ]; then
    echo "Removing existing work tree directory: $dir"
    rm -rf "$dir" || { echo "Failed to remove $dir directory"; exit 1; }
  fi

  # 4. Clone the repository to the specific branch
  echo "Cloning $dir with branch '$ver' from $remote_url..."
  if git clone --branch "$ver" --single-branch "$remote_url" "$dir"; then
    echo "Cloning of $dir for branch '$ver' successful."
    local commit_hash
    commit_hash=$(cd "$dir" && git rev-parse HEAD)
    echo "$dir commit hash: $commit_hash"
    echo "$commit_hash" # Return the hash
  else
    echo "Failed to clone $dir with branch '$ver'. This branch might not exist in the remote repository."
    exit 1
  fi
}

# -----------------------------------------
# Configure NuttX sim: clean previous build and run configure for sim:nsh
# Why:
#   - ensure a clean environment for consistent build
#   - configure build for sim:nsh (NuttX simulator with NSH shell)
configure_nuttx_sim() {
  echo "Configuring NuttX sim..."

  cd nuttx || { echo "Failed to enter nuttx directory"; exit 1; }
  make distclean # Ensures all build artifacts are removed
  ./tools/configure.sh sim:nsh || { echo "Failed to configure NuttX sim"; exit 1; }

  cd ..
}

# -----------------------------------------
# Compile NuttX sim
# Why:
#   - build the updated binary according to the synced code and config
compile_nuttx_sim() {
  echo "Compiling NuttX sim..."

  cd nuttx || { echo "Failed to enter nuttx directory"; exit 1; }
  make -j$(nproc) || { echo "Failed to compile NuttX sim"; exit 1; }

  cd ..
}

# -----------------------------------------
# Run NuttX simulator
# Why:
#   - execute the simulator for testing the system
run_nuttx_sim() {
  echo "Running NuttX sim..."

  cd nuttx || { echo "Failed to enter nuttx directory"; exit 1; }
  ./nuttx || { echo "Failed to run NuttX sim"; exit 1; }

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

  # --- New approach: Remove and re-clone submodules ---
  NUTTX_COMMIT=$(clone_submodule nuttx "$version" "$NUTTX_REMOTE")
  APPS_COMMIT=$(clone_submodule apps "$version" "$APPS_REMOTE")
  # ---------------------------------------------------

  # Update submodule references in the main repository's .gitmodules
  # This makes sure your main repository tracks the new commit hash
  echo "Updating main repository's submodule references..."
  git submodule update --init --recursive || { echo "Failed to update submodule references"; exit 1; }

  # Add and commit updates in main repo
  git add nuttx apps

  # Generate .nuttxsync file with new commit hashes
  echo "[nuttx]" > .nuttxsync
  echo "last_synced = $NUTTX_COMMIT" >> .nuttxsync
  echo >> .nuttxsync
  echo "[apps]" >> .nuttxsync
  echo "last_synced = $APPS_COMMIT" >> .nuttxsync

  git add .nuttxsync
  git commit -m "Sync NuttX and Apps to branch $version ($NUTTX_COMMIT, $APPS_COMMIT)" --allow-empty-message --no-verify || { echo "No changes to commit in main repository, or commit failed."; }


  rm -rf nuttx/.git
  rm -rf apps/.git

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

  # Now call build + run functions:
  echo
  echo "--- Starting NuttX Build Process ---"
  #configure_nuttx_sim
  #compile_nuttx_sim
  # run_nuttx_sim
}

main
