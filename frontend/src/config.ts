import { i18n } from "./i18n";

const t = i18n.global.t;

export const config = {
    // apiBase: "https://api-v2.lyoj.littleyang.icu",
    // wsBase: "wss://api-v2.lyoj.littleyang.icu",
    apiBase: "http://127.0.0.1:8081",
    wsBase: "ws://127.0.0.1:8081",
    // apiBase: "http://192.168.0.5:8081",
    // wsBase: "ws://192.168.0.5:8081",
    casBase: "https://onecas.cau.edu.cn/tpass/login",
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

export const statusCardList = [
    {
        icon: 'mdi-minus',
        color: '#767676'
    }, {
        icon: 'mdi-check',
        color: 'rgb(36, 140, 36)'
    }, {
        icon: 'mdi-close',
        color: 'rgb(255, 0, 0)'
    }
];

export const contestStatusCardList = [
    {
        title: t('pages.contests.notStarted'),
        color: "green"
    }, {
        title: t('pages.contests.running'),
        color: "blue"
    }, {
        title: t('pages.contests.ended'),
        color: "red"
    }
]

export const difficultyList = [
    {
        t: 0,
        r: 254,
        g: 76,
        b: 97,
        color: "rgb(254, 76, 97)"
    }, {
        t: 1 / 6,
        r: 243,
        g: 156,
        b: 17,
        color: "rgb(243, 156, 17)"
    }, {
        t: 2 / 6,
        r: 255,
        g: 193,
        b: 22,
        color: "rgb(255, 193, 22)"
    }, {
        t: 3 / 6,
        r: 82,
        g: 196,
        b: 26,
        color: "rgb(82, 196, 26)"
    }, {
        t: 4 / 6,
        r: 52,
        g: 152,
        b: 219,
        color: "rgb(52, 152, 219)"
    }, {
        t: 5 / 6,
        r: 157,
        g: 61,
        b: 207,
        color: "rgb(157, 61, 207)"
    }, {
        t: 1,
        r: 14,
        g: 29,
        b: 105,
        color: "rgb(14, 29, 105)"
    }
];
export const maxDifficulty = 4000;

export const tagsTypeList = [
    {
        id: 0,
        title: "算法标签",
        color: "rgb(41, 73, 180)"
    }, {
        id: 1,
        title: "日期标签",
        color: "rgb(180, 41, 41)"
    }, {
        id: 2,
        title: "来源标签",
        color: "rgb(255, 193, 22)"
    }, {
        id: 3,
        title: "学校标签",
        color: "rgb(82, 196, 26)"
    }, {
        id: 4,
        title: "其他标签",
        color: "rgb(52, 152, 219)"
    }
]

export const groupColor = "rgb(41, 73, 180)";