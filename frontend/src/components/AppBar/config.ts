import { i18n } from '@/i18n';

const t = i18n.global.t;

export const appBarConfig: any = {
    left: [
        { title: "appbar.index", icon: "mdi-home", href: "/index", prefix: "/index" },
        { title: "appbar.problem", icon: "mdi-format-list-bulleted", href: "/problems/list", prefix: "/problems" },
        { title: "appbar.submission", icon: "mdi-server", href: "/submissions/list", prefix: "/submissions" },
        { title: "appbar.contest", icon: "mdi-calendar-blank", href: "/contests/list", prefix: "/contests" }
    ],
    right: [

    ]
};