#! /bin/sh

sub_string()
{
    src_str=$1
    sub_str=$2

    if [ -z `echo ${src_str} | grep ${sub_str}` ]; then
        return 0
    else
        return 1
    fi
}

cl()
{
    # $1 -- 源目录:$2 -- 镜像目录
    echo "[INFO] new loop"
    printf "\tsrc: $1\n"
    printf "\tmirror: $2\n"

    # 检测镜像目录是否已经存在
    if [ -e $2 ]; then
        echo "[ERROR] mirror directory already existed."
        return 255
    fi

    # 创建镜像目录
    mkdir $2
    printf "\tnew mirror dir created.\n"

    for file in $1/*; do
        if [ -f ${file} ]; then
            ln -sv $file $2
        fi
        if [ -d $file ]; then
            cl $file $2/`basename ${file}`
        fi
    done

    return 0
}

main()
{
    src=$1
    dest=$2

    if   [ -z $2 ]; then
        printf "usage: cl.sh source destination\n"
    else # 至少有2个参数
        # 判断2个目录是否存在
        if   [ ! -x ${src} ]; then
            printf 'source directory does NOT exist or '
            printf 'you do NOT have permission.\n'
            exit -1
        elif [ ! -x $dest ]; then
            printf 'destination directory does NOT exist or '
            printf 'you do NOT have permission.\n'
            exit -1
        fi

        # 获得绝对路径
        which realpath
        if   [ 0 != $? ]; then
            echo "[ERROR] seem that you don't have realpath installed."
            exit -1
        fi
        src=`realpath ${src}`
        printf "source: "${src}"\n"
        dest=`realpath ${dest}`
        printf "destination: "${dest}"\n"

        # 不允许目的目录是源目录的子目录
        sub_string ${dest} ${src}
        if [ 1 = $? ]; then
            printf "[ERROR] the mirror directory can NOT be created in "
            printf "the source one's dir-tree.\n"
            exit -1
        fi

        mirror=${dest}/`basename ${src}`
        printf "mirror: "$mirror"\n"

        # 检测目标目录写权限
        if   [ ! -w ${dest} ]; then
            exit -1
        fi

        # 获取目标目录git分支信息
        cd ${src}
        git_branch=`git branch`
        if [ $? -ne 0 ]; then
            printf "[ERROR] source directory is NOT a git repository!\n"
            exit -1
        fi
        cd -

        # 递归创建软链接
        cl ${src} ${mirror}

        # 打印分支信息\
        if [ 0 = $? ]; then
            echo
            echo "git branch for mirror:"
            echo "${git_branch}"
            echo
        fi
    fi

    exit $?
}

main $1 $2
exit $?
