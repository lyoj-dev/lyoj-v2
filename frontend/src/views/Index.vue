<script lang="ts">
import { defineComponent, ref } from "vue";
import { goto } from "@/router";
import NProgress from 'nprogress';
import markdownit from 'markdown-it'
import markdownItKatex from '@vscode/markdown-it-katex'
import hljs from 'highlight.js'
import { i18n } from '@/i18n';
export default defineComponent({
    async beforeRouteEnter(to, from, next) {
        NProgress.start();
        NProgress.inc();
        var response = await fetch("/announcement.md");
        if (response.status != 200) goto("error", { code: response.status });
        var text = await response.text();
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
</script>

<template>
    <v-card class="announcement card-radius">
        <v-card-title>{{ t('pages.index.announcement') }}</v-card-title>
        <v-card-text>
            <div v-html="result" class="markdown"></div>
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