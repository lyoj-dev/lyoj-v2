<script lang="ts">
import { defineComponent, ref } from "vue";
import { goto, get } from "@/router";
import { config } from '@/config.ts'
import NProgress from 'nprogress';
import { JudgeResultInfo } from '@/shared';
import SubmissionCard from '@/components/Submission/Card.vue'
import SubmissionSearch from "@/components/Submission/Search.vue";

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();
    console.log(to);
    var url = config.apiBase + "/submissions/list";
    if ("page" in to.query) url += "?page=" + to.query["page"];
    else url += "?page=1";
    if ("problems" in to.query) url += "&problems=" + to.query["problems"];
    if ("languages" in to.query) url += "&languages=" + to.query["languages"];
    if ("status" in to.query) url += "&status=" + to.query["status"];
    var response = await fetch(url);
    if (response.status != 200) goto("error", { code: response.status });
    var json = await response.json();
    console.log(json);

    var response = await fetch(config.apiBase + "/problems/listAll");
    if (response.status != 200) goto("error", { code: response.status });
    var problems = await response.json();
    for (var i = 0; i < problems.items.length; i++) problems.items[i].name = problems.items[i].alias + " - " + problems.items[i].title;

    var response = await fetch(config.apiBase + "/configurations/languages");
    if (response.status != 200) goto("error", { code: response.status });
    var languages = await response.json();
    for (var i = 0; i < languages.items.length; i++) languages.items[i].id = i;
    next((e: any) => e.loading({
        data: json,
        query: to.query,
        currPage: "page" in to.query ? parseInt(to.query["page"]) : 1,
        problemsList: problems,
        languagesList: languages
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
const languagesList = ref([]);

const searchProblems = ref([]);
const searchLanguages = ref([]);
const searchStatus = ref([]);
const currPage = ref(1);

const originalQuery: any = ref({});

function loading(data: any) {
    list.value = data.data;
    problemsList.value = data.problemsList.items;
    languagesList.value = data.languagesList.items;
    originalQuery.value = data.query;

    searchProblems.value = "problems" in data.query ? JSON.parse(data.query["problems"]) : [];
    searchLanguages.value = "languages" in data.query ? JSON.parse(data.query["languages"]) : [];
    searchStatus.value = "status" in data.query ? JSON.parse(data.query["status"]) : [];
    currPage.value = data.currPage;

    var unJudged: number[] = [];
    for (var i = 0; i < data.data.items.length; i++) 
        if (data.data.items[i].judged == false) unJudged.push(data.data.items[i].id); 
    var count = 0;
    
    if (unJudged.length) {
        var ws = new WebSocket(config.wsBase + "/submissions/list");
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
    if (searchLanguages.value.length) data.languages = JSON.stringify(searchLanguages.value);
    if (searchStatus.value.length) data.status = JSON.stringify(searchStatus.value);
    goto('submissionsList', data);
}

function jump() {
    var data: any = {};
    for (var key in originalQuery.value) data[key] = originalQuery.value[key];
    data.page = currPage.value;
    goto('submissionsList', data);
}
</script>

<template>
    <div v-if="loaded">
        <SubmissionSearch
            v-model:problems="searchProblems"
            v-model:languages="searchLanguages"
            v-model:status="searchStatus"
            :problemsList="problemsList"
            :languagesList="languagesList"
            @search="search"
        ></SubmissionSearch>
        <SubmissionCard
            v-for="item in list.items"
            :id="item.id"
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