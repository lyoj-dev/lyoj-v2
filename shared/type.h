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