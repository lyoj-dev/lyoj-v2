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
        contestsList: "比赛",
        contestIndex: "比赛首页",
        contestRegister: "报名比赛",
        contestProblemsList: "题目列表",
        contestProblemsDetails: "{name}",
        contestSubmissionsList: "评测列表",
        contestSubmissionsDetails: "评测详情",
        contestRanking: "比赛排名",
        contestEdit: "编辑比赛",
        userDetails: "个人主页",
        login: "登录",
        casLogin: "农大登录",
        adminIndex: "管理首页"
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
            copySuccess: "复制成功",
            rejudge: "重测",
            rejudgeSuccess: "重测成功",
            rejudgeFailed: "重测失败",
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
            addProblem: "新建题目",
            deleteConfirm: "确定删除题目 \"{name}\" 吗？",
            deleteSuccess: "删除成功",
            inputFile: "输入文件",
            outputFile: "输出文件",
            timeLimit: "时间限制",
            memoryLimit: "空间限制",
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
                addTag: "新建标签",
                addTagTitle: "标签名称",
                addTagType: "标签类型",
                addTagTitleEmpty: "标签名称不能为空",
                addTagFailed: "新建标签失败",
                addTagSuccess: "新建标签成功",
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
                uploadDataFailed: "上传数据失败",
                uploadDataSuccess: "上传数据成功",
                downloadData: "下载数据",
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
                submit: "提交",
                submitFailed: "提交失败",
                submitSuccess: "提交成功"
            }
        },
        contests: {
            notStarted: "未开始",
            running: "进行中",
            ended: "已结束",
            problemNumber: "题目数量",
            addContest: "新建比赛",
            time: "比赛时间",
            signups: "报名人数",
            visitAs: "用户身份",
            visitor: "访客",
            contestant: "参赛者",
            admin: "管理员",
            index: "比赛首页",
            problems: "题目列表",
            submissions: "评测列表",
            ranking: "比赛排名",
            register: "报名比赛",
            password: "比赛密码",
            registerButton: "报名",
            registerSuccess: "报名成功",
            registerFailed: "密码错误",
            unregister: "取消报名比赛",
            unregisterSuccess: "取消报名成功",
            info: "比赛信息",
            rank: "排名",
            username: "用户名",
            score: "得分",
            penalty: "罚时",
            editButton: "编辑比赛",
            edit: {
                baseInfo: "比赛基本信息",
                title: "比赛名称",
                startTime: "开始时间",
                endTime: "结束时间",
                timePicker: "选择时间",
                type: "比赛类型",
                tags: "比赛标签",
                addTag: "新建标签",
                addTagTitle: "标签名称",
                addTagType: "标签类型",
                addTagTitleEmpty: "标签名称不能为空",
                addTagFailed: "新建标签失败",
                addTagSuccess: "新建标签成功",
                problems: "比赛题目",
                password: "比赛密码",
                hasPassword: "您之前设置过一次密码，但由于安全原因，我们无法解密您的初始密码，您需要重新设置密码",
                info: "比赛信息",
                preview: "编辑/预览",
                submit: "提交",
                submitFailed: "提交失败",
                submitSuccess: "提交成功"
            }
        },
        users: {
            info: "共进行 {submissions} 次提交，解决了 {problems} 道问题。",
            rating: "当前评分：{rating}。",
            infoButton: "个人介绍",
            problemsButton: "做题情况",
            settingButton: "个人设定",
            triedProblem: "尝试的题目",
            acceptedProblem: "通过的题目",
            uploadHeader: "上传头像",
            uploadHeaderButton: "点我上传",
            uploadHeaderSuccess: "上传成功",
            uploadHeaderFailed: "上传失败",
            uploadBanner: "上传空间头图",
            uploadBannerButton: "点我上传",
            uploadBannerSuccess: "上传成功",
            uploadBannerFailed: "上传失败",
            username: "用户名",
            introduction: "个人介绍",
            preview: "编辑/预览",
            submit: "提交",
            submitSuccess: "提交成功",
            submitFailed: "提交失败",
            submitFailedTitleConflict: "用户名已存在",
            submitFailedTitleEmpty: "用户名不能为空",
            oldPassword: "旧密码",
            newPassword: "新密码",
            confirmPassword: "确认密码",
            passwordNotMatch: "两次输入的密码不一致",
            updatePasswordSuccess: "密码修改成功",
            updatePasswordFailed: "原密码错误",
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
            wrongPassword: "密码错误",
            casLogin: "或者，使用农大账号登录",
            casLoginButton: "农大登录"
        },
        logout: {
            success: "注销成功"
        },
        admin: {
            index: {
                title: "首页"
            },
            problems: {
                title: "题库管理",
            },
            submissions: {
                title: "评测管理",
            },
            contests: {
                title: "比赛管理",
            },
            tags: {
                title: "标签管理",
            },
            users: {
                title: "用户管理",
            },
            groups: {
                title: "用户组管理",
            }
        }
    }
};