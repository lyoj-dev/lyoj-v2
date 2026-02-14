<script lang="ts">
import { defineComponent, ref, watchEffect } from "vue";
import { goto } from "@/router";
import { config } from '@/config.ts'
import NProgress from 'nprogress';
import { JudgeResultInfo } from '@/shared';
import AdminSubmissionCard from '@/components/Admin/Submission/Card.vue'
import SubmissionSearch from "@/components/Submission/Search.vue";
import { myFetch, showMsg } from "@/utils";
import { onBeforeRouteLeave } from "vue-router";
import AdminNavigation from '@/components/Admin/Navigation.vue';
import { i18n } from "@/i18n";

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var url = config.apiBase + "/admin/submissions/list";
    if ("page" in to.query) url += "?page=" + to.query["page"];
    else url += "?page=1";
    if ("problems" in to.query) url += "&problems=" + to.query["problems"];
    if ("users" in to.query) url += "&users=" + to.query["users"];
    if ("languages" in to.query) url += "&languages=" + to.query["languages"];
    if ("status" in to.query) url += "&status=" + to.query["status"];
    var json = await myFetch(url);
    var problems = await myFetch(config.apiBase + "/problems/listAll");
    var users = await myFetch(config.apiBase + "/users/listAll");
    for (var i = 0; i < problems.items.length; i++) problems.items[i].name = problems.items[i].alias + " - " + problems.items[i].title;
    var languages = await myFetch(config.apiBase + "/configurations/languages");
    for (var i = 0; i < languages.items.length; i++) languages.items[i].id = i;
    
    next((e: any) => e.loading({
        data: json,
        query: to.query,
        currPage: "page" in to.query ? parseInt(to.query["page"]) : 1,
        problemsList: problems,
        usersList: users,
        languagesList: languages
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script setup lang="ts">
const t = i18n.global.t;

const list: any = ref([]);
const loaded = ref(false);
const problemsList = ref([]);
const usersList = ref([]);
const languagesList = ref([]);

const searchProblems = ref([]);
const searchUsers = ref([]);
const searchLanguages = ref([]);
const searchStatus = ref([]);
const currPage = ref(1);

const originalQuery: any = ref({});

const selectedOnly = ref(false);
const fullSelected = ref(0);
const selectedList: any = ref([]);

var ws: WebSocket;

function loading(data: any) {
    list.value = data.data;
    problemsList.value = data.problemsList.items;
    usersList.value = data.usersList.items;
    languagesList.value = data.languagesList.items;
    originalQuery.value = data.query;

    searchProblems.value = "problems" in data.query ? JSON.parse(data.query["problems"]) : [];
    searchUsers.value = "users" in data.query ? JSON.parse(data.query["users"]) : [];
    searchLanguages.value = "languages" in data.query ? JSON.parse(data.query["languages"]) : [];
    searchStatus.value = "status" in data.query ? JSON.parse(data.query["status"]) : [];
    currPage.value = data.currPage;

    for (var item of list.value.items) item.selected = selectedList.value.map((e: any) => e.id).indexOf(item.id) != -1;

    var unJudged: number[] = [];
    for (var i = 0; i < data.data.items.length; i++) 
        if (data.data.items[i].judged == false) unJudged.push(data.data.items[i].id); 
    var count = 0;
    
    if (unJudged.length) {
        ws = new WebSocket(config.wsBase + "/submissions/list");
        ws.onopen = (e) => {
            ws.send(JSON.stringify(unJudged));
        };
        ws.onmessage = function(e) {
            var json = JSON.parse(e.data);

            var index = 0;
            for (var i = 0; i < list.value.items.length; i++)
                if (list.value.items[i].id == json.sid) index = i;
            if (json.type == 0 || json.type == 1) {
                list.value.items[index].score = json.score;
                list.value.items[index].statusType = json.status;
                list.value.items[index].status = JudgeResultInfo[json.status as number];
            } else if (json.type == 2) {
                list.value.items[index].score = json.totalScore;
            } else if (json.type == 3) {
                list.value.items[index].score = json.totalScore;
                list.value.items[index].statusType = 11;
                list.value.items[index].status = json.totalStatus;
            }

            if (json.type == 1) count++;
            if (count == unJudged.length) ws.close();
        };    
    }

    loaded.value = true;
}
defineExpose({ loading })

async function updatePage(data: any) {
    if (ws != undefined) ws.close();

    NProgress.start();
    NProgress.inc();

    history.pushState(null, "", "/admin/submissions/list?page=" + (data["page"] || 1) + 
        ("problems" in data ? "&problems=" + data["problems"] : "") + 
        ("users" in data ? "&users=" + data["users"] : "") + 
        ("languages" in data ? "&languages=" + data["languages"] : "") + 
        ("status" in data ? "&status=" + data["status"] : "")
    );
    var url = config.apiBase + "/admin/submissions/list";
    if ("page" in data) url += "?page=" + data["page"];
    else url += "?page=1";
    if ("problems" in data) url += "&problems=" + data["problems"];
    if ("users" in data) url += "&users=" + data["users"];
    if ("languages" in data) url += "&languages=" + data["languages"];
    if ("status" in data) url += "&status=" + data["status"];
    var json = await myFetch(url);
    var problems = await myFetch(config.apiBase + "/problems/listAll");
    var users = await myFetch(config.apiBase + "/users/listAll");
    for (var i = 0; i < problems.items.length; i++) problems.items[i].name = problems.items[i].alias + " - " + problems.items[i].title;
    var languages = await myFetch(config.apiBase + "/configurations/languages");
    for (var i = 0; i < languages.items.length; i++) languages.items[i].id = i;
    
    loading({
        data: json,
        query: data,
        currPage: "page" in data ? parseInt(data["page"]) : 1,
        problemsList: problems,
        usersList: users,
        languagesList: languages
    });

    NProgress.done();
}

function search() {
    var data: any = {};
    data.page = 1;
    if (searchProblems.value.length) data.problems = JSON.stringify(searchProblems.value);
    if (searchUsers.value.length) data.users = JSON.stringify(searchUsers.value);
    if (searchLanguages.value.length) data.languages = JSON.stringify(searchLanguages.value);
    if (searchStatus.value.length) data.status = JSON.stringify(searchStatus.value);
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
            problemsList: { items: problemsList.value },
            usersList: { items: usersList.value },
            languagesList: { items: languagesList.value }
        });
    } else {
        updatePage(data);
    }
}

