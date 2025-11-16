<script lang="ts">
import { config, groupColor, tagsTypeList } from '@/config';
import { i18n } from '@/i18n';
import { loginAs, loginInfo, myFetch, showMsg, sleep } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import ProblemEditCard from '@/components/Problem/EditCard.vue';
import ProblemEditCase from '@/components/Problem/EditCase.vue';
import { useRoute } from 'vue-router';
import { locate } from '@/router';
import { getCookie } from '../../utils';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var data = to.params.id == 0 ? {
            code: 200,
            config: {
                input: "",
                maxMemory: 0,
                maxTime: 0,
                minMemory: 0,
                minTime: 0,
                output: ""
            },
            item: {
                alias: "",
                bg: "",
                cases: [],
                descrip: "",
                difficulty: 0,
                groups: [],
                hint: "",
                id: 0,
                input: "",
                langs: [],
                lastCode: "",
                lastLang: 0,
                output: "",
                tags: [],
                title: ""
            },
            loginAs: 0,
            loginInfo: {
                groups: [],
                permission: 0,
                title: ""
            },
            msg: ""
        } : await myFetch(config.apiBase + "/problems/" + to.params.id);
    var languages = (await myFetch(config.apiBase + "/configurations/languages")).items;
    for (var i = 0; i < languages.length; i++) languages[i].id = i;
    var tags = await myFetch(config.apiBase + "/problems/listAllTags");
    var groups = await myFetch(config.apiBase + "/users/listAllGroups");
    var configs = to.params.id == 0 ? {
            code: 200,
            item: {
                datas: [],
                input: "",
                output: "",
                spj: {
                    compile_cmd: "",
                    exec_name: "",
                    exec_param: "",
                    exec_path: "",
                    source: "",
                    type: 0
                },
                subtasks: []
            },
            loginAs: 0,
            loginInfo: {
                groups: [],
                permission: 0,
                title: ""
            },
            msg: ""
        } : await myFetch(config.apiBase + "/problems/" + to.params.id + "/config");
    var spjs = (await myFetch(config.apiBase + "/configurations/spjs")).items;
    for (var i = 0; i < spjs.length; i++) spjs[i].id = i + 1;
    
    next((e: any) => e.loading({
        data: data,
        languages: languages,
        tags: tags,
        groups: groups,
        config: configs,
        spjs: spjs
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script lang="ts" setup>
const route = useRoute();
const t = i18n.global.t;
const item: any = ref({});
const languages: any = ref([]);
const tagsList: any = ref([]);
const groupsList: any = ref([]);
const configs: any = ref({});
const spjs: any = ref([]);
const loaded = ref(false);
const allowAddTag = ref(false);

const minCase = ref(1);
const maxCase = ref(0);

const minMaxCaseDialog = ref(false);
const addTagDialog = ref(false);
const enableBtn = ref(true);
const enableUploadBtn = ref(true);
const uploadProgress = ref("");

const addTagTitle = ref('');
const addTagType = ref(0);

function loading(data: any) {
    item.value = data.data;
    languages.value = data.languages;
    tagsList.value = data.tags.items;
    groupsList.value = data.groups.items;
    spjs.value = data.spjs;
    configs.value = data.config.item;
    maxCase.value = configs.value.datas.length;
    allowAddTag.value = data.tags.allowAddTag;

    if(item.value.item.cases.length == 0) item.value.item.cases = [ { input: "", output: "" } ];
    loaded.value = true;

    item.value.item.tags = item.value.item.tags.map((tag: any) => tag.id);
}

defineExpose({ loading });

function updateCaseValue(key: string, value: number) {
    for (
        var i = Math.max(1, minCase.value); 
        i <= Math.min(maxCase.value, configs.value.datas.length); 
        i++
    ) configs.value.datas[i - 1][key] = value;
}

async function addTagSubmit() {
    var title = addTagTitle.value;
    var type = addTagType.value;
    console.log(title, type);
    if (title == "") {
        showMsg('error', t('pages.problems.edit.addTagTitleEmpty'));
        addTagDialog.value = false;
        return;
    }
    var res = await myFetch(config.apiBase + "/problems/addTag", {
        method: 'POST',
        body: JSON.stringify({ title: title, type: type })
    }, false);
    if (res.code != 200) {
        showMsg('error', t('pages.problems.edit.addTagFailed'));
        addTagDialog.value = false;
        return;
    }
    showMsg('success', t('pages.problems.edit.addTagSuccess'));
    tagsList.value.push({ id: res.id, title: title, type: type });
    item.value.item.tags.push(res.id);
    addTagDialog.value = false; 
}

function uploadData() {
    var a = document.createElement('input');
    a.type = 'file';
    a.accept = '.zip';
    a.onchange = async function() {
        enableUploadBtn.value = false;
        var reader = new FileReader();
        reader.onload = async function() {
            var data = (this.result as string).split(',')[1];
            const xhr = new XMLHttpRequest();
            xhr.open('POST', config.apiBase + '/problems/' + route.params.id + '/data/upload', true);
            xhr.upload.addEventListener("progress", function(e) {
                if (e.lengthComputable) {
                    var percent = Math.round((e.loaded / e.total) * 100);
                    uploadProgress.value = "(" + percent + "%)";
                }
            });
            xhr.addEventListener("readystatechange", () => {
                if (xhr.readyState == xhr.DONE) {
                    var res = JSON.parse(xhr.responseText);
                    loginAs.value = res.loginAs;
                    loginInfo.value = res.loginInfo;
                    if (res.code != 200) showMsg('error', t('pages.problems.edit.uploadDataFailed'));
                    showMsg('success', t('pages.problems.edit.uploadDataSuccess'));
                    configs.value.datas = [];
                    for (var i = 0; i < res.datas.length; i++) configs.value.datas.push({
                        input: res.datas[i].input,
                        output: res.datas[i].output,
                        time: 1000,
                        memory: 128 * 1024,
                        score: Math.floor(100 / res.datas.length) + (i >= res.datas.length - 100 % res.datas.length ? 1 : 0),
                        subtask: 0
                    });
                    enableUploadBtn.value = true;
                    uploadProgress.value = "";
                    minCase.value = 1;
                    maxCase.value = configs.value.datas.length;
                }
            });
            xhr.setRequestHeader('Content-Type', 'application/json');
            xhr.setRequestHeader('Authorization', 'SessionToken ' + getCookie().session);
            xhr.send(data);
        };
        if (a.files != null) reader.readAsDataURL(a.files[0]);
    };
    a.click();
}

async function downloadData() {
    var url = config.apiBase + '/problems/' + route.params.id + '/data/download';
    var res = await fetch(url, {
        headers: {
            Authorization: 'SessionToken ' + getCookie().session
        }
    });
    var blob = await res.blob();
    var a = document.createElement('a');
    var url = window.URL.createObjectURL(blob);
    a.href = url;
    a.download = "data.zip";
    document.body.appendChild(a);
    a.click();
    window.URL.revokeObjectURL(url);
    a.remove();
}

async function submit() {
    enableBtn.value = false;
    var data = {
        item: {
            id: item.value.item.id,
            alias: item.value.item.alias,
            title: item.value.item.title,
            bg: item.value.item.bg,
            descrip: item.value.item.descrip,
            input: item.value.item.input,
            output: item.value.item.output,
            cases: ((cases) => {
                var res = [];
                for (var i = 0; i < cases.length; i++) {
                    if (cases[i].input == "" && cases[i].output == "") continue;
                    res.push(cases[i]);
                } return res;
            })(item.value.item.cases),
            hint: item.value.item.hint,
            tags: item.value.item.tags,
            groups: item.value.item.groups,
            langs: item.value.item.langs
        },
        config: {
            input: configs.value.input,
            output: configs.value.output,
            datas: configs.value.datas,
            subtasks: ((datas, subtasks) => {
                if (subtasks == undefined) return [];
                var res = [];
                for (var i = 0; i < subtasks.length; i++) {
                    var subtask = subtasks[i];
                    for (var j = 0; j < datas.length; j++) {
                        if (datas[j].subtask == subtask.id) {
                            res.push(subtask); 
                            break;
                        }
                    }
                } return res;
            })(configs.value.datas, configs.value.subtasks),
            spj: configs.value.spj
        }
    };

    var res = await myFetch(config.apiBase + "/problems/" + route.params.id + "/create", {
        method: 'POST',
        body: JSON.stringify(data)
    }, false);
    if (res.code != 200) {
        showMsg('error', t('pages.problems.edit.submitFailed'));
        enableBtn.value = true;
        return;
    }
    showMsg('success', t('pages.problems.edit.submitSuccess'));
    await sleep(1000);
    locate('/problems/' + res.id);
}
</script>

<template>
    <div v-if="loaded">
        <v-card class="ProblemEdit-card card-radius">
            <v-card-title>{{ t('pages.problems.edit.baseInfo') }}</v-card-title>
            <v-card-text>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.edit.title') }}：</p>
                    <v-text-field
                        v-model="item.item.title"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.edit.alias') }}：</p>
                    <v-text-field
                        v-model="item.item.alias"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.edit.tags') }}：</p>
                    <v-autocomplete
                        v-model="item.item.tags"
                        :items="tagsList"
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
                        class="ProblemEdit-addButton"
                        icon="mdi-plus"
                        size="small"
                        v-if="allowAddTag"
                        @click="() => addTagDialog = !addTagDialog"
                    ></v-btn>
                    <v-dialog
                        v-model="addTagDialog"
                        width="400px"
                    >
                        <v-card class="card-radius ProblemEdit-card">
                            <v-card-title>{{ t('pages.problems.edit.addTag') }}</v-card-title>
                            <v-card-text>
                                <div class="d-flex align-center mb-3">
                                    {{ t('pages.problems.edit.addTagTitle') }}：
                                    <v-text-field
                                        v-model="addTagTitle"
                                        dense
                                        hide-details
                                        clearable
                                        density="compact"
                                    ></v-text-field>
                                </div>
                                <div class="d-flex align-center mb-3">
                                    {{ t('pages.problems.edit.addTagType') }}：
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
                                    >{{ t('pages.problems.edit.submit') }}</v-btn>
                                </div>
                            </v-card-text>
                        </v-card>
                    </v-dialog>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.edit.langs') }}：</p>
                    <v-autocomplete
                        v-model="item.item.langs"
                        :items="languages"
                        item-title="name"
                        item-value="id"
                        density="compact"
                        multiple
                        hide-details
                        clearable
                    ></v-autocomplete>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.edit.groups') }}：</p>
                    <v-autocomplete
                        v-model="item.item.groups"
                        :items="groupsList"
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
                                :color="groupColor" 
                                variant="flat"
                            >{{ (item.raw as any).title }}</v-chip>
                        </template>
                    </v-autocomplete>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.edit.inputFile') }}：</p>
                    <v-text-field
                        v-model="configs.input"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.edit.outputFile') }}：</p>
                    <v-text-field
                        v-model="configs.output"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
            </v-card-text>
        </v-card>
        <ProblemEditCard
            :title="t('pages.problems.edit.background')"
            v-model:value="item.item.bg"
        ></ProblemEditCard>
        <ProblemEditCard
            :title="t('pages.problems.edit.description')"
            v-model:value="item.item.descrip"
        ></ProblemEditCard>
        <ProblemEditCard
            :title="t('pages.problems.edit.input')"
            v-model:value="item.item.input"
        ></ProblemEditCard>
        <ProblemEditCard
            :title="t('pages.problems.edit.output')"
            v-model:value="item.item.output"
        ></ProblemEditCard>
        <v-card class="ProblemEdit-card card-radius">
            <v-card-title>{{ t('pages.problems.edit.cases') }}</v-card-title>
            <v-card-text>
                <ProblemEditCase
                    v-for="(item, index) in item.item.cases"
                    :id="index + 1"
                    v-model:input="item.input"
                    v-model:output="item.output"
                ></ProblemEditCase>
                <div class="d-flex justify-center mt-3">
                    <v-btn class="ProblemEdit-appendButton" @click="() => item.item.cases.push({ input: '', output: '' })">
                        {{ t('pages.problems.edit.appendCase') }}
                    </v-btn>
                </div>
            </v-card-text>
        </v-card>
        <ProblemEditCard
            :title="t('pages.problems.edit.hint')"
            v-model:value="item.item.hint"
        ></ProblemEditCard>
        <v-card class="ProblemEdit-card card-radius" v-if="Number(route.params.id) != 0">
            <v-card-title>
                {{ t('pages.problems.edit.datas') }}
                <v-btn
                    size="small"
                    class="ProblemEdit-button"
                    @click="uploadData()"
                    :disabled="!enableUploadBtn"
                >{{ t('pages.problems.edit.uploadData') + uploadProgress }}</v-btn>
                <v-btn
                    size="small"
                    class="ProblemEdit-button"
                    @click="downloadData()"
                >{{ t('pages.problems.edit.downloadData') }}</v-btn>
            </v-card-title>
            <v-card-text>
                <h3>{{ t('pages.problems.edit.dataConfig') }}</h3>
                <div class="d-flex align-center ProblemEdit-dataConfig">
                    <center class="ProblemEdit-dataName">{{ t('pages.problems.edit.data.title') }}</center>
                    <center class="ProblemEdit-dataSingle">{{ t('pages.problems.edit.data.timeLimit') }}</center>
                    <center class="ProblemEdit-dataSingle">{{ t('pages.problems.edit.data.memoryLimit') }}</center>
                    <center class="ProblemEdit-dataSingle">{{ t('pages.problems.edit.data.score') }}</center>
                    <center class="ProblemEdit-dataSingle">{{ t('pages.problems.edit.data.subtask') }}</center>
                </div>
                <div class="d-flex align-center ProblemEdit-dataConfig">
                    <p class="ProblemEdit-dataName d-flex align-center">
                        {{ t('pages.problems.edit.data.oneKey') }}：
                        #{{ minCase }} ~ #{{ maxCase }}
                        <v-btn
                            class="ProblemEdit-addButton"
                            icon="mdi-cog"
                            size="x-small"
                            @click="() => minMaxCaseDialog = !minMaxCaseDialog"
                        ></v-btn>
                        <v-dialog
                            v-model="minMaxCaseDialog"
                            width="auto"
                        >
                            <v-card class="card-radius ProblemEdit-card">
                                <v-card-title>{{ t('pages.problems.edit.data.oneKey') }}</v-card-title>
                                <v-card-text class="d-flex align-center">
                                    #<v-text-field
                                        density="compact"
                                        type="number"
                                        hide-details
                                        single-line
                                        v-model="minCase"
                                    ></v-text-field>&nbsp;~ #<v-text-field
                                        density="compact"
                                        type="number"
                                        hide-details
                                        single-line
                                        v-model="maxCase"
                                    ></v-text-field>    
                                </v-card-text>
                            </v-card>
                        </v-dialog>
                    </p>
                    <center class="ProblemEdit-dataSingle d-flex align-center">
                        <v-text-field
                            density="compact"
                            type="number"
                            hide-details
                            single-line
                            @update:model-value="updateCaseValue('time', parseInt($event))"
                        ></v-text-field>&nbsp;ms
                    </center>
                    <center class="ProblemEdit-dataSingle d-flex align-center">
                        <v-text-field
                            density="compact"
                            type="number"
                            hide-details
                            single-line
                            @update:model-value="updateCaseValue('memory', parseInt($event) * 1024)"
                        ></v-text-field>&nbsp;MB
                    </center>
                    <center class="ProblemEdit-dataSingle d-flex align-center">
                        <v-text-field
                            density="compact"
                            type="number"
                            hide-details
                            single-line
                            @update:model-value="updateCaseValue('score', parseInt($event))"
                        ></v-text-field>
                    </center>
                    <center class="ProblemEdit-dataSingle d-flex align-center">
                        <v-select
                            density="compact"
                            :items="[{ id: 0, title: t('pages.problems.edit.subtask.none') }].concat(configs.subtasks)"
                            item-title="title"
                            item-value="id"
                            hide-details
                            @update:model-value="updateCaseValue('subtask', parseInt($event))"
                        ></v-select>
                    </center>
                </div>
                <div 
                    v-for="(data, index) in configs.datas"
                    class="d-flex align-center ProblemEdit-dataConfig"
                >
                    <p class="ProblemEdit-dataName">{{ "#" + (index + 1) + ": " + data.input + " / " + data.output }}</p>
                    <center class="ProblemEdit-dataSingle d-flex align-center">
                        <v-text-field
                            density="compact"
                            type="number"
                            hide-details
                            single-line
                            v-model="data.time"
                        ></v-text-field>&nbsp;ms
                    </center>
                    <center class="ProblemEdit-dataSingle d-flex align-center">
                        <v-text-field
                            density="compact"
                            type="number"
                            hide-details
                            single-line
                            :model-value="data.memory / 1024"
                            @update:model-value="data.memory = parseInt($event) * 1024"
                        ></v-text-field>&nbsp;MB
                    </center>
                    <center class="ProblemEdit-dataSingle d-flex align-center">
                        <v-text-field
                            density="compact"
                            type="number"
                            hide-details
                            single-line
                            v-model="data.score"
                        ></v-text-field>
                    </center>
                    <center class="ProblemEdit-dataSingle d-flex align-center">
                        <v-select
                            density="compact"
                            v-model="data.subtask"
                            :items="[{ id: 0, title: t('pages.problems.edit.subtask.none') }].concat(configs.subtasks)"
                            item-title="title"
                            item-value="id"
                            hide-details
                        ></v-select>
                    </center>
                </div>
                <h3>{{ t('pages.problems.edit.subtaskConfig') }}</h3>
                <div class="d-flex ProblemEdit-subtaskConfig align-center">
                    <center class="ProblemEdit-subtaskId">{{ t('pages.problems.edit.subtask.id') }}</center>
                    <center class="ProblemEdit-subtaskTitle">{{ t('pages.problems.edit.subtask.title') }}</center>
                    <center class="ProblemEdit-subtaskType">{{ t('pages.problems.edit.subtask.type') }}</center>
                    <center class="ProblemEdit-subtaskDepends">{{ t('pages.problems.edit.subtask.depends') }}</center>
                </div>
                <div 
                    v-for="(subtask, index) in configs.subtasks"
                    class="d-flex ProblemEdit-subtaskConfig align-center"
                >
                    <center class="ProblemEdit-subtaskId d-flex align-center">
                        #<v-text-field
                            density="compact"
                            type="number"
                            hide-details
                            single-line
                            v-model="subtask.id"
                        ></v-text-field>
                    </center>
                    <center class="ProblemEdit-subtaskTitle">
                        <v-text-field
                            density="compact"
                            hide-details
                            single-line
                            v-model="subtask.title"
                        ></v-text-field>
                    </center>
                    <center class="ProblemEdit-subtaskType">
                        <v-select
                            density="compact"
                            v-model="subtask.type"
                            :items="[{
                                title: t('pages.problems.edit.subtask.sumType'),
                                value: 0
                            }, {
                                title: t('pages.problems.edit.subtask.maxType'),
                                value: 1
                            }, {
                                title: t('pages.problems.edit.subtask.minType'),
                                value: 2
                            }, {
                                title: t('pages.problems.edit.subtask.avgType'),
                                value: 3
                            }]"
                            item-title="title"
                            item-value="value"
                            dense
                            hide-details
                        ></v-select>
                    </center>
                    <center class="ProblemEdit-subtaskDepends">
                        <v-autocomplete
                            v-model="subtask.depends"
                            :items="configs.subtasks.slice(0, index)"
                            density="compact"
                            item-title="title"
                            item-value="id"
                            multiple
                            hide-details
                            clearable
                            single-line
                        ></v-autocomplete>
                    </center>
                </div>
                <div class="d-flex justify-center mt-3">
                    <v-btn 
                        class="submitButton" 
                        @click="configs.subtasks.push({ 
                            id: ((subtasks) => {
                                var max = 0;
                                for (var i = 0; i < subtasks.length; i++) 
                                    max = Math.max(max, subtasks[i].id);
                                return max + 1;
                            })(configs.subtasks), 
                            title: 'Subtask #' + (configs.subtasks.length + 1), 
                            type: 2, 
                            depends: [] 
                        })" 
                    >{{ t('pages.problems.edit.subtask.appendSubtask') }}</v-btn>
                </div>
            </v-card-text>
        </v-card>
        <v-card class="ProblemEdit-card card-radius">
            <v-card-title>{{ t('pages.problems.edit.spj') }}</v-card-title>
            <v-card-text>
                <div class="d-flex align-center">
                    {{ t('pages.problems.edit.spjType') }}：
                    <v-select
                        density="compact"
                        v-model="configs.spj.type"
                        :items="[{ id: 0, title: t('pages.problems.edit.customSpj') }].concat(spjs)"
                        item-title="title"
                        item-value="id"
                        hide-details
                        dense
                    ></v-select>
                </div>
                <div v-if="configs.spj.type == 0">
                    <div class="d-flex align-center mt-3">
                        {{ t('pages.problems.edit.spjSource') }}：
                        <v-text-field
                            density="compact"
                            v-model="configs.spj.source"
                            hide-details
                            dense
                            style="font-family: 'Cascadia Mono', monospace!important;"
                        ></v-text-field>
                    </div>
                    <div class="d-flex align-center mt-3">
                        {{ t('pages.problems.edit.spjCompileCmd') }}：
                        <v-text-field
                            density="compact"
                            v-model="configs.spj.compile_cmd"
                            hide-details
                            dense
                            style="font-family: 'Cascadia Mono', monospace!important;"
                        ></v-text-field>
                    </div>
                    <div class="d-flex align-center mt-3">
                        {{ t('pages.problems.edit.spjExecName') }}：
                        <v-text-field
                            density="compact"
                            v-model="configs.spj.exec_name"
                            hide-details
                            dense
                            style="font-family: 'Cascadia Mono', monospace!important;"
                        ></v-text-field>
                    </div>
                    <div class="d-flex align-center mt-3">
                        {{ t('pages.problems.edit.spjExecParam') }}：
                        <v-text-field
                            density="compact"
                            v-model="configs.spj.exec_param"
                            hide-details
                            dense
                            style="font-family: 'Cascadia Mono', monospace!important;"
                        ></v-text-field>
                    </div>
                </div>
                <div class="d-flex justify-center mt-3">
                    <v-btn 
                        class="submitButton" 
                        @click="submit()"
                        :disabled="!enableBtn"
                    >{{ t('pages.problems.edit.submit') }}</v-btn>
                </div>
            </v-card-text>
        </v-card>
    </div>
</template>

<style lang="css" scoped>
.ProblemEdit-card {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    width: 100%;
    margin-bottom: 20px;
    padding: 5px 10px 0px 10px;
}

.ProblemEdit-appendButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}

.ProblemEdit-addButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
    margin-left: 10px;
}

.ProblemEdit-button {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
    position: relative;
    top: -1.5px;
    margin-left: 5px;
}

.ProblemEdit-dataConfig, .ProblemEdit-subtaskConfig {
    margin: 10px 0px;
}

.ProblemEdit-dataName {
    padding: 0px 10px;
    width: 28%;
}

.ProblemEdit-dataSingle {
    padding: 0px 10px;
    width: 18%;
}

.ProblemEdit-subtaskId {
    padding: 0px 10px;
    width: 10%;
}

.ProblemEdit-subtaskTitle {
    padding: 0px 10px;
    width: 20%;
}

.ProblemEdit-subtaskType {
    padding: 0px 10px;
    width: 20%;
}

.ProblemEdit-subtaskDepends {
    padding: 0px 10px;
    width: 50%;
}

h3 {
    margin: 10px 0px;
}

.submitButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}

p {
    flex-shrink: 0;
}
</style>