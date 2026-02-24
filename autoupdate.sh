set -e

API_ROOT="https://cors.littleyang.com.cn/https://api.github.com"
MAIN_ROOT="https://cors.littleyang.com.cn/https://github.com"
ORG="lyoj-dev"
REPO="lyoj-v2"
BRANCH="main"

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
ORANGE='\033[38;5;208m'
CLEAR='\033[0m'

if [[  ! -f .version ]]; then
    echo -e "{\"type\": \"release\", \"version\": \"\"}" > .version
fi
TYPE=$( cat .version | jq ".type // empty" -r )
VERSION=$( cat .version | jq ".version // empty" -r )
URL=""

if [[ $TYPE != "dev" && $TYPE != "release" ]]; then
    TYPE="release"
fi

install_package() {
    echo -e $YELLOW"Installing $1..."$CLEAR
    if command -v apt > /dev/null 2>&1; then
        sudo apt install $2 -y
    elif command -v yum > /dev/null 2>&1; then
        sudo yum install $3 -y
    else
        echo -e $RED"Unsupported operating system. Please install $1 manually."$CLEAR
        exit 1
    fi
}

if command -v curl > /dev/null 2>&1; then
    echo -e $GREEN"curl is already installed."$CLEAR
else
    install_package curl curl curl
fi

if command -v unzip > /dev/null 2>&1; then
    echo -e $GREEN"unzip is already installed."$CLEAR
else
    install_package unzip unzip unzip
fi

if command -v jq > /dev/null 2>&1; then
    echo -e $GREEN"jq is already installed."$CLEAR
else
    install_package jq jq jq
fi

if [[ $TYPE == "dev" ]]; then
    DATA=$( curl -s "$API_ROOT/repos/$ORG/$REPO/branches/$BRANCH" )
    COMMIT=$( echo "$DATA" | jq ".commit.sha // empty" -r )
    if [[ $COMMIT == "$VERSION" || $COMMIT == "" ]]; then
        # No update needed
        exit 0
    fi
    echo -e "Found new version: $BRANCH@$YELLOW$COMMIT$CLEAR！"
    echo ""

    if [[ $VERSION != "" ]]; then 
        # Update
        DETAILS=""
        COMPARE=$( curl -s "$API_ROOT/repos/$ORG/$REPO/compare/$VERSION...$COMMIT" )
        echo "Logs: "
        DETAILS+="Logs: \n"
        MESSAGE=$( echo "$COMPARE" | jq ".commits | reverse | [.[].commit.message]" )
        SHA=($( echo "$COMPARE" | jq -r ".commits | reverse | .[].sha | @sh" ))
        COUNT=$( echo "$MESSAGE" | jq -r "length" )
        for (( i=0; i < $COUNT; i++ )); do
            SHA[$i]=$( echo ${SHA[$i]} | tr -d "'" )
            DETAILS+="$YELLOW"${SHA[$i]:0:6}"$CLEAR - "$( echo $MESSAGE | jq -r ".[$i]" )"\n"
        done
        echo -e $COUNT" commit(s) found."
        echo "";
        echo "Diff: "
        DETAILS+=$COUNT" commit(s) found.\n\nDiff: \n"
        ADDITIONS=($( echo $COMPARE | jq -r '.files[].additions | @sh' ))
        DELETIONS=($( echo $COMPARE | jq -r '.files[].deletions | @sh' ))
        FILENAME=($( echo $COMPARE | jq -r '.files[].filename | @sh' ))
        STATUS=($( echo $COMPARE | jq -r '.files[].status | @sh' ))
        ADDITION=$(( 0 ))
        DELETION=$(( 0 ))
        COUNT=$( echo "$COMPARE" | jq -r ".files | length" )
        for (( i=0; i < $COUNT; i++ )); do
            ADDITION=$(( $ADDITION + ${ADDITIONS[$i]} ))
            DELETION=$(( $DELETION + ${DELETIONS[$i]} ))
            STATUS[$i]=$( echo ${STATUS[$i]} | tr -d "'" )
            FILENAME[$i]=$( echo ${FILENAME[$i]} | tr -d "'" )
            if [[ ${ADDITIONS[$i]} != 0 ]]; then
                DETAILS+="$GREEN+$CLEAR "${ADDITIONS[$i]}"\t"
            else
                DETAILS+="  \t"
            fi
            if [[ ${DELETIONS[$i]} != 0 ]]; then
                DETAILS+="$RED-$CLEAR "${DELETIONS[$i]}"\t"
            else
                DETAILS+="  \t"
            fi
            DETAILS+="| "
            if [[ ${STATUS[$i]} == "added" ]]; then
                DETAILS+=$GREEN"A"$CLEAR
            elif [[ ${STATUS[$i]} == "removed" ]]; then
                DETAILS+=$RED"D"$CLEAR
            elif [[ ${STATUS[$i]} == "modified" ]]; then
                DETAILS+=$YELLOW"M"$CLEAR
            elif [[ ${STATUS[$i]} == "renamed" ]]; then
                DETAILS+=$ORANGE"R"$CLEAR
            fi
            DETAILS+=" "${FILENAME[$i]}"\n"
        done
        echo -e $COUNT" file(s) changed, "$ADDITION" additions(+), "$DELETION" deletions(-)"
        DETAILS+=$COUNT" file(s) changed, "$ADDITION" additions(+), "$DELETION" deletions(-)\n"
        echo "";
        echo -e "Press \`Enter\` to view details, or input \`q\` to skip."
        read -n 1 -s -r
        if [[ $REPLY == "q" ]]; then
            echo ""
        else
            echo ""
            echo -e $DETAILS
        fi

        echo -en "Do you want to update to this version? [Y/n]"
    else
        # Install
        echo -en "Do you want to install this version? [Y/n]"
    fi

    while (( 1 )); do
        read -n 1 -s -r
        echo $REPLY
        if [[ $REPLY == "Y" || $REPLY == "y" || $REPLY == "" ]]; then
            echo ""
            break
        elif [[ $REPLY == "N" || $REPLY == "n" ]]; then
            echo ""
            exit 0
        else
            echo "";
            echo -en "Invalid input, please input \`Y\` or \`N\`."
        fi
    done

    URL="$MAIN_ROOT/$ORG/$REPO/archive/$COMMIT.zip"
    VERSION=$COMMIT
