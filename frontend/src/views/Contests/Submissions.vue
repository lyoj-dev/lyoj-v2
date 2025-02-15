<script lang="ts">
import { defineComponent, ref } from "vue";
import { goto } from "@/router";
import { config } from '@/config.ts'
import NProgress from 'nprogress';
import { JudgeResultInfo } from '@/shared';
import SubmissionCard from '@/components/Submission/Card.vue'
import SubmissionSearch from "@/components/Submission/Search.vue";
import ContestTitle from '@/components/Contest/Title.vue';
import ContestNavigation from '@/components/Contest/Navigation.vue';
import { myFetch } from "@/utils";
import { onBeforeRouteLeave, useRoute } from "vue-router";

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var url = config.apiBase + "/contests/" + to.params.id + "/submissions/list";
    if ("page" in to.query) url += "?page=" + to.query["page"];
    else url += "?page=1";
    if ("problems" in to.query) url += "&problems=" + to.query["problems"];
    if ("users" in to.query) url += "&users=" + to.query["users"];
    if ("languages" in to.query) url += "&languages=" + to.query["languages"];
    if ("status" in to.query) url += "&status=" + to.query["status"];
    var json = await myFetch(url);
    var problems = await myFetch(config.apiBase + "/contests/" + to.params.id + "/problems/listAll");
    for (var i = 0; i < problems.items.length; i++) problems.items[i].name = "#" + problems.items[i].id + " - " + problems.items[i].title;
    var users = await myFetch(config.apiBase + "/contests/" + to.params.id + "/users/listAll");
    var languages = await myFetch(config.apiBase + "/configurations/languages");
    for (var i = 0; i < languages.items.length; i++) languages.items[i].id = i;
    var contest = await myFetch(config.apiBase + '/contests/' + to.params.id);
    
    next((e: any) => e.loading({
        data: json,
        query: to.query,
        currPage: "page" in to.query ? parseInt(to.query["page"]) : 1,
        problemsList: problems,
        usersList: users,
        languagesList: languages,
        contest: contest
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script setup lang="ts">
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
const contest: any = ref({});

const originalQuery: any = ref({});
var ws: WebSocket;
const route = useRoute();

function loading(data: any) {
    list.value = data.data;
    problemsList.value = data.problemsList.items;
    usersList.value = data.usersList.items;
    languagesList.value = data.languagesList.items;
    originalQuery.value = data.query;
    contest.value = data.contest;

    searchProblems.value = "problems" in data.query ? JSON.parse(data.query["problems"]) : [];
    searchUsers.value = "users" in data.query ? JSON.parse(data.query["users"]) : [];
    searchLanguages.value = "languages" in data.query ? JSON.parse(data.query["languages"]) : [];
    searchStatus.value = "status" in data.query ? JSON.parse(data.query["status"]) : [];
    currPage.value = data.currPage;

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
defineExpose({ loading, list })

function search() {
    var data: any = {};
    data.page = 1;
    if (searchProblems.value.length) data.problems = JSON.stringify(searchProblems.value);
    if (searchUsers.value.length) data.users = JSON.stringify(searchUsers.value);
    if (searchLanguages.value.length) data.languages = JSON.stringify(searchLanguages.value);
    if (searchStatus.value.length) data.status = JSON.stringify(searchStatus.value);
    goto('contestSubmissionsList', data, { id: route.params.id });
}

function jump() {
    var data: any = {};
    for (var key in originalQuery.value) data[key] = originalQuery.value[key];
    data.page = currPage.value;
    goto('contestSubmissionsList', data, { id: route.params.id });
}

onBeforeRouteLeave((to, from, next) => {
    if (ws != undefined) ws.close();
    next();
});
</script>

<template>
    <div v-if="loaded">
        <ContestNavigation 
            current="submissions" 
            :id="route.params.id" 
            :identity="contest.identity" 
            :signup="contest.signup"
            :endTime="contest.item.starttime + contest.item.duration"
        ></ContestNavigation>
        <ContestTitle
            :id="contest.item.id"
            :title="contest.item.title"
            :startTime="contest.item.starttime"
            :endTime="contest.item.starttime + contest.item.duration"
            :problems="contest.item.problemNumber"
            :signups="contest.item.signups"
        ></ContestTitle>
        <SubmissionSearch
            v-model:problems="searchProblems"
            v-model:users="searchUsers"
            v-model:languages="searchLanguages"
            v-model:status="searchStatus"
            :problemsList="problemsList"
            :usersList="usersList"
            :languagesList="languagesList"
            @search="search"
        ></SubmissionSearch>
        <SubmissionCard
            v-for="item in list.items"
            :id="item.id"
            :cid="route.params.id"
            :pid="item.pid"
            :uid="item.uid"
            :problem="item.problem"
            :user="item.user"
            :statusType="item.statusType"
            :status="item.status"
            :score="item.score"
        ></SubmissionCard>
        <v-pagination
            v-model="currPage"
            :length="list.pageCount"
            :total-visible="8"
            @update:model-value="jump()"
        ></v-pagination>
    </div>
</template>