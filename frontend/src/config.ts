export const config = {
    apiBase: "http://192.168.0.12:8080",
    wsBase: "ws://192.168.0.12:8080",
    title: {
        short: "LYOJ",
        full: "LittleYang OnlineJudge"
    },
};

export const statusList = [
    {
        icon: 'mdi-check',
        color: 'rgb(36, 140, 36)',
        text: 'Accepted'
    }, {
        icon: 'mdi-close',
        color: 'rgb(255, 0, 0)',
        text: 'Wrong Answer'
    }, {
        icon: 'mdi-clock',
        color: 'rgb(244, 164, 96)',
        text: 'Time Limit Exceeded'
    }, {
        icon: 'mdi-database',
        color: 'rgb(244, 164, 96)',
        text: 'Memory Limit Exceeded'
    }, {
        icon: 'mdi-bomb',
        color: 'rgb(153, 50, 204)',
        text: 'Runtime Error'
    }, {
        icon: 'mdi-progress-question',
        color: 'pink',
        text: 'No Test Data'
    }, {
        icon: 'mdi-progress-question',
        color: 'pink',
        text: 'Unknown Error'
    }, {
        icon: 'mdi-progress-question',
        color: 'pink',
        text: 'Partial Correct'
    }, {
        icon: 'mdi-code-tags',
        color: 'rgb(0, 68, 136)',
        text: 'Compile Error'
    }, {
        icon: 'mdi-progress-question',
        color: 'pink',
        text: 'Waiting...'
    }, {
        icon: 'mdi-spin mdi-loading',
        color: '#6cf',
        text: 'Compiling...'
    }, {
        icon: 'mdi-spin mdi-loading',
        color: '#6cf',
        text: 'Running...'
    }, {
        icon: 'mdi-fast-forward',
        color: 'rgb(180, 204, 216)',
        text: 'Skipped'
    }, {
        icon: 'mdi-check',
        color: 'rgb(36, 140, 36)',
        text: 'Submitted'
    }
];

export const difficultyList = [
    {
        title: "暂无评定",
        color: "rgb(191,191,191)"
    }, {
        title: "入门",
        color: "rgb(254,76,97)"
    }, {
        title: "普及-",
        color: "rgb(243,156,17)"
    }, {
        title: "普及\/提高-",
        color: "rgb(255,193,22)"
    }, {
        title: "普及+\/提高",
        color: "rgb(82,196,26)"
    }, {
        title: "提高+\/省选-",
        color: "rgb(52,152,219)"
    }, {
        title: "省选\/NOI-",
        color: "rgb(157,61,207)"
    }, {
        title: "NOI\/NOI+\/CTSC",
        color: "rgb(14,29,105)"
    }
];

export const tagsTypeList = [
    {
        title: "算法标签",
        color: "rgb(41,73,180)"
    }
]