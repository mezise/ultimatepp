const char *install_script =
R"--(#!/usr/bin/env bash

AskContinue()
{
  read -p "Continue (Y/n)?" answer
  if [ "$answer" != "${answer//[nN]/x}" ]; then
	 exit;
  fi
}

uname=`uname`

if [ -x "$(command -v apt-get)" ]; then
  DEP="apt-get install g++ clang make libgtk-3-dev libnotify-dev libbz2-dev libssl-dev xdotool"
elif [ -x "$(command -v yum)" ]; then
  DEP="yum install gcc-c++ clang make gtk3-devel libnotify-devel bzip2-devel freetype-devel openssl-devel"
elif [ -x "$(command -v dnf)" ]; then
  DEP="dnf install gcc-c++ clang make gtk3-devel libnotify-devel bzip2-devel freetype-devel openssl-devel"
elif [ -x "$(command -v urpmi)" ]; then
  DEP="urpmi install gcc-c++ clang make gtk3-devel libnotify-devel bzip2-devel freetype-devel openssl-devel"
elif [ -x "$(command -v zypper)" ]; then
  DEP="zypper install gcc-c++ clang make gtk3-devel libnotify-devel bzip2-devel freetype-devel libopenssl-devel"
elif [ -x "$(command -v pacman)" ]; then
  DEP="pacman -Sy --needed gcc make zlib bzip2 gtk3 libnotify openssl clang pkgconfig gdb"
elif [ -x "$(command -v pkg)" ]; then
  DEP="pkg install bash gmake gtk3 libnotify llvm90 pkgconf"
  if [[ "$uname" == 'SunOS' ]]; then
     DEP="pkg install bash gtk3 libnotify developer/clang-80 build-essential"
  fi
elif [ -x "$(command -v pkg_add)" ]; then
  DEP="pkg_add bash gmake gtk3 libnotify clang-devel"
fi

if [[ "$uname" == 'OpenBSD' ]]; then
  DEP=""
fi

if [ -z "$DEP" ]; then
  if [[ "$uname" == 'OpenBSD' ]]; then
    echo Automatic dependecies installation is not supported on OpenBSD.
    echo See README for details.
  else
    echo Packaging system was not identified.
    echo Automatic dependency instalation has failed.
    echo You will have to install required packages manually.
  fi
  echo Please make sure that build dependecies are satisfied.
  AskContinue
else
  echo Following command should be used to install required packages:
  echo
  echo sudo $DEP
  echo
  echo Install script can run this command for you, but that will require
  echo your sudo password.
  read -p "Do you want the script to do that (Y/n)?" answer
  if [ "$answer" == "${answer//[nN]/x}" ]; then
     if ! eval 'sudo $DEP'; then
        echo Failed to install all required packages.
        echo You will have to install required packages manually.
        AskContinue
     fi
  else
     echo Please make sure that build dependecies are satisfied.
     AskContinue
  fi
fi

if ./umks32 >/dev/null; then
  echo
  read -p "Use prebuilt binary umks32 to accelerate the build (Y/n)?" answer
  if [ "$answer" == "${answer//[nN]/x}" ]; then
    echo Using umks32 to build
    UMK="./umks32"
  fi
fi

if [ -z "$UMK" ]; then
  echo umks32 cannot be used, building umk using make
  ./configure
  make -f umkMakefile -j 4
  UMK="./umk"
fi

if clang++ 2>/dev/null; then
	$UMK ./uppsrc ide CLANG -brs ./theide
	$UMK ./uppsrc umk CLANG -brs ./umk
else
	$UMK ./uppsrc ide GCC -brs ./theide
	$UMK ./uppsrc umk GCC -brs ./umk
fi

if [ -x ./theide ]; then
  echo Install process has been finished, TheIDE is built as ./theide
  read -p "Do you want to start TheIDE now? (Y/n):" answer
  if [ "$answer" == "${answer//[nN]/x}" ]; then
     ./theide
  fi
else
  echo Something went wrong.
  echo Please use 'make' to compile theide and/or notify developers.
fi

)--";

const char *clean_script =
R"--(#!/usr/bin/env bash
#!/usr/bin/env bash

rm -rf .config/*
rm -rf .cache/*
rm -f theide
rm -f umk
)--";
