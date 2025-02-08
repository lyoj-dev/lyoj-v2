enum JudgeResult {
    AC = 0,
    WA = 1,
    TLE = 2,
    MLE = 3,
    RE = 4,
    NTD = 5, // 无测试点 No Test Data
    UE = 6,
    PC = 7,
    CE = 8,
    Waiting = 9,
    Compiling = 10,
    Running = 11,
    Skipped = 12,
    Submitted = 13
};

string JudgeResultInfo[] = {
    "Accepted",
    "Wrong Answer",
    "Time Limited Exceeded",
    "Memory Limited Exceeded",
    "Runtime Error",
    "No Test Data",
    "Unknown Error",
    "Partial Correct",
    "Compile Error",
    "Waiting...",
    "Compiling...",
    "Running on case %d...",
    "Skipped",
    "Submitted"
};

enum SubmissionDataType {
    DetailedSubmission = 0,
    UpdateSubmission = 1,
    UpdateSubtask = 2,
    UpdateData = 3
};

enum WebSocketDataType {
    // DetailedSubmission = 0,
    SimpleSubmissions = 1,
    DetailedSubmissionLongConnection = 2,
    SimpleSubmissionsLongConnection = 3
};

enum UserPermission {
    ProblemEdit = 1 << 0,
    ProblemEditOthers = (1 << 0) | (1 << 1),
    ProblemDelete = 1 << 2,
    ProblemDeleteOthers = (1 << 2) | (1 << 3),
    ProblemListPage = 1 << 4,
    ProblemSubmit = 1 << 5,
    SubmissionListPage = 1 << 6,
    SubmissionDetailsPage = 1 << 7,
    SubmissionCode = 1 << 8,
    SubmissionCodeOthers =  (1 << 8) | (1 << 9),
    SubmissionRejudge = 1 << 10,
    SubmissionRejudgeOthers = (1 << 10) | (1 << 11),
    SubmissionData = 1 << 12,
    AdminPage = 1 << 13
};