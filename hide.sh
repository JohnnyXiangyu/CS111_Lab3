git filter-branch --force --index-filter \
  'git rm --cached --ignore-unmatch A/README' \
  --prune-empty --tag-name-filter cat -- --all

git filter-branch --force --index-filter \
  'git rm --cached --ignore-unmatch B/README' \
  --prune-empty --tag-name-filter cat -- --all
