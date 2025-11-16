<p align="center"><img src="https://cdn.jsdelivr.net/gh/LittleYang0531/image/lyoj/logo.png"></p>

<p align="center">A multi-platform online judge system. Built with Vue and C++.</p>

------

# LittleYang OnlineJudge (LYOJ) v2

## Build

### Install Dependencies

```bash
sudo apt install g++ libjsoncpp-dev libmysqlclient-dev openssl
```

### Compile Judge Service

```bash
cd judge
g++ unit.cpp -ounit -ljsoncpp -O3 -w
g++ judge.cpp -ojudge -ljsoncpp -lmysqlclient -O3 -w
```

### Compile Backend Service

```bash
cd backend
g++ main.cpp -omain -ljsoncpp -lmysqlclient -lcrypto -lssl -O3 -w
```

### Build Frontend Service

Please modify `frontend/src/config.ts` before build.

```bash
cd frontend
npm install
npm run build
```

## Run

### Judge Service

```bash
cd judge && sudo ./judge
```

### Backend Service

```bash
cd backend && sudo ./main
```

## Specs

For any information about special judge and problem configuration file, see [specs.md](specs.md).

## APIs

See [apis.md](apis.md)