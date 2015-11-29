#!/bin/sh
set -e -x

die() {
  echo "$1" >&2
  exit 1
}

ORIGDIR="$PWD"

[ "$PROF" ] || die "No PROF defined"

CACHEKEY=ver1

if [ ! -f "$HOME/.cache/$PROF/$CACHEKEY" ]
then
  rm -rf "$HOME/.cache/$PROF"
  install -d "$HOME/.cache/$PROF"
  cd "$HOME/.cache/$PROF"

  case "$PROF" in
    3.15)
      wget -O base.tar.gz https://github.com/epics-base/epics-base/archive/R3.15.3.tar.gz
    ;;
    3.14)
      wget -O base.tar.gz https://github.com/epics-base/epics-base/archive/R3.14.12.5.tar.gz
    ;;
    *) die "Unsupported profile $PROF";;
  esac

  install -d base

  cd base
  tar --strip-components=1 -xzf ../base.tar.gz
  make -j2
  cd ..

  chmod -R a-w base

  touch "$HOME/.cache/$PROF/$CACHEKEY"
fi

cd "$ORIGDIR"

make -C code-listings EPICS_BASE=$HOME/.cache/$PROF/base MOTOR=
