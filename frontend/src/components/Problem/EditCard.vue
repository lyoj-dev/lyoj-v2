<script lang="ts" setup>
import { ref } from 'vue';
import hljs from 'highlight.js';
import markdownit from 'markdown-it';
import markdownItKatex from '@vscode/markdown-it-katex'
import MonacoEditor from 'monaco-editor-vue3';
import { i18n } from '@/i18n';

const t = i18n.global.t;
const isPreview = ref(false);
const model: any = defineModel('value');
const props = defineProps([ 'title' ]);

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
</script>

<template>
    <v-card class="ProblemEdit-card card-radius">
        <v-card-title>
            {{ title }}
            <v-btn
                size="small"
                class="ProblemEdit-button"
                @click="isPreview = !isPreview"
            >{{ t('pages.problems.edit.preview') }}</v-btn>
        </v-card-title>
        <v-card-text>
            <MonacoEditor
                v-if="!isPreview"
                language="markdown"
                theme="vs-dark"
                height="500"
                v-model:value="model"
                style="margin-top: 5px;"
                :options="{ wordWrap: 'on' }"
            ></MonacoEditor>
            <div
                v-else
                class="markdown-text"
                v-html="md.render(model)"
                style="min-height: 500px; max-height: 500px; overflow-y: auto;"
            ></div>
        </v-card-text>
    </v-card>
</template>

<style scoped>
.ProblemEdit-button {
    color: var(--color-text)!important;     
    background-color: var(--color-background-mute)!important;
    position: relative;
    top: -1.5px;
    margin-left: 5px;
}
</style>