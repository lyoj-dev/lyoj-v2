import { status } from "nprogress";
import { OutputFileType } from "typescript";

export const zh_cn = {
    route: {
        index: "首页",
        problemsList: "题库",
        problemsDetails: "{name}",
        problemsEdit: "编辑题目",
        submissionsList: "评测",
        submissionsDetails: "评测详情",
        login: "登录"
    },
    appbar: {
        index: "首页",
        problem: "题库",
        submission: "评测",
        contest: "比赛",
        login: "登录",
        logout: "注销",
        admin: "管理"
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
            deleteConfirm: "确定删除题目 \"{name}\" 吗？",
            deleteSuccess: "删除成功",
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
            submitFailed: "提交失败",
            searchTitle: "搜索",
            difficulty: "难度",
            search: {
                title: "标题",
                tags: "标签",
                minDifficulty: "最小难度",
                maxDifficulty: "最大难度",
                searchButton: "搜索"
            },
            edit: {
                baseInfo: "题目基本信息",
                title: "题目名称",
                alias: "题目别名",
                tags: "题目标签",
                langs: "支持的语言",
                groups: "可访问用户组",
                inputFile: "输入文件名",
                outputFile: "输出文件名",
                preview: "编辑/预览",
                background: "题目背景",
                description: "题目描述",
                input: "输入格式",
                output: "输出格式",
                cases: "测试样例",
                casesInput: "样例输入 #{id}",
                casesOutput: "样例输出 #{id}",
                appendCase: "添加样例",
                hint: "数据范围与提示",
                datas: "数据配置",
                uploadData: "上传数据",
                dataConfig: "测试点配置",
                data: {
                    title: "测试点",
                    timeLimit: "时间限制",
                    memoryLimit: "空间限制",
                    score: "测试点分数",
                    subtask: "所属子任务",
                    oneKey: "一键配置"
                },
                subtaskConfig: "子任务配置",
                subtask: {
                    id: "子任务编号",
                    title: "子任务名称",
                    type: "计分类型",
                    depends: "子任务依赖",
                    sumType: "得分总和",
                    minType: "得分最小值",
                    maxType: "得分最大值",
                    avgType: "得分平均值",
                    none: "不分配子任务",
                    appendSubtask: "新建子任务"
                },
                spj: "SPJ 配置",
                spjType: "SPJ 类型",
                customSpj: "自定义 SPJ",
                spjSource: "SPJ 源文件名",
                spjCompileCmd: "SPJ 编译指令",
                spjExecName: "SPJ 可执行文件名",
                spjExecParam: "SPJ 额外执行参数",
                submit: "提交"
            }
        },
        login: {
            title: "登录",
            user: "用户名",
            userPlaceholder: "请在此输入您的用户名/学号",
            password: "密码",
            passwordPlaceholder: "请在此输入您的密码",
            loginButton: "登录",
            loginSuccess: "登录成功",
            userNotFound: "用户不存在",
            disabledPassword: "该用户未启用密码登录，请使用农大登录",
            wrongPassword: "密码错误"
        },
        logout: {
            success: "注销成功"
        }
    }
};