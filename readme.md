# LittleYang OnlineJudge v2

## Build

### Preprocess

```bash
sudo apt install g++ libjsoncpp-dev libmysqlclient-dev openssl
```

### Judge Service

```bash
cd judge
g++ unit.cpp -ounit -ljsoncpp -O3 -w
g++ judge.cpp -ojudge -ljsoncpp -lmysqlclient -O3 -w
```

### Backend Service

```bash
cd backend
g++ main.cpp -omain -ljsoncpp -lmysqlclient -lcrypto -lssl -O3 -w
```

### Frontend Service

```bash
cd frontend
npm install
npm run build
```

## APIs

待补充...