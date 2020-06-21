
. =FTN=LIB=/ftn_cfg.inc
shopt -s extglob
func_list="@(convert|electionlog|electionrun|setstake|adnl|balance|transactions|submit|recover|getconfig|config|exec|get_mid_stake2|help)"
ftn_version="0.1 (alpha)"

function version {
    echo "ftn $ftn_version"
}

function tonos {
    date
    echo "ftn_function.tonos: $@"
    cd "${FTN_DIR}/scripts"
    ${TONOS_CLI_DIR}/tonos-cli $@
    cd $OLD_PWD
}

function getconfig {
    tonos $@ 2>&1 | tee $FTN_LOG/exec.log
}

function config {
    tonos $@ 2>&1 | tee $FTN_LOG/exec.log
}

function convert {
    #    truncate -s 0 "${FTN_LOG}/convert.log"
    cd "${FTN_DIR}/scripts"
    n=$(${TONOS_CLI_DIR}/tonos-cli $1 tokens $2)
    cd $OLD_PWD
    echo $((10#$n)) # 10#$n - used to remove leading zeros
}

function electionlog {
    if [ -z "$2" ]; then
        lines=14
    else
        if [ "$2" == "full" ]; then
            tail -n 30 "${FTN_LOG}/election.log"
            exit
        else
            lines=$2
        fi
    fi
    tail -n $lines "${FTN_LOG}/election.log" | grep "INFO:"
}

function electionrun {
    if [ -z $2 ]; then
        STAKE=$(cat "${KEYS_DIR}/elections/stake" 2>/dev/null || STAKE='')
    else
        STAKE=$2
    fi

    if [ -z "$STAKE" ]; then
        echo "<STAKE> is not set. Please use \"ftn setstake\" or \"ftn electionrun <STAKE>\""
    fi

    #truncate -s 0 "${FTN_LOG}/election.log"
    cd "${FTN_DIR}/scripts"
    ./validator_msig.sh $STAKE | tee -a "${FTN_LOG}/election.log" 2>&1
    cd "${OLDPWD}"
    tail -n 14 "${FTN_LOG}/election.log"
}

#function get_mid_stake2 {
#    result="$($TONOS_CLI getconfig 34)"
#    #    TOTAL_NODES=$(echo $result | grep \"total\" | awk -F, '{print $1}' | awk -F: '{print $2}')
#    TOTAL_NODES=$(echo "$result" | perl -ne '/^\s*\"total\" : (\d+).*$/ && print $1')
#    #echo $result | grep \"total\" 
#    echo $TOTAL_NODES
#}

function setstake { 
    . "${FTN_DIR}/scripts/env.sh"

#    function get_mid_stake {
#        ELECTOR_ADDR="$(cat "$KEYS_DIR/elections/elector-addr-base64")"
#            LC_OUTPUT="$($TON_BUILD_DIR/lite-client/lite-client \
#            -a 127.0.0.1:3031 \
#            -p $KEYS_DIR/liteserver.pub \
#            -rc "getconfig 34" \
#        -rc "$(printf "runmethod %s past_elections" "$ELECTOR_ADDR")" \
#        -rc "quit" 2>&1)"
#    #   echo $LC_OUTPUT
#        TOTAL_NODES=$(echo "$LC_OUTPUT" | perl -ne '/^\s*cur_validators:.*total:(\d+).*$/ && print $1')
#        TOTAL_STAKE=$(echo "$LC_OUTPUT" | \
#            perl -ne '/^result:\s*\[\s*\((?>\[\d+ \d+ \d+ \d+ [\{\}\(\)A-F0-9]+ (\d+) \d+ [\{\}\(\)A-F0-9]+\]\s*)*\)\s*\]\s*$/ && print $1')
#        TOTAL_STAKE=$((TOTAL_STAKE/1000000000))
#        MID_STAKE=$((TOTAL_STAKE/TOTAL_NODES))
#        echo "$TOTAL_STAKE/$TOTAL_NODES=$MID_STAKE"
#    }

    #get_mid_stake
    bal=$(ftn balance | grep "balance" | awk '{print $2}')
    #exit
    echo "Got acc balance $bal"
    echo "Got mid stake $MID_STAKE"
    #    stake=$((bal-5))

    if [ $# -ne 0 ]; then
        echo "Got stake size $1"
        stake=$1
    else
        stake=$MID_STAKE
    fi
#    MAX_STAKE_SIZE=30000
    #    if [[ $stake -gt $MAX_STAKE_SIZE ]]; then
    #       echo "More than max_stake. stake set as the max stake"
    #       stake=$MAX_STAKE_SIZE
    #    fi

    if [[ $stake -gt $bal ]]; then
        echo "More than balance. stake set as bal-1"
        stake=$((bal-1))
    fi
    #    stake=$((bal/2))
    if [[ $stake -lt 10000 ]]; then
        echo "Less than minimal stake. Set 0 stake"
        $stake=0
    fi
    echo $stake > "${KEYS_DIR}/elections/stake"
    echo "${stake} set as stake."

}
function adnl { 
    adnlfile="$FTN_KEYS_DIR/elections/$(hostname -s)-election-adnl-key"
    keyfile="$FTN_KEYS_DIR/elections/$(hostname -s)-election-key"
    if [ -z "${adnlfile}" ]; then
        echo "ADNL adress not found."
    else
            adnl=$(cat "${adnlfile}" | grep "created new key")
            pubkey=$(cat "${keyfile}" | grep "created new key")
            echo -e "adnl: ${adnl:16}\npubkey: ${pubkey:16}"
    fi    
}


function balance { 
    truncate -s 0 "${FTN_LOG}/balance.log"
    if [ -z "${2}" ]; then
      Acc="${FTN_ACCOUNT}"
    else
      Acc="${2}"
    fi
    # "${TONOS_CLI}" account "${Acc}" > "${FTN_LOG}/balance.log" 2>&1 
    #    tonos account "${Acc}" 2>&1 | tee ${FTN_LOG}/balance.log
    tonos account "${Acc}" > ${FTN_LOG}/balance.log
    acc="$(cat "${FTN_LOG}/balance.log" | grep "address:" | awk -F':' '{printf ("address %s:%s", $2, $3)}')"
    bal="$(cat "${FTN_LOG}/balance.log" | grep "balance:" | awk -F':' '{printf ("balance %d", $2/1000000000)}')"
    echo -e "$acc\n$bal"
}

function transactions { 
    #    truncate -s 0 $FTN_LOG/transactions.log
    tonos run "${FTN_ACCOUNT}" getTransactions {} --abi "${TON_CFG_DIR}/SafeMultisigWallet.abi.json" 2>&1 | tee $FTN_LOG/transactions.log
}

function submit { 
    #    truncate -s 0 "${FTN_LOG}/submit.log"
    NANOTKNS=$(ftn convert $3)
    echo $NANOTKNS
    if [ "${4}" = "false" ]; then
      isBounce=false
    else
      isBounce=true
    fi
    #set -x
    #    "${TONOS_CLI}" call "${FT_ACCOUNT}" submitTransaction "{\"dest\":\"${1}\",\"value\":${nt},\"bounce\":"${isBounce}",\"allBalance\":false,\"payload\":\"\"}" --abi "${TON_CFG_DIR}/SafeMultisigWallet.abi.json" --sign "${FTN_SEEDPHRASE}" > "${FTN_LOG}/submit.log" 2>&1 
    tonos call "${FTN_ACCOUNT}" submitTransaction "{\"dest\":\"${2}\",\"value\":${NANOTKNS},\"bounce\":"${isBounce}",\"allBalance\":false,\"payload\":\"\"}" --abi "${TON_CFG_DIR}/SafeMultisigWallet.abi.json" --sign "${FTN_KEYS_DIR}/msig.keys.json" 2>&1 | tee ${FTN_LOG}/submit.log 
    #set +x
    #    cat "${FTN_LOG}/submit.log"
}

function recover { 
    cd "${FTN_DIR}/scripts"
    . "${FTN_DIR}/scripts/env.sh"
    SEND_ATTEMPTS=1
    #set -x

    elector_addr=$(cat "${FTN_KEYS_DIR}/elections/elector-addr-base64")

    "${TON_BUILD_DIR}/lite-client/lite-client" \
        -p "${FTN_KEYS_DIR}/liteserver.pub" -a 127.0.0.1:3031 \
        -rc "runmethod ${elector_addr} compute_returned_stake 0x$(echo "${FTN_ACCOUNT}" | cut -d ':' -f 2)" \
        -rc "quit" &>"${FTN_KEYS_DIR}/elections/recover-state"

    awk '{
        if ($1 == "result:") {
            print $3
        }
    }' "${FTN_KEYS_DIR}/elections/recover-state" >"${FTN_KEYS_DIR}/elections/recover-amount"

    recover_amount=$(cat "${FTN_KEYS_DIR}/elections/recover-amount")
    echo "INFO: recover_amount = ${recover_amount} nanotokens"

    if [ "$recover_amount" != "0" ]; then
        "${TON_BUILD_DIR}/crypto/fift" -I "${TON_SRC_DIR}/crypto/fift/lib:${TON_SRC_DIR}/crypto/smartcont" -s recover-stake.fif "${FTN_KEYS_DIR}/elections/recover-query.boc"

        recover_query_boc=$(base64 --wrap=0 "${FTN_KEYS_DIR}/elections/recover-query.boc")

        for i in $(seq ${SEND_ATTEMPTS}); do
            echo "INFO: tonos-cli submitTransaction attempt #${i}..."
            set -x
            if ! "${UTILS_DIR}/tonos-cli" call "${FTN_ACCOUNT}" submitTransaction \
                "{\"dest\":\"${elector_addr}\",\"value\":\"1000000000\",\"bounce\":true,\"allBalance\":false,\"payload\":\"${recover_query_boc}\"}" \
                --abi "${CONFIGS_DIR}/SafeMultisigWallet.abi.json" \
                --sign "${FTN_KEYS_DIR}/msig.keys.json"; then
                echo "INFO: tonos-cli submitTransaction attempt #${i}... FAIL"
            else
                echo "INFO: tonos-cli submitTransaction attempt #${i}... PASS"
                break
            fi
            set +x
        done

        date +"INFO: %F %T Recover of $recover_amount GR requested"
        exit 0
    else
        echo "INFO: nothing to recover"
    fi
}

function exec {  
    tonos "${@:2}" 2>&1 | tee $FTN_LOG/exec.log
    #    "${TONOS_CLI} $@" >> $FTN_LOG/exec.log
}


function help {
    . =FTN=LIB=/ftn_help.inc
    if [ -n "$2" ]; then
        section=$2
    else
        section="overview"
    fi

    echo "${_help[$section]}"
}
