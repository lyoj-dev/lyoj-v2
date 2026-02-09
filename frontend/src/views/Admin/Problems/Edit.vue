<script lang="ts">
import { config, groupColor, tagsTypeList } from '@/config';
import { i18n } from '@/i18n';
import { myFetch, showMsg, sleep } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import ProblemEditCard from '@/components/Problem/EditCard.vue';
import { useRoute } from 'vue-router';
import { locate, router } from '@/router';
import AdminNavigation from '@/components/Admin/Navigation.vue';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var data = (await myFetch(config.apiBase + "/admin/problems/details?ids=" + to.query.ids));
    var languages = (await myFetch(config.apiBase + "/configurations/languages")).items;
    for (var i = 0; i < languages.length; i++) languages[i].id = i;
    var tags = (await myFetch(config.apiBase + "/problems/listAllTags")).items;
    var groups = (await myFetch(config.apiBase + "/users/listAllGroups")).items;
    
    next((e: any) => e.loading({
        data: data.item,
        origin: data.origin,
        languages: languages,
        tags: tags,
        groups: groups
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
const loaded = ref(false);
const origin: any = ref([]);
const allowAddTag = ref(false);

const addTagDialog = ref(false);
const enableBtn = ref(true);

const addTagTitle = ref('');
const addTagType = ref(0);

function loading(data: any) {
    item.value = data.data;
    languages.value = data.languages;
    tagsList.value = data.tags;
    groupsList.value = data.groups;
    allowAddTag.value = data.tags.allowAddTag;
    origin.value = data.origin;

    loaded.value = true;

    item.value.tags = item.value.tags == undefined ? [] : item.value.tags.map((tag: any) => tag.id);
}

defineExpose({ loading });

async function addTagSubmit() {
    var title = addTagTitle.value;
    var type = addTagType.value;
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
    item.value.tags.push(res.id);
    addTagDialog.value = false; 
}

async function submit() {
    enableBtn.value = false;
    var data : any = {
        ids: JSON.parse(route.query.ids as string),
        item: {
        }
    };
    if (item.value.alias != undefined && item.value.alias.trim() != "") data.item.alias = item.value.alias.trim();
    if (item.value.title != undefined && item.value.title.trim() != "") data.item.title = item.value.title.trim();
    if (item.value.bg != undefined && item.value.bg.trim() != "") data.item.bg = item.value.bg.trim();
    if (item.value.descrip != undefined && item.value.descrip.trim() != "") data.item.descrip = item.value.descrip.trim();
    if (item.value.input != undefined && item.value.input.trim() != "") data.item.input = item.value.input.trim();
    if (item.value.output != undefined && item.value.output.trim() != "") data.item.output = item.value.output.trim();
    if (item.value.hint != undefined && item.value.hint.trim() != "") data.item.hint = item.value.hint.trim();
    if (item.value.tags != undefined && item.value.tags.length != 0) data.item.tags = item.value.tags;
    if (item.value.groups != undefined && item.value.groups.length != 0) data.item.groups = item.value.groups;
    if (item.value.langs != undefined && item.value.langs.length != 0) data.item.langs = item.value.langs;
    if (item.value.inputFile != undefined && item.value.inputFile.trim() != "") data.item.inputFile = item.value.inputFile.trim();
    if (item.value.outputFile != undefined && item.value.outputFile.trim() != "") data.item.outputFile = item.value.outputFile.trim();

    var res = await myFetch(config.apiBase + "/admin/problems/edit", {
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
    router.go(0);
}
</script>

<template>
    <div v-if="loaded">
        <AdminNavigation current="problems"></AdminNavigation>
        <v-card class="ProblemEdit-card card-radius">
            <v-card-title>{{ t('pages.problems.edit.baseInfo') }}</v-card-title>
            <v-card-text>
                <div style="color: red; font-weight: 500">
                    <p class="mb-3">{{ t('pages.admin.problems.edit.hint') }}</p>
                    <p class="mb-3">您将要批量修改以下题目：</p>
                    <li class="mb-3" v-for="item in origin">
                        <router-link :to="'/problems/' + item.id">{{ item.alias }} - {{ item.title }}</router-link>
                    </li>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.edit.title') }}：</p>
                    <v-text-field
                        v-model="item.title"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.edit.alias') }}：</p>
                    <v-text-field
                        v-model="item.alias"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.edit.tags') }}：</p>
                    <v-autocomplete
                        v-model="item.tags"
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
                        v-model="item.langs"
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
                        v-model="item.groups"
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
                        v-model="item.inputFile"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.edit.outputFile') }}：</p>
                    <v-text-field
                        v-model="item.outputFile"
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
            v-model:value="item.bg"
        ></ProblemEditCard>
        <ProblemEditCard
            :title="t('pages.problems.edit.description')"
            v-model:value="item.descrip"
        ></ProblemEditCard>
        <ProblemEditCard
            :title="t('pages.problems.edit.input')"
            v-model:value="item.input"
        ></ProblemEditCard>
        <ProblemEditCard
            :title="t('pages.problems.edit.output')"
            v-model:value="item.output"
        ></ProblemEditCard>
        <v-card class="ProblemEdit-card card-radius">
            <v-card-title>{{ t('pages.problems.edit.cases') }}</v-card-title>
            <v-card-text>{{ t('pages.admin.problems.edit.casesIsNotSupported') }}</v-card-text>
        </v-card>
        <ProblemEditCard
            :title="t('pages.problems.edit.hint')"
            v-model:value="item.hint"
        ></ProblemEditCard>
        <v-card class="ProblemEdit-card card-radius" v-if="Number(route.params.id) != 0">
            <v-card-title>
                {{ t('pages.problems.edit.datas') }}
            </v-card-title>
            <v-card-text>{{ t('pages.admin.problems.edit.datasIsNotSupported') }}</v-card-text>
        </v-card>
        <v-card class="ProblemEdit-card card-radius">
            <v-card-title>{{ t('pages.problems.edit.spj') }}</v-card-title>
            <v-card-text>
                <p>{{ t('pages.admin.problems.edit.spjIsNotSupported') }}</p>
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