<script lang="ts">
import { config } from '@/config';
import { locate } from '@/router';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import markdownit from 'markdown-it'
import markdownItKatex from '@vscode/markdown-it-katex'
import hljs from 'highlight.js'
import { i18n } from '@/i18n';
import ProblemTitle from '@/components/Problem/Title.vue';
import ProblemCase from '@/components/Problem/Case.vue';
import ContestNavigation from '@/components/Contest/Navigation.vue';
import MonacoEditor from 'monaco-editor-vue3';
import { loginAs, myFetch, showMsg, sleep } from '@/utils';
import { useRoute } from 'vue-router';
import { fa } from 'vuetify/locale';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var data = 
        to.params.cid == undefined ?
        await myFetch(config.apiBase + "/problems/" + to.params.id) :
        await myFetch(config.apiBase + "/contests/" + to.params.cid + "/problems/" + to.params.id);
    var languages = (await myFetch(config.apiBase + "/configurations/languages")).items;
    for (var i = 0; i < languages.length; i++) languages[i].id = i;
    var contest = 
        to.params.cid == undefined ?
        {} :
        await myFetch(config.apiBase + "/contests/" + to.params.cid);
    
    next((e: any) => e.loading({
        data: data,
        languages: languages,
        contest: contest
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
const route = useRoute();

const item: any = ref({});
const languages: any = ref([]);
const loaded = ref(false);
const contest: any = ref({});

const submitLanguage: any = ref(0);
const submitCode = ref("");
const enableBtn = ref(true);

function loading(data: any) {
    item.value = data.data;
    languages.value = [];
    contest.value = data.contest;
    submitCode.value = data.data.item.lastCode;
    submitLanguage.value = data.data.item.lastLang;
    document.title = data.data.item.title + ' - ' + config.title.full;
    for (var i = 0; i < data.languages.length; i++) {
        for (var j = 0; j < data.data.item.langs.length; j++) {
            if (data.languages[i].id == data.data.item.langs[j]) {
                languages.value.push(data.languages[i]);
                break;
            }
        }
    }
    enableBtn.value = languages.value.length;

    loaded.value = true;
}
defineExpose({ loading });

async function submit() {
    enableBtn.value = false;
    var data = {
        lang: submitLanguage.value,
        code: submitCode.value,
    };

    var res = await myFetch(config.apiBase + (route.params.cid != undefined ? "/contests/" + route.params.cid : "" ) + "/problems/" + item.value.item.id + "/submit", {
        method: "POST",
        body: JSON.stringify(data)
    }, false);
    if (res.id != undefined && res.id != null && res.id > 0) {
        showMsg("success", t('pages.problems.submitSuccess'));
        await sleep(1000);
        locate((route.params.cid != undefined ? "/contests/" + route.params.cid : "") + "/submissions/" + res.id);    
    } else {
        showMsg("success", t('pages.problems.submitFailed'));
        enableBtn.value = true;
        await sleep(1000);
    }
}
</script>

<template>
    <div v-if="loaded">
        <ContestNavigation 
            :id="route.params.cid" 
            current="problems"
            :identity="item.identity"
            :signup="item.signup"
            :endTime="contest.item.starttime + contest.item.duration"
            v-if="route.params.cid != undefined"
        ></ContestNavigation>
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
        <v-card 
            class="ProblemVCard card-radius" 
            v-if="
                loginAs != 0 && 
                item.config.minTime <= item.config.maxTime && 
                item.config.minMemory <= item.config.maxMemory
            "
        >
            <v-card-title>{{ t('pages.problems.submit') }}</v-card-title>
            <v-card-text>
                <div class="d-flex align-center">
                    <p>{{ t('pages.problems.languages') }}：&nbsp;</p>
                    <v-select
                        v-model="submitLanguage"
                        :items="languages"
                        item-title="name"
                        item-value="id"
                        density="compact"
                        hide-details
                    ></v-select>
                </div>
                <MonacoEditor
                    :language="languages.length == 0 ? '' : languages[submitLanguage].mode"
                    theme="vs-dark"
                    height="500"
                    v-model:value="submitCode"
                    style="margin-top: 15px;"
                ></MonacoEditor>
                <div class="d-flex justify-center mt-3">
                    <v-btn class="submitButton" @click="submit()" :disabled="!enableBtn">{{ t('pages.problems.submit') }}</v-btn>
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