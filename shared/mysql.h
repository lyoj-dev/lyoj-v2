#include<mysql/mysql.h>

const set<int> retryedErrno = { 2013 };

/**
 * @brief 连接数据库
 * @param host 数据库地址
 * @param user 数据库用户
 * @param passwd 数据库密码
 * @param db 数据库名
 * @param port 数据库端口
 * @return MySQL 连接符
 */
MYSQL mysqli_connect(string host, string user, string passwd, string db, int port) {
    MYSQL mysql, *res1; 
    res1 = mysql_init(&mysql); 
    if (res1 == NULL) writeLog(LOG_LEVEL_ERROR, "Failed to connect to database: %s(Errno: %d)", mysql_error(&mysql), mysql_errno(&mysql));
	bool res2 = mysql_real_connect(&mysql, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, nullptr, 0);
    if (!res2) {
        if (retryedErrno.count(mysql_errno(&mysql))) return mysqli_connect(host, user, passwd, db, port);
        else writeLog(LOG_LEVEL_ERROR, "Failed to connect to database: %s(Errno: %d)", mysql_error(&mysql), mysql_errno(&mysql));
    }
	return mysql;
}

/**
 * @brief 查询 SQL 语句
 * @param conn MySQL 连接符
 * @param sql SQL 语句
 * @return 查询结果
 */
vector<map<string, string> > mysqli_query(MYSQL &conn, string sql) {
    writeLog(LOG_LEVEL_DEBUG, "Querying SQL sentence: %s", sql.c_str());
    vector<map<string, string> > res; 
    bool res1 = mysql_query(&conn, sql.c_str());
    if (res1) {
        if (retryedErrno.count(mysql_errno(&conn))) return mysqli_query(conn, sql);
        else writeLog(LOG_LEVEL_ERROR, "Failed to query SQL sentence: %s(Errno: %d)", mysql_error(&conn), mysql_errno(&conn));
    }
    MYSQL_RES *res2 = mysql_store_result(&conn);
    if (!res2) writeLog(LOG_LEVEL_ERROR, "Failed to query SQL sentence: %s(Errno: %d)", mysql_error(&conn), mysql_errno(&conn));
	vector<string> field; MYSQL_FIELD *fd; MYSQL_ROW row;
    while(fd = mysql_fetch_field(res2)) field.push_back(fd->name);
	while (row = mysql_fetch_row(res2)) {
        map<string, string> tmp;
		for (int i = 0; i < field.size(); i++) tmp[field[i]] = row[i] == NULL ? "" : row[i];
		res.push_back(tmp);
	} mysql_free_result(res2);
	return res;
}

/**
 * @brief 查询 SQL 语句
 * @param conn MySQL 连接符
 * @param format SQL 格式
 * @return 查询结果
 */
vector<map<string, string> > mysqli_query(MYSQL &conn, const char* format, ...) {
    // 格式化文本
    const int size = 2 * 1024 * 1024;
    va_list args;
    va_start(args, format);
    char buf[size];
    memset(buf, '\0', sizeof buf);
    int n = vsnprintf(buf, size, format, args);
    va_end(args);
    if (n >= size) writeLog(LOG_LEVEL_WARNING, "Buffer size may be not enough! Real size: %d", n);
    return mysqli_query(conn, string(buf));
}

/**
 * @brief 执行 SQL 语句
 * @param conn MySQL 连接符
 * @param sql SQL 语句
 */
void mysqli_execute(MYSQL &conn, string sql) {
    writeLog(LOG_LEVEL_DEBUG, "Executing SQL sentence: %s", sql.c_str());
	if (mysql_query(&conn, sql.c_str())) {
        if (retryedErrno.count(mysql_errno(&conn))) mysqli_execute(conn, sql);
        else writeLog(LOG_LEVEL_ERROR, "Failed to execute SQL sentence: %s(Errno: %d)", mysql_error(&conn), mysql_errno(&conn));
    }
}

/**
 * @brief 执行 SQL 语句
 * @param conn MySQL 连接符
 * @param format SQL 格式
 */
void mysqli_execute(MYSQL &conn, const char* format, ...) {
    // 格式化文本
    const int size = 2 * 1024 * 1024;
    va_list args;
    va_start(args, format);
    char buf[size];
    memset(buf, '\0', sizeof buf);
    int n = vsnprintf(buf, size, format, args);
    va_end(args);
    if (n >= size) writeLog(LOG_LEVEL_WARNING, "Buffer size may be not enough! Real size: %d", n);
    mysqli_execute(conn, string(buf));
}

void mysqli_close(MYSQL &conn) {
    mysql_close(&conn);
}