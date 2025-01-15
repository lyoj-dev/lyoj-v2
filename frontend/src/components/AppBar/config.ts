import { i18n } from '@/i18n';

const t = i18n.global.t;

export const appBarConfig = {
    left: [
        { title: "appbar.index", icon: "mdi-home", href: "/index" },
        { title: "appbar.problem", icon: "mdi-format-list-bulleted", href: "/problems/list" },
        { title: "appbar.submission", icon: "mdi-server", href: "/submissions/list" },
        { title: "appbar.contest", icon: "mdi-calendar-blank", href: "/contest" }
    ],
    right: [
        { title: "appbar.login", icon: "mdi-arrow-right-bold-circle", href: "/login" }
    ]
};