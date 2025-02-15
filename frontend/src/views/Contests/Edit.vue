<script lang="ts">
import { config, tagsTypeList } from '@/config';
import { myFetch, showMsg, sleep } from '@/utils';
import NProgress from 'nprogress';
import { computed, defineComponent, ref, watchEffect } from 'vue';
import { useRoute } from 'vue-router';
import ContestNavigation from '@/components/Contest/Navigation.vue';
import { i18n } from '@/i18n';
import hljs from 'highlight.js';
import markdownit from 'markdown-it';
import markdownItKatex from '@vscode/markdown-it-katex'
import MonacoEditor from 'monaco-editor-vue3';
import { locate } from '@/router';
import crypto from 'crypto-js';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var data = to.params.id == 0 ? {
        item: {
            title: '',
            info: '',
            starttime: new Date().getTime() / 1000,
            duration: 24 * 60 * 60,
            type: 0,
            tags: [],
            problems: [],
            password: '',
            hasPassword: false
        }
    }: await myFetch(config.apiBase + "/contests/" + to.params.id);
    var problems = await myFetch(config.apiBase + '/problems/listAll');
    for (var i = 0; i < problems.items.length; i++) problems.items[i].name = problems.items[i].alias + " - " + problems.items[i].title;
    var tags = await myFetch(config.apiBase + '/problems/listAllTags');
    var contestProblems = to.params.id == 0 ? { items: [] } : await myFetch(config.apiBase + '/contests/' + to.params.id + '/problems/list');
    data.item.problems = [];
    for (var i = 0; i < contestProblems.items.length; i++) data.item.problems.push(contestProblems.items[i].id);
    
    next((e: any) => e.loading({
        data: data,
        problems: problems.items,
        tags: tags.items
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script lang="ts" setup>
const loaded = ref(false);
const contest: any = ref({});
const problems: any = ref([]);
const tags: any = ref([]);
const route = useRoute();
const t = i18n.global.t;
const calendarDialog1 = ref(false);
const calendarDialog2 = ref(false);
const addTagDialog = ref(false);

const time1Date = ref(new Date());
const time1Time = ref("");
const time2Date = ref(new Date());
const time2Time = ref("");

const addTagTitle = ref('');
const addTagType = ref(0);

const enableBtn = ref(true);

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
const isPreview = ref(false);

function loading(data: any) {
    contest.value = data.data;
    problems.value = data.problems;
    tags.value = data.tags;
    contest.value.item.tags = contest.value.item.tags.map((tag: any) => tag.id);
    contest.value.item.password = '';
    
    var date = new Date(contest.value.item.starttime * 1000);
    time1Date.value = new Date(date.getFullYear() + '-' +
        (date.getMonth() + 1).toString().padStart(2, '0') + '-' +
        date.getDate().toString().padStart(2, '0'));
    time1Time.value = date.getHours().toString().padStart(2, '0') + ':' +
        date.getMinutes().toString().padStart(2, '0') + ':' +
        date.getSeconds().toString().padStart(2, '0');
        
    var date = new Date((contest.value.item.starttime + contest.value.item.duration) * 1000);
    time2Date.value = new Date(date.getFullYear() + '-' +
        (date.getMonth() + 1).toString().padStart(2, '0') + '-' +
        date.getDate().toString().padStart(2, '0'));
    time2Time.value = date.getHours().toString().padStart(2, '0') + ':' +
        date.getMinutes().toString().padStart(2, '0') + ':' +
        date.getSeconds().toString().padStart(2, '0');

    loaded.value = true;
}

defineExpose({ loading });

async function addTagSubmit() {
    var title = addTagTitle.value;
    var type = addTagType.value;
    console.log(title, type);
    if (title == "") {
        showMsg('error', t('pages.contests.edit.addTagTitleEmpty'));
        addTagDialog.value = false;
        return;
    }
    var res = await myFetch(config.apiBase + "/problems/addTag", {
        method: 'POST',
        body: JSON.stringify({ title: title, type: type })
    }, false);
    if (res.code != 200) {
        showMsg('error', t('pages.contests.edit.addTagFailed'));
        addTagDialog.value = false;
        return;
    }
    showMsg('success', t('pages.contests.edit.addTagSuccess'));
    tags.value.push({ id: res.id, title: title, type: type });
    contest.value.item.tags.push(res.id);
    addTagDialog.value = false; 
}

async function submit() {
    var data = contest.value.item;
    data = JSON.parse(JSON.stringify(data));
    data.starttime = new Date(time1Date.value.toLocaleDateString() + ' ' + time1Time.value).getTime() / 1000;
    data.duration = new Date(time2Date.value.toLocaleDateString() + ' ' + time2Time.value).getTime() / 1000 - data.starttime;
    data.password = data.password != '' ? crypto.MD5(data.password).toString() : '';

    var res = await myFetch(config.apiBase + "/contests/" + route.params.id + "/create", {
        method: 'POST',
        body: JSON.stringify(data)
    }, false);
    if (res.code != 200) {
        showMsg('error', t('pages.contests.edit.submitFailed'));
        enableBtn.value = true;
        return;
    }
    showMsg('success', t('pages.contests.edit.submitSuccess'));
    await sleep(1000);
    locate('/contests/' + res.id + '/index');
}
</script>

<template>
    <div v-if="loaded">
        <ContestNavigation 
            v-if="Number(route.params.id) != 0"
            current="edit" 
            :id="route.params.id" 
            :identity="contest.identity" 
            :signup="contest.signup"
            :endTime="contest.item.starttime + contest.item.duration"
        ></ContestNavigation>
        <v-card class="ContestEdit-card card-radius">
            <v-card-title>{{ t('pages.contests.edit.baseInfo') }}</v-card-title>
            <v-card-text>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.contests.edit.title') }}：</p>
                    <v-text-field
                        v-model="contest.item.title"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.contests.edit.startTime') }}：</p>
                    <v-text-field
                        :model-value="new Date(time1Date.toLocaleDateString() + ' ' + time1Time).toLocaleString()"
                        dense
                        hide-details
                        density="compact"
                        disabled
                    ></v-text-field>
                    <v-btn
                        class="ContestEdit-button"
                        icon="mdi-calendar"
                        size="small"
                        @click="() => calendarDialog1 = !calendarDialog1"
                    ></v-btn>
                    <v-dialog
                        v-model="calendarDialog1"
                        width="auto"
                    >
                        <v-card class="card-radius ContestEdit-card">
                            <v-card-title>{{ t('pages.contests.edit.timePicker') }}</v-card-title>
                            <v-card-text class="d-flex align-center">
                                <v-date-picker
                                    v-model="time1Date"
                                    first-day-of-week="1"
                                    show-week
                                    show-adjacent-months
                                    class="ContestEdit-picker"
                                ></v-date-picker>
                                <v-time-picker
                                    v-model="time1Time"
                                    format="24hr"
                                    use-seconds
                                    scrollable
                                    style="margin-left: 10px;"
                                    class="ContestEdit-picker"
                                ></v-time-picker>
                            </v-card-text>
                        </v-card>
                    </v-dialog>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.contests.edit.endTime') }}：</p>
                    <v-text-field
                        :model-value="new Date(time2Date.toLocaleDateString() + ' ' + time2Time).toLocaleString()"
                        dense
                        hide-details
                        density="compact"
                        disabled
                    ></v-text-field>
                    <v-btn
                        class="ContestEdit-button"
                        icon="mdi-calendar"
                        size="small"
                        @click="() => calendarDialog2 = !calendarDialog2"
                    ></v-btn>
                    <v-dialog
                        v-model="calendarDialog2"
                        width="auto"
                    >
                        <v-card class="card-radius ContestEdit-card">
                            <v-card-title>{{ t('pages.contests.edit.timePicker') }}</v-card-title>
                            <v-card-text class="d-flex align-center">
                                <v-date-picker
                                    v-model="time2Date"
                                    first-day-of-week="1"
                                    show-week
                                    show-adjacent-months
                                    class="ContestEdit-picker"
                                ></v-date-picker>
                                <v-time-picker
                                    v-model="time2Time"
                                    format="24hr"
                                    use-seconds
                                    scrollable
                                    style="margin-left: 10px;"
                                    class="ContestEdit-picker"
                                ></v-time-picker>
                            </v-card-text>
                        </v-card>
                    </v-dialog>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.contests.edit.type') }}：</p>
                    <v-select
                        v-model="contest.item.type"
                        :items="[{
                            title: 'OI',
                            id: 0
                        }, {
                            title: 'IOI',
                            id: 1
                        }, {
                            title: 'ACM',
                            id: 2
                        }]"
                        item-text="title"
                        item-value="id"
                        dense
                        hide-details
                        density="compact"
                    ></v-select>
                </div>
                <div class="d-flex align-center mb-3">
                    <p style="flex-shrink: 0;">{{ t('pages.contests.edit.tags') }}：</p>
                    <v-autocomplete
                        v-model="contest.item.tags"
                        :items="tags"
                        density="compact"
                        item-title="title"
                        item-value="id"
                        multiple
                        hide-details
                        clearable
                        chips
                    >
                        <template v-slot:chip="{ item, }">
                            <v-chip
                                size="small" 
                                :color="tagsTypeList[(item.raw as any).type].color" 
                                variant="flat"
                            >{{ (item.raw as any).title }}</v-chip>
                        </template>
                    </v-autocomplete>
                    <v-btn
                        class="ContestEdit-button"
                        icon="mdi-plus"
                        size="small"
                        @click="() => addTagDialog = !addTagDialog"
                    ></v-btn>
                    <v-dialog
                        v-model="addTagDialog"
                        width="400px"
                    >
                        <v-card class="card-radius ContestEdit-card">
                            <v-card-title>{{ t('pages.contests.edit.addTag') }}</v-card-title>
                            <v-card-text>
                                <div class="d-flex align-center mb-3">
                                    {{ t('pages.contests.edit.addTagTitle') }}：
                                    <v-text-field
                                        v-model="addTagTitle"
                                        dense
                                        hide-details
                                        clearable
                                        density="compact"
                                    ></v-text-field>
                                </div>
                                <div class="d-flex align-center mb-3">
                                    {{ t('pages.contests.edit.addTagType') }}：
                                    <v-select
                                        v-model="addTagType"
                                        :items="tagsTypeList"
                                        item-title="title"
                                        item-value="id"
                                        dense
                                        hide-details
                                    ></v-select>
                                </div>
                                <div class="d-flex justify-center">
                                    <v-btn 
                                        class="submitButton" 
                                        @click="addTagSubmit()" 
                                    >{{ t('pages.contests.edit.submit') }}</v-btn>
                                </div>
                            </v-card-text>
                        </v-card>
                    </v-dialog>
                </div>
                <div class="d-flex align-center mb-3">
                    <p style="flex-shrink: 0;">{{ t('pages.contests.edit.problems') }}：</p>
                    <v-autocomplete
                        v-model="contest.item.problems"
                        :items="problems"
                        item-title="name"
                        item-value="id"
                        density="compact"
                        multiple
                        hide-details
                        clearable
                    ></v-autocomplete>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.contests.edit.password') }}：</p>
                    <v-text-field
                        v-model="contest.item.password"
                        dense
                        hide-details
                        clearable
                        density="compact"
                        type="password"
                        :placeholder="contest.item.hasPassword ? t('pages.contests.edit.hasPassword') : ''"
                    >
                        <template 
                            v-slot:append
                            v-if="contest.item.hasPassword"
                        >
                            <v-icon
                                color="rgb(244, 164, 96)"
                                icon="mdi-alert"
                                :title="t('pages.contests.edit.hasPassword')"
                            />
                        </template>
                    </v-text-field>
                </div>
            </v-card-text>
        </v-card>
        <v-card class="ContestEdit-card card-radius">
            <v-card-title>
                {{ t('pages.contests.edit.info') }}
                <v-btn
                    size="small"
                    class="ContestEdit-button"
                    @click="isPreview = !isPreview"
                >{{ t('pages.contests.edit.preview') }}</v-btn>
            </v-card-title>
            <v-card-text>
                <MonacoEditor
                    v-if="!isPreview"
                    language="markdown"
                    theme="vs-dark"
                    height="500"
                    v-model:value="contest.item.info"
                    style="margin-top: 5px;"
                    :options="{ wordWrap: 'on' }"
                ></MonacoEditor>
                <div
                    v-else
                    class="markdown-text"
                    v-html="md.render(contest.item.info)"
                    style="min-height: 500px; max-height: 500px; overflow-y: auto;"
                ></div>
                <div class="d-flex justify-center mt-3">
                    <v-btn 
                        class="submitButton" 
                        @click="submit()"
                        :disabled="!enableBtn"
                    >{{ t('pages.contests.edit.submit') }}</v-btn>
                </div>
            </v-card-text>
        </v-card>
    </div>
</template>

<style lang="css">
.ContestEdit-card {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    width: 100%;
    margin-bottom: 20px;
    padding: 5px 10px 0px 10px!important;
}

.ContestEdit-button {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
    margin-left: 10px;
}

.ContestEdit-picker {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
}

.submitButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}
</style>

<style>
.ContestEdit-card input {
    display: block!important;
}

.ContestEdit-card input::-webkit-calendar-picker-indicator {
    color: var(--color-text)!important;
    fill: var(--color-text)!important;
}
</style>