elif [[ $TYPE == "release" ]]; then
    DATA=$( curl -s "$API_ROOT/repos/$ORG/$REPO/releases/latest" )
    TAG=$( echo "$DATA" | jq ".tag_name // empty" -r )

    if [[ $TAG == "$VERSION" || $TAG == "" ]]; then
        # No update needed
        exit 0
    fi
    echo -e "Found new version: $YELLOW$TAG$CLEAR！"
    echo ""

    if [[ $VERSION != "" ]]; then 
        # Update
        echo "Logs: "
        echo $DATA | jq -r ".body"
        echo ""
        echo -en "Do you want to update to this version? [Y/n]"
    else
        # Install
        echo -en "Do you want to install this version? [Y/n]"
    fi

    while (( 1 )); do
        read -n 1 -s -r
        if [[ $REPLY == "Y" || $REPLY == "y" || $REPLY == "" ]]; then
            echo ""
            break
        elif [[ $REPLY == "N" || $REPLY == "n" ]]; then
            echo ""
            exit 0
        else
            echo "";
            echo -en "Invalid input, please input \`Y\` or \`N\`."
        fi
    done

    URL=$( echo $DATA | jq -r ".zipball_url" )
    VERSION=$TAG
fi

SUM=$(date | sha256sum | cut -d " " -f 1)
TMP="/tmp/"${SUM:0:8}
mkdir -p $TMP
ZIP="$TMP/raw.zip"
curl $URL -kL -o $TMP/raw.zip
unzip $ZIP -d $TMP > /dev/null
rm -rf $TMP/raw.zip
DIR=$( ls $TMP )
mv $TMP/$DIR/* $TMP/
rm -rf $TMP/$DIR

# =============================================
#
# 自定义安装过程
# 文件在 $TMP 目录下，版本号为 $VERSION
#
# =============================================

if command -v g++ > /dev/null 2>&1; then
    echo -e $GREEN"g++ is already installed."$CLEAR
else
    install_package g++ g++ gcc-c++
fi

if command -v npm > /dev/null 2>&1; then
    echo -e $GREEN"node is already installed."$CLEAR
else
    echo -e $RED"Node.js is not installed. Please install Node.js manually."$CLEAR
    exit 1
fi

install_package openssl openssl openssl-devel
install_package jsoncpp libjsoncpp-dev jsoncpp-devel
install_package mysqlclient libmysqlclient-dev mysql-devel

echo -e $YELLOW"Building..."$CLEAR
g++ $TMP/backend/main.cpp -o./backend -lssl -lcrypto -ljsoncpp -lmysqlclient -O3 -Wno-unused-result -Wno-deprecated-declarations
g++ $TMP/judge/judge.cpp -o./judge -ljsoncpp -lmysqlclient -O3 -Wno-unused-result -Wno-deprecated-declarations
g++ $TMP/judge/unit.cpp -o./unit -ljsoncpp -O3 -Wno-unused-result -Wno-deprecated-declarations
mkdir -p spj
g++ $TMP/spj/1.cpp -o./spj/1 -O3

cp $TMP/data ./ -r
cp $TMP/config.json ./
cp $TMP/judge.json ./
cp $TMP/frontend ./ -r
cd frontend
npm install && npm run build-only
cd ..

mkdir -p tmp
sudo chmod 0777 tmp -R

if id "judge" &>/dev/null; then
    echo -e $GREEN"User \`judge\` already exists."$CLEAR
else
    sudo adduser judge
    sudo passwd -d judge # Delete judge's password
fi

echo -e $GREEN"Completed!"$CLEAR

# =============================================
#
# 安装完成，更新版本号
# 请不要改动下面的代码
#
# =============================================

echo -e "{\"type\": \"$TYPE\", \"version\": \"$VERSION\"}" > .version
rm -rf $TMP