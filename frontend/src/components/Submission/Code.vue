<script setup lang="ts">
import hljs from 'highlight.js'
import { i18n } from '@/i18n';
import SubmissionCodeInfo from './CodeInfo.vue';
import { computed } from 'vue';
import { showMsg } from '@/utils';

const t = i18n.global.t;
const props = defineProps([ 'code', 'langCode', 'time', 'memory', 'langName', 'date' ]);

function highlight(str: string, lang: string) {
    var code: string = '<pre class="hljs" style="margin: 0px!important;"><code><ul>';
    var strs: string[] = str.split('\n');
    for (var i = 0; i < strs.length; i++) 
        code += '<li class="hljs language-' + lang + '">' + hljs.highlight(lang, strs[i], true).value + '</li>'
    code += '</ul></code></pre>';
    return code;
}

function timeToDate(timestamp: number) {
    var d: Date = new Date(timestamp * 1000);
    return d.getFullYear() + '-' + 
        (d.getMonth() + 1).toString().padStart(2, '0') + '-' + 
        d.getDate().toString().padStart(2, '0') + ' ' + 
        d.getHours().toString().padStart(2, '0') + ':' +
        d.getMinutes().toString().padStart(2, '0') + ':' + 
        d.getSeconds().toString().padStart(2, '0');
}

const CodeInfo = [
    { icon: 'mdi-clock', info: computed(() => Math.round(props.time / 1000 * 100) / 100 + 's') },
    { icon: 'mdi-database', info: computed(() => Math.round(props.memory / 1024 * 100) / 100 + 'MB') },
    { icon: 'mdi-code-tags', info: computed(() => Math.round(props.code.length / 1000 * 100) / 100 + "KB " + props.langName) },
    { icon: 'mdi-calendar', info: computed(() => timeToDate(props.date)) }
];

function copyCode(e: MouseEvent) {
    const el = document.createElement('textarea');
    el.value = props.code;
    document.body.appendChild(el);
    el.select();
    document.execCommand('copy');
    document.body.removeChild(el);
    e.stopPropagation();
    showMsg("success", "Copy Success!");
}
</script>

<template>
    <v-expansion-panels :model-value="[0]">
        <v-expansion-panel class="SubmissionCode card-radius" static>
            <v-expansion-panel-title class="d-flex align-center" style="padding: 0px 20px 0px 10px;">
                <p class="v-card-title" style="padding-right: 0px">{{ t('pages.submissions.code') }}</p>
                <v-btn
                    size="small"
                    class="SubmissionCode-button"
                    @click="copyCode"
                >{{ t('pages.submissions.copy') }}</v-btn>
                <v-btn
                    size="small"
                    class="SubmissionCode-button"
                >{{ t('pages.submissions.rejudge') }}</v-btn>
                <div
                    v-for="(item, index) in CodeInfo"
                    class="d-flex align-center"
                >
                    <SubmissionCodeInfo
                        :icon="item.icon"
                        :info="item.info"
                    ></SubmissionCodeInfo>
                    <p 
                        style="font-size: 15px;"
                        v-if="index != CodeInfo.length - 1"
                    >|</p>
            </div>
            </v-expansion-panel-title>
            <v-expansion-panel-text style="padding-bottom: 0px;">
                <div v-html="highlight(code, langCode)" class="markdown SubmissionCode-code"></div>
            </v-expansion-panel-text>
        </v-expansion-panel>
    </v-expansion-panels>
</template>

<style scoped>
.SubmissionCode {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 5px;
    margin-bottom: 20px;
}

.SubmissionCode-button {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
    position: relative;
    top: 1.5px;
    margin-left: 5px;
}

.SubmissionCode-code {
    width: 100%;
    padding-bottom: 0px;
}
</style>

<style>
.v-expansion-panel-text__wrapper {
    padding: 0 1rem;
}
</style>