#!/bin/sh
set -e -x

die() {
  echo "$1" >&2
  exit 1
}

[ "$BASE" ] || die "No BASE defined"

cat <<EOF > code-listings/configure/RELEASE.local
EPICS_BASE=$HOME/.cache/$BASE/base
EOF

CACHEKEY=ver1

if [ ! -f "$HOME/.cache/$BASE/$CACHEKEY" ]
then
  rm -rf "$HOME/.cache/$BASE"
  install -d "$HOME/.cache/$BASE"

  git clone --branch $BASE https://github.com/${REPO:=epics-base}/epics-base.git "$HOME/.cache/$BASE/base"

  make -j2 -C "$HOME/.cache/$BASE/base"

  chmod -R a-w "$HOME/.cache/$BASE/base"

  touch "$HOME/.cache/$BASE/$CACHEKEY"
fi
