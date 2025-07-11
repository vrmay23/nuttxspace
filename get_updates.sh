#!/bin/bash

echo '=================================================='
echo '====         Updating NuttX submodules        ===='
echo '=================================================='
echo

# Update nuttx submodule
cd nuttx || exit 1
echo "[nuttx] Fetching and pulling latest changes..."
git fetch origin
git checkout master
git pull origin master
NUTTX_COMMIT=$(git rev-parse HEAD)
cd ..

# Update apps submodule
cd apps || exit 1
echo "[apps] Fetching and pulling latest changes..."
git fetch origin
git checkout master
git pull origin master
APPS_COMMIT=$(git rev-parse HEAD)
cd ..

# Stage updated submodules
git add nuttx apps

# Generate sync tracking file with current commit hashes
echo "[nuttx]" > .nuttxsync
echo "last_synced = $NUTTX_COMMIT" >> .nuttxsync
echo >> .nuttxsync
echo "[apps]" >> .nuttxsync
echo "last_synced = $APPS_COMMIT" >> .nuttxsync

git add .nuttxsync
git commit -m "Update: NuttX and Apps synced with master ($NUTTX_COMMIT, $APPS_COMMIT)"

echo
echo "Submodules updated and commit hashes tracked in .nuttxsync"

