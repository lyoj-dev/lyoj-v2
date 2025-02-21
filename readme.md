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

## Run

### Judge Service

```bash
sudo ./judge
```

**为什么要用 `sudo`？**

`judge` 的子程序 `unit` 会以新的用户 `judge["runas"]` 身份运行用户程序，以及在 `kill` 用户程序时也需要用到 `sudo` 超级管理员权限。

### Backend Service

```bash
sudo ./main
```

**为什么要用 `sudo`？**

`main` 需要通过 `judge["sock"]` 套接字与 `judge` 进行信息通信。`judge` 在超级管理员权限下创建的套接字文件同样需要在超级管理员权限下访问。

## APIs

待补充...