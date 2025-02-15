<script lang="ts">
import { defineComponent, ref } from "vue";
import { goto } from "@/router";
import NProgress from 'nprogress';
import markdownit from 'markdown-it'
import markdownItKatex from '@vscode/markdown-it-katex'
import hljs from 'highlight.js'
import { i18n } from '@/i18n';
import { myFetch } from "@/utils";
import { config } from "@/config";
export default defineComponent({
    async beforeRouteEnter(to, from, next) {
        NProgress.start();
        NProgress.inc();
        var response = await fetch("/announcement.md");
        if (response.status != 200) goto("error", { code: response.status });
        var text = await response.text();
        var unused = await myFetch(config.apiBase + "/users/check");
        next((e: any) => e.loading({
            announcement: text
        }));
    },
});
</script>

<script setup lang="ts">
const t = i18n.global.t;
const md = markdownit({
    html: true,
    highlight: function (str: string, lang: string): string {
        if (str[str.length - 1] == '\n') str = str.substr(0, str.length - 1);
        if (lang && hljs.getLanguage(lang)) {
            try {
                var code: string = '<pre class="hljs"><code><ul>';
                var strs: string[] = str.split('\n');
                for (var i = 0; i < strs.length; i++) 
                    code += '<li class="hljs language-' + lang + '">' + hljs.highlight(lang, strs[i], true).value + '</li>'
                code += '</ul></code></pre>';
                return code;
            } catch (__) {}
        }
        var code: string = '<pre class="hljs"><code><ul>';
        var strs: string[] = str.split('\n');
        for (var i = 0; i < strs.length; i++) 
            code += '<li class="hljs language-' + lang + '">' + md.utils.escapeHtml(strs[i]) + '</li>'
        code += '</ul></code></pre>';
        return code;
    }
});
md.use(markdownItKatex);

const result = ref("");
function loading(data: any) {
    result.value = md.render(data.announcement);
}
defineExpose({ loading })

// async function download() {
//     for (var i = 1; i < 185; i++) {
//         if (i == 69 || i == 140 || i == 154) continue;
//         var response = await myFetch("/problem/" + i + "/problem.md");
//         if (response.status != 200) goto("error", { code: response.status });
//         var text = await response.text();
//         var html = md.render(text);
//         html = html.replaceAll("class=\"katex-html\"", "class=\"katex-html\" style=\"display: none\"");
//         html = html.replaceAll("<blockquote>", "<blockquote style=\"color: #666; border-left: 4px solid #666; padding-left: 20px; margin-left: 0; font-size: 14px; font-style: italic;\">");
//         html = html.replaceAll("<code><ul>", "<code><ul style=\"margin: 0px!important;\">");
//         html = html.replaceAll("<li class=\"", "<li style=\"list-style: none; font-family: 'Cascadia Mono', monospace!important; line-height: 25px; width: 100%; box-sizing: border-box; height: 25px; font-size: 15px; border-top-left-radius: 0px!important; border-bottom-left-radius: 0px!important; overflow-x: none!important; overflow-y: none!important;\" class=\"");
//         html = html.replaceAll("<ul>", "<ul style=\"padding-left: 2em;\">");
//         html = html.replaceAll("<ol>", "<ol style=\"padding-left: 2em;\">");
//         html = html.replaceAll("<table>", "<table style=\"display: block; width: 100%; overflow: auto; word-break: normal; word-break: keep-all; margin-top: 0; margin-bottom: 16px; border-collapse: collapse; border-spacing: 0;\">");
//         html = html.replaceAll("<tr>", "<tr style=\"border-top: 1px solid #ccc;\">");
//         html = html.replaceAll("<th ", "<th style=\"padding: 6px 13px; border: 1px solid #ddd; font-weight: bold;\" ");
//         html = html.replaceAll("<td", "<td style=\"padding: 6px 13px; border: 1px solid #ddd;\"");
//         html = html.replaceAll("class=\"hljs\"", "class=\"hljs\" style=\"background-color: rgb(242, 242, 242); border-radius: 10px; padding: 10px 0px; width: 100%;\"");
//         html = html.replaceAll("<code>", "<code style=\"font-family: 'Cascadia Mono', monospace!important; border-radius: 0px;\">");
//         var blob = new Blob([html], { type: "text/html" });
//         var url = URL.createObjectURL(blob);
//         var a = document.createElement("a");
//         a.href = url;
//         a.download = i + ".html";
//         a.click();
//         await sleep(100);
//     }
// }
</script>

<template>
    <!-- <v-btn @click="download">Download</v-btn> -->
    <v-card class="announcement card-radius">
        <v-card-title>{{ t('pages.index.announcement') }}</v-card-title>
        <v-card-text>
            <div v-html="result" class="markdown-text"></div>
        </v-card-text>
    </v-card>
</template>

<style lang="css" scoped>
.announcement {
    padding: 5px; 
    margin: auto; 
    background-color: var(--color-background);
    color: var(--color-text);
}
</style>