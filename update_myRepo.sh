#!/bin/bash

echo "==========================================="
echo "=== Commit and push all local changes ===="
echo "==========================================="
echo

# --- Configuration ---
REMOTE_BRANCH="main" # Or 'master' if your remote uses it
# ---------------------

# Obter hashes abreviados dos submodules
# Note: These lines will only work if nuttx and apps directories exist.
# If the script failed to clone them previously, this might be empty.
NUTTX_HASH=$(cd nuttx && git rev-parse --short HEAD 2>/dev/null || echo "N/A")
APPS_HASH=$(cd apps && git rev-parse --short HEAD 2>/dev/null || echo "N/A")

# Prompt para o usuário adicionar uma frase extra na mensagem de commit
read -p "Add extra commit message (optional): " USER_MSG

# Construir a mensagem de commit base
COMMIT_MSG="Update submodules: nuttx@$NUTTX_HASH apps@$APPS_HASH on $(date '+%Y-%m-%d %H:%M:%S')"

# Se o usuário digitou algo, adiciona à mensagem
if [ -n "$USER_MSG" ]; then
  COMMIT_MSG="$COMMIT_MSG - $USER_MSG"
fi

# Adicionar todas as mudanças, incluindo deleções e submodules
echo "Adding all changes to Git index..."
git add -A || { echo "Failed to add changes."; exit 1; }

# Fazer commit apenas se houver mudanças staged
if git diff --cached --quiet; then
  echo "No changes to commit. Working tree is clean."
else
  echo "Committing changes..."
  git commit -m "$COMMIT_MSG" || { echo "Failed to commit changes."; exit 1; }
fi

# --- NOVO PASSO: Puxar as últimas mudanças do remoto antes de enviar ---
echo "Pulling latest changes from origin/$REMOTE_BRANCH before pushing..."
if git pull origin "$REMOTE_BRANCH"; then
  echo "Successfully pulled latest changes."
else
  echo "------------------------------------------------------------------------------------------------"
  echo "!!! Git Pull failed or encountered conflicts !!!"
  echo "Please resolve conflicts manually, then 'git add .' and 'git pull origin $REMOTE_BRANCH' again."
  echo "Alternatively, you can run 'git reset --hard origin/$REMOTE_BRANCH' to discard your local changes."
  echo "------------------------------------------------------------------------------------------------"
  exit 1 # Exit with error
fi
# --------------------------------------------------------------------

# Fazer push para o seu repositório remoto
echo "Pushing changes to origin/$REMOTE_BRANCH..."
git push origin "$REMOTE_BRANCH" || { echo "Failed to push changes to remote."; exit 1; }

echo
echo "All changes committed and pushed with message:"
echo "$COMMIT_MSG"
