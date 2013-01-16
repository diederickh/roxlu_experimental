#!/bin/bash
# Compiling liblampmp3
# --------------------
# Download the source: 
# curl -L "http://downloads.sourceforge.net/project/lame/lame/3.99/lame-3.99.5.tar.gz" -o lame.tar.gz

d=${PWD}
bd=${d}/build/
sd=${d}/

lame_src="lame.tar.gz"

if [ ! -f ${lame_src} ] ; then 
    echo "Download lame 3.99.5.tar.gz into this folder"
    exit
fi

function extract_targz {
    v=${1}
    name=${2}
    out_dir=${3}
    is_bunzip=${4}
    if [ -d ${out_dir} ] ; then 
        echo "Already exist: ${out_dir}"
        return
    fi

    sf=${sd}/${v}

    if [ ! -f ${sf} ] ; then 
        echo "File: ${sf} not found."
        exit
    fi

    cp ${sf} .
    if [ ${is_bunzip} = 1 ] ; then
        tar -jxvf ${v}
    else 
        tar -zxvf ${v}
    fi


    extracted_dir=$(find . -type d -maxdepth 1 -name "${name}")
    if [ "${extracted_dir}" != "" ] ; then 
        dirname=${extracted_dir#"./"}
        mv ${dirname} ${out_dir}
    else 
        echo "Cannot find extract dir for: ${sf}"
        exit
    fi
    
    rm ${v}

}

extract_targz ${lame_src} "lame*" "lame" 0

cd lame
./configure --prefix=${bd} --enable-static
make 
make install
