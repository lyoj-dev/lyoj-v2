auto ContestsSubmissionsDetails = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    int identity = getContestIdentity(userId, atoi(argv[0].c_str()));
    auto contest = mysqli_query(
        mysql,
        "SELECT uid FROM contest WHERE id = %d",
        atoi(argv[0].c_str())
    );
    if (contest.size() == 0) quickSendMsg(404);
    auto res = mysqli_query(
        mysql, 
        "SELECT * FROM contest_signup WHERE id = %d AND uid = %d", 
        atoi(argv[0].c_str()),
        userId
    );
    if (res.size() == 0 && identity != Administrator) quickSendMsg(403);
    mysqli_close(mysql);

    swap(argv[0], argv[1]);
    SubmissionsDetails(conn, request, argv);
};