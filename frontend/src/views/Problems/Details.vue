<script lang="ts">
import { config } from '@/config';
import { goto, locate, sleep } from '@/router';
import NProgress from 'nprogress';
import { defineComponent, inject, ref } from 'vue';
import markdownit from 'markdown-it'
import markdownItKatex from '@vscode/markdown-it-katex'
import hljs from 'highlight.js'
import { i18n } from '@/i18n';
import ProblemTitle from '@/components/Problem/Title.vue';
import ProblemCase from '@/components/Problem/Case.vue';
import MonacoEditor from 'monaco-editor-vue3';
import { red } from 'vuetify/util/colors';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();
    var response = await fetch(config.apiBase + "/problems/" + to.params.id);
    if (response.status != 200) goto("error", { code: response.status });
    var data = await response.json();
    var response = await fetch(config.apiBase + "/configurations/languages");
    if (response.status != 200) goto("error", { code: response.status });
    var languages = (await response.json()).items;
    for (var i = 0; i < languages.length; i++) languages[i].id = i;
    next((e: any) => e.loading({
        data: data,
        languages: languages
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
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
const showMsg: any = inject('showMsg');

const item: any = ref({});
const languages: any = ref([]);
const loaded = ref(false);

const submitLanguage: any = ref(0);
const submitCode = ref("");

function loading(data: any) {
    item.value = data.data;
    languages.value = data.languages;
    document.title = data.data.item.title + ' - ' + config.title.full;

    loaded.value = true;
}

async function submit() {
    var data = {
        lang: submitLanguage.value,
        code: submitCode.value,
    };

    var response = await fetch(config.apiBase + "/problems/" + item.value.item.id + "/submit", {
        method: "POST",
        body: JSON.stringify(data),
    });
    if (response.status != 200) goto("error", { code: response.status });
    var res = await response.json();
    showMsg("success", t('pages.problems.submitSuccess'));
    await sleep(1000);
    locate("/submissions/" + res.id);
}

defineExpose({ loading });
</script>

<template>
    <div v-if="loaded">
        <ProblemTitle
            :alias="item.item.alias"
            :title="item.item.title"
            :inputFile="item.config.input"
            :outputFile="item.config.output"
            :minTime="item.config.minTime"
            :maxTime="item.config.maxTime"
            :minMemory="item.config.minMemory"
            :maxMemory="item.config.maxMemory"
        ></ProblemTitle>
        <v-card class="ProblemVCard card-radius" v-if="item.item.bg != ''">
            <v-card-title>{{ t('pages.problems.background') }}</v-card-title>
            <v-card-text class="markdown-text" v-html="md.render(item.item.bg)"></v-card-text>
        </v-card>
        <v-card class="ProblemVCard card-radius" v-if="item.item.descrip != ''">
            <v-card-title>{{ t('pages.problems.description') }}</v-card-title>
            <v-card-text class="markdown-text" v-html="md.render(item.item.descrip)"></v-card-text>
        </v-card>
        <v-card class="ProblemVCard card-radius" v-if="item.item.input != ''">
            <v-card-title>{{ t('pages.problems.input') }}</v-card-title>
            <v-card-text class="markdown-text" v-html="md.render(item.item.input)"></v-card-text>
        </v-card>
        <v-card class="ProblemVCard card-radius" v-if="item.item.output != ''">
            <v-card-title>{{ t('pages.problems.output') }}</v-card-title>
            <v-card-text class="markdown-text" v-html="md.render(item.item.output)"></v-card-text>
        </v-card>
        <v-card class="ProblemVCard card-radius" v-if="item.item.cases.length != 0">
            <v-card-title>{{ t('pages.problems.cases') }}</v-card-title>
            <v-card-text>
                <ProblemCase
                    v-for="(item, index) in item.item.cases"
                    :id="index + 1"
                    :input="item.input"
                    :output="item.output"
                ></ProblemCase>
            </v-card-text>
        </v-card>
        <v-card class="ProblemVCard card-radius" v-if="item.item.hint != ''">
            <v-card-title>{{ t('pages.problems.hint') }}</v-card-title>
            <v-card-text class="markdown-text" v-html="md.render(item.item.hint)"></v-card-text>
        </v-card>
        <v-card class="ProblemVCard card-radius">
            <v-card-title>{{ t('pages.problems.submit') }}</v-card-title>
            <v-card-text>
                <div class="d-flex align-center">
                    <p>{{ t('pages.problems.languages') }}:&nbsp;</p>
                    <v-select
                        v-model="submitLanguage"
                        :items="languages"
                        item-title="name"
                        item-value="id"
                        hide-details
                    ></v-select>
                </div>
                <MonacoEditor
                    :language="languages[submitLanguage].mode"
                    theme="vs-dark"
                    height="500"
                    v-model:value="submitCode"
                    style="margin-top: 15px;"
                ></MonacoEditor>
                <div class="d-flex justify-center mt-3">
                    <v-btn class="submitButton" @click="submit()" size="small">{{ t('pages.problems.submit') }}</v-btn>
                </div>
            </v-card-text>
        </v-card>
    </div>
</template>

<style scoped>
.ProblemVCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    width: 100%;
    margin-bottom: 20px;
    padding: 5px 10px 0px 10px;
}

.submitButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}
</style>