async function rejudgeSubmission(id: number) {
    var res = await myFetch(config.apiBase + '/admin/submissions/rejudge', { method: "POST", body: JSON.stringify({ ids: [ id ] }) });
    showMsg("success", t('pages.admin.submissions.list.rejudgeSuccess'));
    jump();
}

async function rejudgeSubmissions() {
    if (!confirm(t('pages.admin.submissions.list.rejudgeSelectedConfirm', { count: selectedList.value.length }))) return;
    var res = await myFetch(config.apiBase + '/admin/submissions/rejudge', { method: "POST", body: JSON.stringify({ ids: selectedList.value.map((e: any) => e.id) }) });
    showMsg("success", t('pages.admin.submissions.list.rejudgeSelectedSuccess'));
    jump();
}

function updateSelectedOnly() {
    selectedOnly.value = !selectedOnly.value;
    currPage.value = 1;
    jump();
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
    for (var item of list.value.items) {
        var index = selectedList.value.map((e: any) => e.id).indexOf(item.id);
        if (index != -1) selectedList.value[index] = item;
    }
});

onBeforeRouteLeave((to, from, next) => {
    if (ws != undefined) ws.close();
    next();
});
</script>

<template>
    <div v-if="loaded">
        <AdminNavigation current="submissions"></AdminNavigation>
        <SubmissionSearch
            v-model:problems="searchProblems"
            v-model:users="searchUsers"
            v-model:languages="searchLanguages"
            v-model:status="searchStatus"
            :problemsList="problemsList"
            :usersList="usersList"
            :languagesList="languagesList"
            :disableSearch="selectedOnly"
            @search="search"
        ></SubmissionSearch>
        <v-card class="d-flex SubmissionCard card-radius">
            <div
                class="SubmissionCard-status"
                @click="fullSelectedClick()"
            >
                <v-icon 
                    :icon="fullSelected == 0 ? 'mdi-checkbox-blank-outline' : (fullSelected == list.items.length ? 'mdi-checkbox-marked' : 'mdi-minus-box')"
                ></v-icon>
            </div>
            <div 
                style="width: 87%; padding: 3px;"
            >{{ t('pages.admin.submissions.list.selectedSubmission', { count: selectedList.length, selected: fullSelected }) }}</div>
            <div class="SubmissionCard-actions d-flex justify-center align-center">
                <v-btn 
                    class="SubmissionCard-actionButton" 
                    icon="mdi-restart" 
                    size="x-small"
                    :title="t('pages.admin.submissions.list.rejudgeSelectedButton')"
                    :disabled="selectedList.length == 0"
                    @click="rejudgeSubmissions()"
                ></v-btn>
                <v-btn 
                    class="SubmissionCard-actionButton" 
                    :icon="selectedOnly ? 'mdi-eye-off' : 'mdi-eye'" 
                    size="x-small"
                    :title="t('pages.admin.submissions.list.' + (selectedOnly ? 'showAllButton' : 'showSelectedButton'))"
                    :disabled="selectedList.length == 0"
                    @click="updateSelectedOnly()"
                ></v-btn>
            </div>
        </v-card>
        <AdminSubmissionCard
            v-for="item in list.items"
            :id="item.id"
            :pid="item.pid"
            :uid="item.uid"
            :problem="item.problem"
            :user="item.user"
            :statusType="item.statusType"
            :status="item.status"
            :score="item.score"
            :selected="item.selected"
            @updateSelected="(value) => selected(item.id, value)"
            @rejudge="rejudgeSubmission(item.id)"
        ></AdminSubmissionCard>
        <v-pagination
            v-model="currPage"
            :length="list.pageCount"
            :total-visible="8"
            @update:model-value="jump()"
        ></v-pagination>
    </div>
</template>

<style lang="css" scoped>
.SubmissionCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 0px 30px;
    width: 100%;
    min-height: 50px;
    margin-bottom: 20px;
    align-items: center;
}

.SubmissionCard-status {
    display: block;
    padding: 3px;
    width: 3%;
    position: relative;
    top: -0.8px;
    cursor: pointer;
}

.SubmissionCard-actions {
    width: 10%;
    gap: 5px;
}

.SubmissionCard-actionButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}
</style>