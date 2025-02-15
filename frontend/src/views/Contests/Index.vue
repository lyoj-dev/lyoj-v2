<script lang="ts">
import { config } from '@/config';
import { myFetch } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import ContestTitle from '@/components/Contest/Title.vue';
import ContestNavigation from '@/components/Contest/Navigation.vue';
import markdownit from 'markdown-it';
import hljs from 'highlight.js';
import markdownItKatex from '@vscode/markdown-it-katex';
import { i18n } from '@/i18n';
import { useRoute } from 'vue-router';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var url = config.apiBase + "/contests/" + to.params.id;
    var json = await myFetch(url);

    next((e: any) => e.loading({
        data: json.item,
        identity: json.identity,
        signup: json.signup,
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script lang="ts" setup>
const loaded = ref(false);
const t = i18n.global.t;
const route = useRoute();

const item: any = ref({});
const identity: any = ref(0);
const signup: any = ref(false);

function loading(data: any) {
    item.value = data.data;
    identity.value = data.identity;
    signup.value = data.signup;
    loaded.value = true;
}

defineExpose({ loading });

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
    <div v-if="loaded">
        <ContestNavigation 
            current="index" 
            :id="route.params.id" 
            :identity="identity" 
            :signup="signup"
            :endTime="item.starttime + item.duration"
        ></ContestNavigation>
        <ContestTitle
            :id="item.id"
            :title="item.title"
            :startTime="item.starttime"
            :endTime="item.starttime + item.duration"
            :problems="item.problemNumber"
            :signups="item.signups"
        ></ContestTitle>
        <v-card class="ContestVCard card-radius">
            <v-card-title>{{ t('pages.contests.info') }}</v-card-title>
            <v-card-text class="markdown-text" v-html="md.render(item.info)"></v-card-text>
        </v-card>
    </div>
</template>

<style scoped>
.ContestVCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    width: 100%;
    margin-bottom: 20px;
    padding: 5px 10px 0px 10px;
}
</style>