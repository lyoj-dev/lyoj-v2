import { status } from "nprogress";
import { OutputFileType } from "typescript";

export const zh_cn = {
    route: {
        index: "首页",
        problemsList: "题库",
        problemsDetails: "{name}",
        submissionsList: "评测",
        submissionsDetails: "评测详情",
    },
    appbar: {
        index: "首页",
        problem: "题库",
        submission: "评测",
        contest: "比赛",
        login: "登录",
        logout: "注销"
    },
    pages: {
        index: {
            announcement: "系统公告"
        },
        submissions: {
            code: "代码",
            copy: "复制",
            rejudge: "重测",
            compileInfo: "编译信息",
            case: "测试点 #{id}",
            subtask: "子任务 #{id}",
            score: "分数",
            time: "时间",
            memory: "内存",
            inputFile: "输入文件",
            answerFile: "答案文件",
            outputFile: "用户输出",
            checkerOutput: "校验器输出",
            ignored: "已省略 {size}",
            searchTitle: "搜索",
            search: {
                problems: "指定题目",
                users: "指定用户",
                languages: "代码语言",
                status: "评测状态",
                searchButton: "搜索"
            }
        },
        problems: {
            background: "题目背景",
            description: "题目描述",
            input: "输入格式",
            output: "输出格式",
            cases: "测试样例",
            casesInput: "样例输入 #{id}",
            casesOutput: "样例输出 #{id}",
            hint: "数据范围与提示",
            submit: "提交",
            languages: "选择代码语言",
            submitSuccess: "提交成功",
            searchTitle: "搜索",
            search: {
                title: "标题",
                tags: "标签",
                difficulties: "难度",
                searchButton: "搜索"
            }
        }
    }
};