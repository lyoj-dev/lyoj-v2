<p align="center"><img src="https://cdn.jsdelivr.net/gh/LittleYang0531/image/lyoj/logo.png"></p>

<p align="center">A multi-platform online judge system. Built with Vue and C++.</p>

------

# LittleYang OnlineJudge (LYOJ) v2

## Build

### One-Click Script

```bash
bash <(curl -s -L https://raw.githubusercontent.com/lyoj-dev/lyoj-v2/refs/heads/main/autoupdate.sh)
```

### Build Manually

#### Install Dependencies

```bash
sudo apt install g++ libjsoncpp-dev libmysqlclient-dev openssl
```

#### Compile Judge Service

```bash
cd judge
g++ unit.cpp -ounit -ljsoncpp -O3 -w
g++ judge.cpp -ojudge -ljsoncpp -lmysqlclient -O3 -w
cd ../spj
g++ 1.cpp -o1 -O3
cd ..
```

#### Compile Backend Service

```bash
cd backend
g++ main.cpp -omain -ljsoncpp -lmysqlclient -lcrypto -lssl -O3 -w
cd ..
```

#### Build Frontend Service

Please modify `frontend/src/config.ts` before build.

```bash
cd frontend
npm install
npm run build
cd ..
```

#### Create `judge` user

```bash
sudo adduser judge
sudo mkdir tmp
sudo chmod 0777 tmp -R
sudo passwd -d judge # Delete judge's password
```

You should set an empty password so that judge service can use `judge` user successfully.

When system reminds you to put password, you should enter `Ctrl + D` to set an empty password.

## Run

### Judge Service

```bash
cd judge && sudo ./judge
```

### Backend Service

```bash
cd backend && ./main
```

## Specs

For any information about special judge and problem configuration file, see [specs.md](specs.md).

## APIs

See [apis.md](apis.md)