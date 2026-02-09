#pragma once

#include <string>
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

std::string JudgeResultInfo[] = {
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

enum ContestIdentity {
    Visitor = 0,
    Contestant = 1,
    Administrator = 2
};

enum ContestType {
    OI = 0,
    IOI = 1,
    ACM = 2
};

enum UserPermission {
    ProblemEdit = 1 << 0, // 新建题目，编辑自己的题目，删除自己的题目
    ProblemEditOthers = 1 << 1, // 编辑别人的题目，删除别人的题目
    AddTag = 1 << 2, // 新建未知标签权限
    SubmissionListOthers = 1 << 3, // 列举他人提交代码
    SubmissionCodeOthers = 1 << 4, // 查看他人提交代码
    SubmissionRejudge = 1 << 5, // 重测自己提交的代码
    SubmissionRejudgeOthers = 1 << 6, // 重测他人提交的代码
    SubmissionData = 1 << 7, // 查看测试数据
    ContestEdit = 1 << 8, // 新建比赛，编辑比赛
    ContestEditOthers = 1 << 9, // 高级比赛管理员权限
    UserEditOthers = 1 << 10, // 编辑他人信息权限
    AdminPage = 1 << 11 // 管理员页面权限
};