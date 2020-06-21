#!/bin/bash

#########################################
#
#  Configurable part.
#  This is usually not required, but you can change something in this section.
#
#########################################

NET_NAME="net.ton.dev"
#NET_NAME="main.ton.dev"
LOG_DIR="/var/log/ftn"

# The following variable can be used to ingnore a direcctory names. 
ignore_list='docker'


#########################################
#
#  Please don't change anything below without a good reason
#
#########################################
FTN_WRP=~/ftn
FTN_LIB=/usr/local/lib
FTN_BIN=/usr/local/bin
FTN_REPO="https://github.com/user529/ftn"

# Clone and update git repository of ftn
git clone --recursive "${FTN_REPO}" "${FTN_WRP}"
cd "${FTN_WRP}" && git checkout "master"

# Substitude pattern to used FTN_LIB dir
replace=$(echo "${FTN_LIB}" | sed -e 's/[]$.*\/[\^]/\\&/g')
sed -i "s/=FTN=LIB=/${replace}/" "ftn"
sed -i "s/=FTN=LIB=/${replace}/" "ftn_functions.inc"

# Auto finding directories and files
TON_DIR=$(find / -type d -name $NET_NAME -print -quit 2>/dev/null | grep -vE $ignore_list)
if [ -z "TON_DIR" ]; then
    echo "ERROR: Can't find FreeTON instalation."
    exit 1
fi

. $TON_DIR/scripts/env.sh
ADDR_FILE="$(hostname -s).addr"
TON_MSIGACC=$(cat "${KEYS_DIR}/${ADDR_FILE}")

# Composing a file with environment variables
bash -c "cat <<EOLONGFILE > $FTN_WRP/ftn_cfg.inc
FTN_ACCOUNT='$TON_MSIGACC'
FTN_LOG="$LOG_DIR"
FTN_KEYS_DIR="$KEYS_DIR"
FTN_DIR="$TON_DIR"
TONOS_CLI_DIR="$TON_DIR/tonos-cli/target/release"
TONOS_CLI="$TON_DIR/tonos-cli/target/release/tonos-cli"
TON_CFG_DIR="$TON_DIR/configs"
EOLONGFILE"

# Copy file to local lib and bin directories
sudo chmod 755 ${FTN_WRP}/ftn
sudo chmod 644 ${FTN_WRP}/*.inc
sudo cp -s ${FTN_WRP}/ftn ${FTN_BIN}
sudo cp -s ${FTN_WRP}/*.inc ${FTN_LIB}

# Create log dir
sudo mkdir -p $LOG_DIR
sudo chmod 755 $LOG_DIR