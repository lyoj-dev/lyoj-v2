<script lang="ts">
import { config, maxDifficulty } from '@/config';
import { locate } from '@/router';
import NProgress from 'nprogress';
import { defineComponent, ref, watchEffect } from 'vue';
import AdminProblemCard from '@/components/Admin/Problem/Card.vue';
import ProblemSearch from '@/components/Problem/Search.vue';
import { myFetch, showMsg, sleep } from '@/utils';
import { i18n } from '@/i18n';
import { useRouter } from 'vue-router';
import AdminNavigation from '@/components/Admin/Navigation.vue';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();
    
    var url = config.apiBase + "/problems/list";
    if ("page" in to.query) url += "?page=" + to.query["page"];
    if ("title" in to.query) url += "&title=" + to.query["title"];
    if ("tags" in to.query) url += "&tags=" + to.query["tags"];
    if ("minDiff" in to.query) url += "&minDiff=" + to.query["minDiff"];
    if ("maxDiff" in to.query) url += "&maxDiff=" + to.query["maxDiff"];
    var json = await myFetch(url);
    var tags = await myFetch(config.apiBase + "/problems/listAllTags");

    next((e: any) => e.loading({
        data: json,
        query: to.query,
        currPage: to.query["page"] ? parseInt(to.query["page"]) : 1,
        tagsList: tags.items
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script setup lang="ts">
const t = i18n.global.t;
const router = useRouter();

const list: any = ref({});
const loaded = ref(false);
const currPage = ref(1);

const tagsList: any = ref([]);

const title = ref("");
const tags: any = ref([]);
const minDiff = ref(0);
const maxDiff = ref(maxDifficulty);

const originalQuery: any = ref({});

const selectedList: any = ref([]);
const fullSelected = ref(0);
const selectedOnly = ref(false);

function loading(data: any) {
    list.value = data.data;
    currPage.value = data.currPage;
    originalQuery.value = data.query;

    for (var item of list.value.items) item.selected = selectedList.value.map((e: any) => e.id).indexOf(item.id) != -1;

    tagsList.value = data.tagsList;

    if ("title" in data.query) title.value = data.query["title"];
    if ("tags" in data.query) tags.value = JSON.parse(data.query["tags"]);
    if ("minDiff" in data.query) minDiff.value = JSON.parse(data.query["minDiff"]);
    if ("maxDiff" in data.query) maxDiff.value = JSON.parse(data.query["maxDiff"]);

    loaded.value = true;
}

defineExpose({ loading });

async function updatePage(data: any) {
    NProgress.start();
    NProgress.inc();

    var url = config.apiBase + "/problems/list";
    if ("page" in data) url += "?page=" + data["page"];
    if ("title" in data) url += "&title=" + data["title"];
    if ("tags" in data) url += "&tags=" + data["tags"];
    if ("minDiff" in data) url += "&minDiff=" + data["minDiff"];
    if ("maxDiff" in data) url += "&maxDiff=" + data["maxDiff"];
    var json = await myFetch(url);
    var tags = await myFetch(config.apiBase + "/problems/listAllTags");

    loading({
        data: json,
        query: data,
        currPage: data["page"] ? parseInt(data["page"]) : 1,
        tagsList: tags.items
    });

    NProgress.done();
}

function search() {
    var data: any = {};
    data.page = 1;
    if (title.value != "") data.title = title.value;
    if (tags.value.length) data.tags = JSON.stringify(tags.value);
    if (minDiff.value != 0) data.minDiff = minDiff.value;
    if (maxDiff.value != maxDifficulty) data.maxDiff = maxDiff.value;

    updatePage(data);
}

function jump() {
    var data: any = {};
    for (var key in originalQuery.value) data[key] = originalQuery.value[key];
    data.page = currPage.value;
    if (selectedOnly.value) {
        loading({
            data: { 
                items: selectedList.value.slice((data.page - 1) * 10, data.page * 10), 
                pageCount: Math.ceil(selectedList.value.length / 10) 
            },
            query: data,
            currPage: data["page"] ? parseInt(data["page"]) : 1,
            tagsList: tagsList.value
        });
    } else {
        updatePage(data);
    }
}

function updateSelectedOnly() {
    selectedOnly.value = !selectedOnly.value;
    currPage.value = 1;
    jump();
}

function addTag(id: number) {
    if (tags.value.indexOf(id) == -1) tags.value = tags.value.concat([ id ]);
}

async function deleteProblem(id: number, name: string) {
    if (!confirm(t('pages.problems.deleteConfirm', { name: name }))) return;
    var res = await myFetch(config.apiBase + "/problems/" + id + "/delete", { method: "POST" });
    showMsg("success", t('pages.problems.deleteSuccess'));
    await sleep(1000);
    updatePage(originalQuery.value);
}

async function cloneProblem(id: number, name: string) {
    if (!confirm(t('pages.admin.problems.list.cloneConfirm', { name: name }))) return;
    var res = await myFetch(config.apiBase + "/admin/problems/clone", { method: "POST", body: JSON.stringify({ id: id }) });
    showMsg("success", t('pages.admin.problems.list.cloneSuccess'));
    await sleep(1000);
    locate("/problems/" + res.id + "/edit");
}

async function deleteProblems() {
    if (selectedList.value.length == 0) return;
    if (!confirm(t('pages.admin.problems.list.deleteSelectedConfirm', { count: selectedList.value.length }))) return;
    var res = await myFetch(config.apiBase + "/admin/problems/delete", { 
        method: "POST", 
        body: JSON.stringify({ ids: selectedList.value.map((e: any) => e.id) }) 
    });
    showMsg("success", t('pages.admin.problems.list.deleteSelectedSuccess', { count: selectedList.value.length }));
    await sleep(1000);
    selectedList.value = [];
    if (selectedOnly.value) updateSelectedOnly();
    else updatePage(originalQuery.value);
}

function editProblems() {
    if (selectedList.value.length == 0) return;
    locate("/admin/problems/edit?ids=[" + selectedList.value.map((e: any) => e.id).join(",") + "]");
}

function selected(id: number, value: boolean) {
    var item = list.value.items.find((item: any) => item.id == id);
    item.selected = value;
    if (value) {
        if (selectedList.value.map((e: any) => e.id).indexOf(id) == -1) selectedList.value.push(item);
    } else {
        var index = selectedList.value.map((e: any) => e.id).indexOf(id);
        if (index != -1) selectedList.value.splice(index, 1);
    }
    selectedList.value.sort((a: any, b: any) => b.id - a.id);
}

function fullSelectedClick() {
    var allSelected = fullSelected.value == list.value.items.length;
    for (var item of list.value.items) selected(item.id, !allSelected);
}

watchEffect(() => {
    if (list.value.items == undefined) return;
    fullSelected.value = 0;
    for (var item of list.value.items) if (item.selected) fullSelected.value++;
});
</script>

<template>
    <div v-if="loaded">
        <AdminNavigation current="problems"></AdminNavigation>
        <ProblemSearch
            v-model:tags="tags"
            v-model:minDiff="minDiff"
            v-model:maxDiff="maxDiff"
            v-model:title="title"
            :tagsList="tagsList"
            @search="search"
        ></ProblemSearch>
        <v-card class="d-flex ProblemCard card-radius">
            <router-link 
                to="/problems/0/edit" 
                class="ProblemCard-status"
                style="color: rgb(36, 140, 36)!important;"
            ><v-icon icon="mdi-plus"></v-icon></router-link>
            <router-link to="/problems/0/edit" class="ProblemCard-problem">{{ t('pages.problems.addProblem') }}</router-link>
        </v-card>
        <v-card class="d-flex ProblemCard card-radius">
            <div
                class="ProblemCard-status"
                @click="fullSelectedClick()"
            >
                <v-icon 
                    :icon="fullSelected == 0 ? 'mdi-checkbox-blank-outline' : (fullSelected == list.items.length ? 'mdi-checkbox-marked' : 'mdi-minus-box')"
                ></v-icon>
            </div>
            <div 
                style="width: calc(87% - 37px); padding: 3px;"
            >{{ t('pages.admin.problems.list.selectedProblem', { count: selectedList.length, selected: fullSelected }) }}</div>
            <div class="ProblemCard-actions d-flex justify-center align-center">
                <v-btn 
                    class="ProblemCard-actionButton" 
                    :icon="selectedOnly ? 'mdi-eye-off' : 'mdi-eye'" 
                    size="x-small"
                    @click="updateSelectedOnly()"
                ></v-btn>
                <v-btn 
                    class="ProblemCard-actionButton" 
                    icon="mdi-pen" 
                    size="x-small"
                    @click="editProblems()"
                ></v-btn>
                <v-btn 
                    class="ProblemCard-actionButton" 
                    icon="mdi-trash-can" 
                    size="x-small"
                    @click="deleteProblems()"
                ></v-btn>
            </div>
        </v-card>
        <AdminProblemCard
            v-for="item in list.items"
            :selected="item.selected"
            :id="item.id"
            :alias="item.alias"
            :title="item.title"
            :tags="item.tags"
            :accepted="item.accepted"
            :total="item.total"
            :difficulty="item.difficulty"
            :allowEdit="item.allowEdit"
            :allowDelete="item.allowDelete"
            @addTag="addTag"
            @deleteProblem="deleteProblem"
            @cloneProblem="cloneProblem"
            @updateSelected="(value) => selected(item.id, value)"
        ></AdminProblemCard>
        <v-pagination
            v-model="currPage"
            :length="list.pageCount"
            :total-visible="8"
            @update:model-value="jump()"
        ></v-pagination>
    </div>
</template>

<style lang="css" scoped>
.ProblemCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 0px 30px;
    width: 100%;
    min-height: 50px;
    margin-bottom: 20px;
    align-items: center;
}

.ProblemCard-status {
    display: block;
    padding: 3px;
    width: 3%;
    position: relative;
    top: -0.8px;
    cursor: pointer;
}

.ProblemCard-actions {
    width: calc(10% + 37px);
    gap: 5px;
}

.ProblemCard-problem {
    width: 97%;
}

.ProblemCard-actionButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}
</style>