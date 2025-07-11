#!/bin/bash

echo "==========================================="
echo "=== Commit and push all local changes ===="
echo "==========================================="
echo

# Obter hashes abreviados dos submodules
NUTTX_HASH=$(cd nuttx && git rev-parse --short HEAD)
APPS_HASH=$(cd apps && git rev-parse --short HEAD)

# Prompt para o usuário adicionar uma frase extra na mensagem de commit
read -p "Add extra commit message (optional): " USER_MSG

# Construir a mensagem de commit base
COMMIT_MSG="Update submodules: nuttx@$NUTTX_HASH apps@$APPS_HASH on $(date '+%Y-%m-%d %H:%M:%S')"

# Se o usuário digitou algo, adiciona à mensagem
if [ -n "$USER_MSG" ]; then
  COMMIT_MSG="$COMMIT_MSG - $USER_MSG"
fi

# Adicionar todas as mudanças, incluindo deleções e submodules
git add -A

# Fazer commit
git commit -m "$COMMIT_MSG"

# Fazer push para o seu repositório remoto
git push origin main

echo
echo "All changes committed and pushed with message:"
echo "$COMMIT_MSG"

